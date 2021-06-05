from queue import Queue
import functions as fn
#import schedule as sch
import threading
import time
import var

toohot = 40
monitering = 10

gpi0 = fn.ghardware()
scribe = fn.logger(file=var.locations.script_log, csv=var.locations.script_csv)
q = Queue()

def moniter(flag:Queue=q):
    while flag.empty():
        temp = fn.info.cputemp()
        if temp >= toohot:
            scribe.timedLog(f'High CPU temp of {temp}')
        time.sleep(monitering)

#start of activity
scribe.timedLog('Startup')

end = threading.Thread(target=gpi0.waitButton)
temps = threading.Thread(target=moniter)

end.start()
temps.start()


#shutting down process
end.join()
scribe.timedLog('Shutdown initiated by button press')
q.put(True)
temps.join()
scribe.timedLog('All threads closed : shutting down\n')
fn.interfacing.shutdown()