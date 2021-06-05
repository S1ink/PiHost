#import matplotlib.pyplot as plt
from pythonping import ping
import gpiozero as gz
import RPi.GPIO as gpio
import pandas as pd
import numpy as np
import subprocess
import threading
import datetime
import shutil
import psutil
import time
import csv
import var
import os

class logger:
    def __init__(self, file:str=None, csv:str=None):
        self.source = file
        self.csv = csv

    @staticmethod
    def logFile(file:str, text:str='default text'):
        with open(file, 'a') as log:
            log.write(text+'\n')
    
    def log(self, text:str='default text'):
        with open(self.source, 'a') as filelog:
            filelog.write(text+'\n')

    def timedLog(self, text:str='default text'):
        with open(self.source, 'a') as timedlog:
            timedlog.write(f'{info.datetime()} : {text}\n')

    @staticmethod
    def appendcsv(csv:str, text:list=['default text']):
        append = pd.DataFrame([text])
        append.to_csv(csv, header=False, index=False, mode='a')

    def append(self, text:list=['default text']):
        append = pd.DataFrame([text])
        append.to_csv(self.csv, header=False, index=False, mode='a')

    def timedAppend(self, text:list=['default text']):
        text.insert(0, info.datetime())
        append = pd.DataFrame([text])
        append.to_csv(self.csv, header=False, index=False, mode='a')

class info:
    @staticmethod
    def datetime():
        '''Format: Dayname Monthname Day Hr:Min:Sec Year'''
        return(time.asctime(time.localtime()))

    @staticmethod
    def datetime2():
        '''Format: Year-Month-Day Hr:Min:Sec.dec'''
        return(str(datetime.datetime.now()))

    @staticmethod
    def daytime():
        return(time.strftime("%H:%M:%S", time.localtime()))
    
    @staticmethod
    def date():
        return(time.strftime('%d-%m-%Y', time.localtime()))

    @staticmethod
    def cputemp():
        res = os.popen('vcgencmd measure_temp').readline()
        temp =(res.replace("temp=","").replace("'C\n",""))
        return float(temp)

    @staticmethod
    def cpusage():
        return psutil.cpu_percent()

    @staticmethod
    def cpucores():
        return psutil.cpu_percent(percpu = True)
    
    @staticmethod
    def coreavg():
        cores = psutil.cpu_percent(percpu=True)
        return (cores[0] + cores[1] + cores[2] + cores[3])/4

    @staticmethod
    def cpufreq():
        return int(psutil.cpu_freq(percpu = False)[0])

    @staticmethod
    def aptUpdates():
        ret = None
        text = interfacing.aptUpdate()
        var = text[text.rfind('packages')-2]
        if var == 'l':
            ret = 0
        elif int(var) == -3:
            ret = -1
        else:
            ret = int(var)
        return ret

class objects:
    class timelink():
        def __init__(self, data:list=[], time:datetime.datetime=datetime.datetime.fromtimestamp(time.mktime(time.localtime()))):
            self.time = time
            self.data = data
            self.all = self.data
            self.all.insert(0, time)

        def getTime(self):
            return self.time

        def getData(self):
            return self.data

        def getAll(self):
            return self.timestamp

        def getYear(self):
            return self.time.year

        def getMonth(self):
            return self.time.month

        # def getDoy(self):
        #     return self.time.

        def getDom(self):
            return self.time.day

        def getDow(self):
            return self.time.weekday()

        def getHour(self):
            return self.time.hour

        def getMinute(self):
            return self.time.minute

        def getSecond(self):
            return self.time.second

class ghardware:
    fan_speed = None
    pc_buttontime = 0.25

    def __init__(self, fan_speed:float=0.2):
        self.fan_speed = fan_speed
        self.fan = gz.PWMOutputDevice(pin=var.gpin.fan, frequency=25000, initial_value=self.fan_speed)
        self.power = gz.Button(pin=var.gpin.shutdown)
        self.pc_power = gz.DigitalOutputDevice(pin=var.gpin.pc_power)
        self.pc_reset = gz.DigitalOutputDevice(pin=var.gpin.pc_reset)
        self.pc_status = gz.DigitalInputDevice(pin=var.gpin.pc_status, pull_up=True)

    def fanSpeed(self, speed:float=0.8):
        self.fan_speed = speed
        self.fan.value = self.fan_speed

    def waitButton(self):
        self.power.wait_for_active()
        return 1

    def pcPower(self):
        self.pc_power.on()
        time.sleep(self.pc_buttontime)
        self.pc_power.off()

    def pcReset(self):
        self.pc_reset.on()
        time.sleep(self.pc_buttontime)
        self.pc_reset.off()

    def pcHoldPower(self):
        self.pc_power.on()
        time.sleep(3)
        self.pc_power.off()

    def pcStatus(self):
        return self.pc_status.is_active

    def oldGpioSetup(self, warnings:bool=False):
        gpio.setwarnings(warnings)
        gpio.setmode(gpio.BCM)
        gpio.setup(var.gpin.fan, gpio.OUT, initial = gpio.LOW)
        gpio.setup(var.gpin.shutdown, gpio.IN, pull_up_down=gpio.PUD_UP)
        self.fan = gpio.PWM(var.gpin.fan, 25000)
        self.fan.start(self.fan_speed)

    def oldFanspeed(self, speed:float=80):
        self.fan_speed = speed
        self.fan.start(self.fan_speed)

    def oldWaitButton():
        gpio.wait_for_edge(var.gpin.shutdown, gpio.FALLING)
        return

class files:
    default_csv = var.locations.backups_csv

    class winpc:
        def dircheck(path:str=var.locations.pc_data):
            if os.path.exists(path):
                return True
            elif os.path.exists(path) == False:
                return False
            else:
                return "Error"

        def reconnect():
            connection = files.winpc.dircheck()
            if connection == True:
                return "Already connected"
            elif connection == False:
                try:
                    interfacing.rmount()
                    return "Successfully mounted"
                except:
                    return "Connection not available"
            elif connection == "Error":
                return "Relayed Error"
            else:
                return "Error"

        def disconnect():
            connection = files.winpc.dircheck()
            if connection == True:
                try:
                    interfacing.umount(var.locations.pc_mount)
                    return "Successfully disconnected"
                except:
                    return "Disconnection unsuccessful"
            elif connection == False:
                return "Already disconnected"
            elif connection == "Error":
                return "Relayed Error"
            else:
                return "Error"

    def rping(Lan='192.168.0.11', Wan='ssn.ddns.net', timo=5, verbo=False, ct=1):
        '''
        TESTS CONNECTION TO 2 SERVERS (LAN AND WAN RPI SERVERS DEFAULT)
        
        Params ->
        Lan: the server's local ip
        Wan: the server's public ip
        timo: the time before timout 
        verbo: print outputs
        ct: number of times that connection is attempted
        
        Return values-    
        0: neither server responded, most likely offline or not connected to the internet
        1: the server is available over LAN
        2: the server is available over WAN
        3: the server is available over LAN and WAN
        -1: something broke
        '''
        rlan = ping(target=Lan, timeout=timo, verbose=verbo, count=ct)
        rwan = ping(target=Wan, timeout=timo, verbose=verbo, count=ct)
        #if output contains "request timed out", the boolean true is returned then inverted, to correlate false with a connection error
        lancon = not('Request timed out' in str(rlan))
        wancon = not('Request timed out' in str(rwan))
        #return x based on y
        if lancon == False and wancon == False:
            return 0
        elif lancon == True and wancon == False:
            return 1
        elif lancon == False and wancon == True:
            return 2
        elif lancon == True and wancon == True:
            return 3
        else:
            return -1

    def sync(csv:str=default_csv):
        check = files.winpc.dircheck()
        conn = files.winpc.reconnect()
        if (check == True) or ((check == False) and conn == "Successfully mounted"):
            #run sync code here or call sync function
            files.rcsv(csv)
            return 1
        elif (check == False or "Error") and (conn == "Connection not available" or "Error"):
            return 0
        else:
            return "Error"

    def lastmod(item:str=None):
        '''
        gets last modified time of folder or file
        '''
        return time.ctime(os.path.getmtime(item))

    def lastmod_s(item:str=None):
        '''
        gets the last modified time and does not convert from seconds
        '''
        return os.path.getmtime(item)

    def rcopy(source:str, destination:str):
        cmdl = '/usr/bin/rclone sync ' + source + ' ' + destination + ' -vv'
        return subprocess.getoutput(cmdl)

    def fcsv(csv:str=default_csv):
            '''
            PARAMETER: csv- csv file with coresponding folder source and destination data of folders that will be backed up -> is passed into the function "read_csv" before executed
            '''
            start = time.perf_counter()
            csvd = files.read_csv(csv)
            data = csvd[0]
            num = csvd[1]
            print(f'{info.datetime()}: Starting sync...\n')
            count = -1
            for i in range(num):
                count = count + 1
                item = data[count]
                exsrc = files.winpc.dircheck(item.src)
                exdest = files.winpc.dircheck(item.dest)
                #check if exists
                if exsrc and exdest:
                    #compare modification times 
                    src_mod = files.lastmod_s(item.src)
                    dest_mod = files.lastmod_s(item.dest)
                    if src_mod <= dest_mod:
                        print(f'{info.datetime()}: [{item.name}] was passed\n')
                    elif src_mod > dest_mod:  
                        print(f'{info.datetime()}: Starting [{item.name}]...')
                        #test if dest already exist -> if so replace, if not, just copy
                        if os.path.exists(item.dest) == True:
                            print(f'{info.datetime()}: Removing existing files...')
                            shutil.rmtree(item.dest)
                            print(f"{info.datetime()}: Saving [{item.name}]...")
                            shutil.copytree(item.src, item.dest)
                            print(f'{info.datetime()}: Saved [{item.name}]\n')
                        else:
                            print(f'{info.datetime()}: Saving [{item.name}]...')
                            shutil.copytree(item.src, item.dest)
                            print(f'{info.datetime()}: Saved [{item.name}]\n')
                    else:
                        print(f'{info.datetime()}: Error occured while comparing mod times for [{item.name}]')
                elif not exsrc and exdest:
                    print(f'{info.datetime()}: Source directory for [{item.name}] does not exist')
                elif exsrc and not exdest:
                    print(f'{info.datetime()}: Destination directory for [{item.name}] does not exist')
                elif not exsrc and not exdest:
                    print(f'{info.datetime()}: Neither source nor destination directories for [{item.name}] exist')
                else:
                    print(f'{info.datetime()}: Error occured while testing source and destination locations for [{item.name}]')
            finish = time.perf_counter()
            print(f'{info.datetime()}: Process completed successfully in {round(finish-start, 2)} seconds')
            return info.datetime()

    def rcsv(csv:str=default_csv):
        start = time.perf_counter()
        csvd = files.read_csv(csv)
        data = csvd[0]
        num = csvd[1]
        print(f'{info.datetime()}: Starting sync...\n')
        count = -1
        for i in range(num):
            count = count + 1
            item = data[count]
            print(f'{info.datetime()}: Syncing [{item.name}]')
            print(files.rcopy(item.src, item.dest))
            print(f'{info.datetime()}: Synced [{item.dest}] with [{item.src}]\n')
        finish = time.perf_counter()
        print(f'{info.datetime()}: Process completed successfully in {round(finish-start, 3)} seconds')
        return info.datetime()

    def appendline_csv(name, src, dest, inpfile=default_csv):
        '''
        PARAMETER: inpfile- the file that will be written to 

        PARAMETER: name- the name of the game that the corresponding data will be for

        PARAMETER: src- source path where game saves are saved

        PARAMETER: dest- the destination folder where saves will be copied to 
        '''
        output = pd.DataFrame([[name, src, dest]])
        output.to_csv(inpfile, header = False, index = False, mode = 'a')

    def read_csv(file:str=default_csv):
        '''
        Reads csv file from path passed into it and returns a dictionary with keys representing rows, and their defentions storing a "gamedata" data type. Also returns the number of rows for convenience.
        '''
        path = str(file)
        #read csv with pandas
        data = pd.read_csv(path)
        #read csv with csv.reader to get number of rows of data
        readfile = open(path, 'r+')
        read = csv.reader(readfile)
        #takes into account a label column at the top of the file
        rows = len(list(read))-1
        row = -1
        #dictionary that is returned
        ret = {}
        for i in range(rows):
            row = row + 1 
            key = row
            #use pandas and the current row to access each item of data on a row and place it into a dictionary with a key corresponding to that row -> is able to return as many rows as need without returning an unknown amount of variables
            value = files.backupsync(data.iloc[row, 0], data.iloc[row, 1], data.iloc[row, 2])
            ret[key] = value
        return ret, rows

    class backupsync:
        def __init__(self, name, src, dest):
            self.name = name
            self.src = src
            self.dest = dest

class interfacing:
    def cmdwrap(command:str='ls -l'):
        return subprocess.getoutput(command)

    def rmount():
        return subprocess.run(['sudo', 'mount', '-a'])

    def umount(drive:str):
        return subprocess.run(['sudo', 'umount', '-l', drive])

    def aptUpdate():
        return subprocess.getoutput('sudo apt update')

    def shutdown():
        #return subprocess.getoutput('shutdown -h now')
        return subprocess.call(['shutdown', '-h', 'now'], shell= False)

