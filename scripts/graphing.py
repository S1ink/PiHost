import functions as fn
import time

while(True):
    #fn.logger.appendcsv(f'/data/logs/{fn.info.date()}.csv', [fn.info.daytime(), fn.info.cpusage(), fn.info.cpufreq(), fn.info.cputemp()])
    print([fn.info.cpusage(), fn.info.cpufreq(), fn.info.cputemp()])
    time.sleep(2)