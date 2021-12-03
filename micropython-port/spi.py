from machine import Pin, SPI

class MySPI:
    is_init: bool = False

    def __init__(self):
        self._spi = SPI(0, baudrate=1000000, polarity=1, phase=1,
                bits=8, sck=Pin(2), mosi=Pin(3), miso=Pin(4))
        self._cs = Pin(5, Pin.OUT, value=1)

    def write_readinto(write_data):
        read_data = [0] * len(write_data)
        self._cs.off()
        self._spi.write_readinto(write_data, read_data)
        self._cs.on()
        return read_data
    
    # def write(self, buff: bytes) -> None:
    #     self._cs.off()
    #     self._spi.write(buff)
    #     self._cs.on()

    # def read(self, nbytes: int) -> bytes:
    #     self._cs.off()
    #     data = self._spi.read(nbytes)
    #     self._cs.on()
    #     return data
