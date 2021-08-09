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
| Argname | Internal storage type | Description |
| "fanpin" | uint | Gpio pin which a pwm fan is being controlled on - see https://abyz.me.uk/rpi/pigpio/cif.html#gpioHardwarePWM for specifications |
| "buttonpin" | uint | Gpio pin which contains a button that will be used to control shutdown |
## Additional Info
 - If you have any suggestions feel free to create an issue or pull request!
