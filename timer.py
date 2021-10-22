from machine import Timer, Pin

class MyTimer:
    
    def __init__(self):
        self.timer = Timer()
        self.led = Pin(25, Pin.OUT)

    def tick(self, timer):
        self.led.toggle()

    def start(self):
        self.timer.init(freq=1, mode=Timer.PERIODIC, callback=self.tick)
    
    def error(self):
        self.timer.deinit()
        self.led.on()
