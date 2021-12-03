from machine import Pin, SPI
from array import array
from spi import MySPI
from config import ADC_CHANNELS

class MCP3464:
    dev_id = 0
    DEV_ADDRESS = 0x01
    REG_MAP = {
        'ADCDATA': 0x0,
        'CONFIG0': 0x1,
        'CONFIG1': 0x2,
        'CONFIG2': 0x3,
        'CONFIG3': 0x4,
        'IRQ': 0x5,
        'MUX': 0x6,
        'SCAN': 0x7,
        'TIMER': 0x8,
        'OFFSETCAL': 0x9,
        'GAINCAL': 0xA,
        'LOCK': 0xD,
        'CRCREG': 0xF
    }
    CMD_TYPES = {
        'SINGLE_READ': 0b01,
        'WRITE': 0b10
    }
    NUM_CHANNELS = 8

    def __init__(self, spi, irq_pin_num):
        self._spi = spi
        self.data_ready = False
        self.id = MCP3464.dev_id
        MCP3464.dev_id += 1
        self.fast_commands = {
            'reset': (MCP3464.DEV_ADDRESS << 6) | (0x38) 
        }
        self.data = [-1] * MCP3464.NUM_CHANNELS
        self._irq_pin = Pin(irq_pin_num, Pin.IN)
        self._irq_pin.irq(self.data_ready_handler, Pin.IRQ_FALLING)

    # IRQ Handler, special rules
    def data_ready_handler(self):
        self.data_ready = True

    def init(self):
        self.config()

    def config(self):
        # TODO Concern: signed values might cause issues?
        # Set CONFIG1 to [7:6] = 0b00, [5:2] = 0b0011, [1:0] = 0b00
        config1 = [0b0000_1100]
        self.write(MCP3464.REG_MAP['CONFIG1'], config1)
        # Set CONFIG2 to [7:6] = 0b10, [5:3] = 0b001, [2] = 0b0, [1:0] = 0b11
        config2 = [0b1000_1011]
        self.write(MCP3464.REG_MAP['CONFIG2'], config2)
        # Set CONFIG3 to [7:6] = 0b11, [5:4] = 0b11, [3] = 0b0, [2] = 0b0, [1] = 0b0, [0] = 0
        config3 = [0b1111_0000]
        self.write(MCP3464.REG_MAP['CONFIG3'], config3)
        # Set IRQ (Reg 8-6) to [7] = 0b0, [6] = 0b1, [5] = 0b1, [4] = 0b1, [3:2] = 0b01, [1] = 0b1, [0] = 0b1
        irq = [0b0111_0110]
        self.write(MCP3464.REG_MAP['IRQ'], irq)
        # Set SCAN (0x7) to 0b0000_0000_0000_0000_1111_1111
        scan_channels = 0x00
        for channel in ADC_CHANNELS:
            scan_channels |= 0b1 << channel
        scan = [0b0000_0000, 0b0000_0000, scan_channels]
        self.write(MCP3464.REG_MAP['SCAN'], scan)
        # Set CONFIG0 to [7:6] = 0b11, [5:4] = 0b10, [3:2] = 0b00, [1:0] = 0b11
        config0 = [0b1110_0011]
        self.write(MCP3464.REG_MAP['CONFIG0'], config0)

    def parse_status_byte(self, status):
        npor_status = status & 0b0000_0001
        ncrccfg_status = (status & 0b0000_0010) >> 1
        ndr_status =     (status & 0b0000_0100) >> 2
        ndev_addr0 =     (status & 0b0000_1000) >> 3
        dev_addr =       (status & 0b0011_0000) >> 4
        return (npor_status, ncrccfg_status, ndr_status)

    def build_read_buf(self, reg, size):
        cmd = MCP3464.DEV_ADDRESS << 6 | reg << 2 | MCP3464.CMD_TYPES['SINGLE_READ']
        payload = [0] * (size + 1)
        payload[0] = cmd
        return array('B', payload)

    def build_write_buf(self, reg, payload):
        cmd = MCP3464.DEV_ADDRESS << 6 | reg << 2 | MCP3464.CMD_TYPES['WRITE']
        payload.insert(0, cmd)
        return array('B', payload)

    def write(self, reg, payload):
        """ Write data to MCP and return status byte """

        write_data = self.build_write_buf(reg, payload)
        read_data = self._spi.write_readinto(write_data)
        return read_data[0]

    def static_read(self, reg: int, num_bytes: int):
        write_data = self.build_read_buf(reg, num_bytes)
        read_data = self._spi.write_readinto(write_data)
        return read_data

    def read_data(self):
        resp = self.static_read(self, MCP3464.REG_MAP['ADCDATA'], 4)
        (_, _, ndr_status) = self.parse_status_byte(resp[0])
        if not ndr_status:
            data = (resp[2] << 8) & resp[1]
            chan = 0xF0 & resp[4]
            self.data[chan] = data
            return chan
        else:
            return -1