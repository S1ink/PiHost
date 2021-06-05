import var
import gpiozero as gz
import time

intv = 0.25
exit = False

pc_power = gz.DigitalOutputDevice(pin=var.gpin.pc_power)
pc_reset = gz.DigitalOutputDevice(pin=var.gpin.pc_reset)
pc_status = gz.DigitalInputDevice(pin=var.gpin.pc_status, pull_up=True)

def power():
    pc_power.on()
    time.sleep(intv)
    pc_power.off()

def reset():
    pc_reset.on()
    time.sleep(intv)
    pc_reset.off()

def status():
    return pc_status.is_active


# while not exit:
#     inp = input("type 'power', 'reset' or 'exit'")
#     if inp == "power":
#         power()
#         print('pressed power button')
#     elif inp == "reset":
#         reset()
#         print('pressed reset button')
#     elif inp == 'exit':
#         exit = True
#     else:
#         break
while True:
    print(status())
    time.sleep(1)