# What is PiHost?
 _**PiHost is a (personal) project aimed at managing gpio, displaying and logging information, directing backups/syncing of files, and other basic tasks - an autonomous HOST for raspberry PI.**_
## Quick Info
 - PiHost was originally scripted in Python, but is now only being actively(kind of) developed in C++. (Python code is located within [scripts/](/scripts))
 - Additional includes come from the [Shared](/project/Shared) project. 
 - Prebuilt binaries for pihost are contained in [/Live](/Live) (SEE DISCLAIMERS). 
 - External files (or example ones) used by pihost are located in [/resources](/resources). 
## Disclaimers
 - Much of the contained code has been personalized (hard coded) to contain file paths/directories, settings, etc. specific to my machine/network. Some settings are configurable through CLI args, but if you want more customization you will have to modify and build your own version of the project.  
 - With the above in mind, it is not recommended to use the included binaries on your own hardware without a local rebuild. The programs have also only been tested (and built) on a Raspberry Pi 4, and thus the binaries are built for ARM only.  
 - Items under "resources" (especially csv's) are also very much personalized.
 - If you are have any problems but not much technical knowlege, I'm sorry but I can't really help you. 
## Functionality
 - Using a gpio button to signal a system shutdown (or simply a program exit). 
 - Controlling a pwm fan at a fixed value (Variable speed is easy to implement, just not really that necessary atm. If you would like to see it, create an issue.)
 - Warning (logging) when SoC temps rise above a set value. 
 - Creating a variable amount of tasks (threads) that can be assigned from a CSV file (like crontab). See the section below for more info. 
## CLI Arguments
 - Most settings that pihost uses can be configured through cli args. 
 - All args follow the format of _**argname=argvalue**_, and should all be separated by at least one space. 

| Argname | Internal Datatype | Default Value | Description |
|:-------:|:-----------------:|:-------------:|-------------|
| "fanpin" | uint | 18 | A gpio pin that controls (hardware) pwm for a fan, for supported pins and pin naming, see the [Pigpio library](https://abyz.me.uk/rpi/pigpio/index.html#Type_1). (make sure hardware pwm is supported) |
| "buttonpin" | uint | 3 | A gpio pin that contains a button - this is used to signal a shutdown. Pin 3 is used by default because it will also wake the pi from a powered-down state. |
| "fanspeed" | float | 40.f | Fan speed in percent. Only a resolution of 4 digits right of the decimal will be used. |
| "warningtemp" | int | 40 | (Soc) Temperature in degrees celsius above which the program will log the time, temp, and CPU utilization. |
| "pollinterval" | long | 10 | The interval in seconds at which temps will be polled and threads will check if a shutdown has been initialized. Longer intervals mean less processing time but also less responsiveness to events. |
| "output" | std::string | WKDIR/pihost.txt | A file to which pihost outputs all verbosity (WKDIR = working directory, or the folder which the program is run within) |
| "tasks" | std::string | WKDIR/taskst.csv | The path to a task file (.csv) from which task threads will be generated (see section below for more info). This arg should always be specified as the default value is specific to my machine. |
| "halt" | bool | true | Whether or not the program will call a shutdown when it exits. This should be specified as 1 or 0 compared a textual value of "true" or "false" (these would both be resolved as true). | 
 - Example: `sudo /path/to/program/pihost tasks=/path/to/tasks.csv halt=0`
## Tasks
_**Currently under construction - see source files for fuctionality**_
## Additional Info
 - If you have any suggestions feel free to create an issue or pull request!
