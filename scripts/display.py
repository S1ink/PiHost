import RPi.GPIO as GPIO
from gpiozero import DigitalOutputDevice as digital
import functions as fn
import time

#TEMPORARY
class pins:
    top = 23
    tl = 24
    tr = 5
    mid = 6
    bl = 13
    br = 19
    bot = 26

    first = 16
    second = 20
    third = 21

# GPIO.setmode(GPIO.BCM)
# GPIO.setup(pins.top, GPIO.OUT, initial = GPIO.LOW)

top = digital(pins.top)
tl = digital(pins.tl)
tr = digital(pins.tr)
mid = digital(pins.mid)
bl = digital(pins.bl)
br = digital(pins.br)
bot = digital(pins.bot)

first = digital(pins.first)
second = digital(pins.second)
third = digital(pins.third)

section = [first, second, third]

class characters:

    def zero(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        tr.on()
        bl.on()
        br.on()
        bot.on()
        return 0

    def one(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        tr.on()
        br.on()
        return 1

    def two(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tr.on()
        mid.on()
        bl.on()
        bot.on()
        return 2

    def three(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        mid.on()
        bl.on()
        bot.on()
        return 3

    def four(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        tl.on()
        tr.on()
        mid.on()
        br.on()
        return 4
    
    def five(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        mid.on()
        br.on()
        bot.on()
        return 5

    def six(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        mid.on()
        bl.on()
        br.on()
        bot.on()
        return 6

    def seven(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tr.on()
        br.on()
        return 7

    def eight(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        tr.on()
        mid.on()
        bl.on()
        br.on()
        bot.on()
        return 8

    def nine(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.on()
        tl.on()
        tr.on()
        mid.on()
        br.on()
        bot.on()
        return 9

    def refresh(sections:list = [0,1,2]):
        for place in sections:
            section[place].on
        top.off()
        tl.off()
        tr.off()
        mid.off()
        bl.off()
        br.off()
        bot.off()

nums1 = [
    characters.zero([0]), 
    characters.one([0]), 
    characters.two([0]), 
    characters.three([0]), 
    characters.four([0]), 
    characters.five([0]), 
    characters.six([0]), 
    characters.seven([0]), 
    characters.eight([0]), 
    characters.nine([0])
]

nums2 = [
    characters.zero([1]), 
    characters.one([1]), 
    characters.two([1]), 
    characters.three([1]), 
    characters.four([1]), 
    characters.five([1]), 
    characters.six([1]), 
    characters.seven([1]), 
    characters.eight([1]), 
    characters.nine([1])
]

nums3 = [
    characters.zero([2]), 
    characters.one([2]), 
    characters.two([2]), 
    characters.three([2]), 
    characters.four([2]), 
    characters.five([2]), 
    characters.six([2]), 
    characters.seven([2]), 
    characters.eight([2]), 
    characters.nine([2])
]

def num1(num):
    return nums1[num]

def num2(num):
    return nums2[num]

def num3(num):
    return nums3[num]