# PiHost 
## What is PiHost?
 _**PiHost is a (personal) project aimed at managing gpio, displaying and logging information, directing backups/syncing of files, and other basic tasks - an autonomous HOST for raspberry PI.**_
## Quick Info
 - PiHost was originally scripted in Python, but is now only being actively developed in C++. (Python code is located within the "scripts" folder)
 - All C++ source files are located under the "build" folder. All files here are copied by Visual Studio so there may be duplicates and messed up organization. (Hopefully I will fix this in the future)
 - The "Live" folder contains prebuilt binaries - see disclaimers below. 
 - "resources" contains extrenal resources for the application(s) - again, see disclaimers below. 
 - Most of the code for PiHost is inherited from a separate (shared) project called PiLib, which can be built as a library or just included from the source. These files are located under "build/Shared/Source/". 
## Disclaimers
 - Much of the contained code has been personalized and contains file paths, directories, etc. specific to my machine/network. If you want to try things out you will have to modify and build your own version of the project. In the future I hope to add general-use functionality to override these settings. 
 - With the above in mind, it is not recommended to use the included binaries on your own hardware. The programs have also only been tested (and built on) on a Raspberry Pi 4, and thus the binaries are built for ARM only. If you would like to test things please build your own version. 
 - Items under "resources" (especially csv's) are also personalized to my setup.
## CLI Args
 - Although changing the default logging location is still nonconfigurable (without a rebuild), most other settings are supported through CLI args. 
 - All args follow the format of _**argname=argvalue**_, and are separated from other args by at least one space. 

_**This chart is not complete**_
| Argname | Internal storage type | Default value | Description |
|-|-|-|
| "fanpin" | uint | 18 | Gpio pin which a pwm fan is being controlled on - see https://abyz.me.uk/rpi/pigpio/cif.html#gpioHardwarePWM for specifications |
| "buttonpin" | uint | 3 | Gpio pin for a power button, pin 3 is used by default because it will also wake the pi when it is powered off. |
| "fanspeed" | float | 40.f | Fan speed in percent. Only a resolution of 4 digits right of the decimal will be used. |
| "warningtemp" | int | 40 | (Soc) Temperature in degrees celsius above which a the program will log the time, temp, and CPU utilization. |
| "pollinterval" | long | 10 | The interval which temps will be polled and threads will check if they should exit. A longer interval may mean closing the program could take up to this amount of time to exit. |
| "tasks" | std::string | NA | The path from which task threads will be generated (currently supports an rsync task, getting APT updates, and running a command). This arg should always be specified as the default value is specific to my machine. |
| "halt" | bool | true | Weather or not the program will call call a shutdown on exit. This should be specified as 1/0 compared a textual value of "true"/"false". |
## Additional Info
 - If you have any suggestions feel free to create an issue or pull request!
