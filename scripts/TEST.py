import os
import subprocess
import time
import datetime
import functions as fn
import psutil
from gpiozero import DigitalOutputDevice as Digital
import threading
from queue import Queue

#subprocess.run(['sudo', 'apt-get', 'update'])
# x=0
# while (x < 10):
#     x+=1
#     functions.logger.appendcsv('text.csv', [functions.info.datetime2(), functions.info.cputemp()])
#     time.sleep(5)

#fn.logger.appendcsv(f'/data/logs/{fn.info.date()}.csv', [fn.info.daytime(), fn.info.cpusage(), fn.info.cpufreq(), fn.info.cputemp()])

# def fn1():
#     print("fn1 was run")
#     return 1

# def fn2():
#     print("fn2 was run")
#     return 2

# print("debug")

# fns = [fn1(), fn2()]

# print("debug")

# print(fns[0])

# display = Digital(26)

# while True:
#     display.on()
#     time.sleep(1)
#     # display.off()
#     # time.sleep(1)
#     print("looped")

# def timer(amount:float = 10):
#     time.sleep(amount)
#     print(f'{amount-round((time.perf_counter()-start), 3)} is the amount of error')

# start = time.perf_counter()

# a = threading.Thread(target=timer, args=(10,))
# b = threading.Thread(target=timer, args=(5,))
# a.start()
# b.start()

# q = Queue()

# def wait():
#     time.sleep(10)
#     return 

# def process(flag:Queue=q):
#     while flag.empty():
#         print('process running')
#         time.sleep(1)
#     print('process ended')

# w = threading.Thread(target=wait)
# p = threading.Thread(target=process)

# w.start()
# p.start()


# w.join()
# q.put(True)
# p.join()
# print('last line')



# toohot = 40

# gpi0 = fn.ghardware()
# q = Queue()

# def moniter(flag:Queue=q):
#     while flag.empty():
#         temp = fn.info.cputemp()
#         if temp >= toohot:
#             print(f'{fn.info.datetime()} : High CPU temp of {temp}')
#         print('monitering active')
#         time.sleep(10)

# #start of activity
# print('Startup')

# end = threading.Thread(target=gpi0.waitButton)
# temps = threading.Thread(target=moniter)

# end.start()
# temps.start()


# #shutting down process
# end.join()
# q.put(True)
# print('Shutdown initiated by button press')
# temps.join()
# print('All threads closed - shutting down')
# #fn.interfacing.shutdown()
# print('<shutdown>')

def updates():
    ret = None
    text = fn.interfacing.aptUpdate()
    var = text[text.rfind('packages')-2]
    if var == 'l':
        ret = 0
    elif int(var) == -3:
        ret = -1
    else:
        ret = int(var)
    return ret

print(updates())