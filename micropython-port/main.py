import time
import sys
from spi import MySPI
from mcp3464 import MCP3464
from machine import SPI, Pin, Timer
from timer import MyTimer
from config import DEBUG

def init():
    global spi, adc, timer
    print("Enter")
    spi = MySPI()
    print("one")
    adc = MCP3464(spi, irq_pin_num=6)
    print("two")
    timer = MyTimer()
    print("three")
    timer.start()
    print("four")

try:
    init()
except:
    print("PRE-INIT ERROR")
    while True:
        pass

try:
    while True:
        if adc.data_ready:
            chan = adc.read_data()
            if chan != -1:
                sys.stdout.buffer.write(format_packet(adc.data[chan]))
        time.sleep_us(10)
except:
    timer.error()
