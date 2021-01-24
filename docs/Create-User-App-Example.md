# Creating a Custom User App for AMDC

Guidelines for generating custom user app code for the AMDC

## Background

This document gives an overview of generating custom user app code for use with the AMDC. 

AMDC utilizes an RTOS to coordinate system resources for the various tasks that need to be performed. It's important to understand these concepts and can be reviewed [here](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/Firmware-Arch-System.md).

In short our _application_ has some _task_ (where the work actually gets done) and we, the end user manage the task via serial _commands_. This structure requires 3 files to be constructed in a manner that the AMDC RTOS will recognize. In order to do that we will be basing our custom application off of the Blink application that is included on a clean clone of the AMDC repo, found [here](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/Create-Private-Repo.md). 

## Procedure

1. Define the application in the compiler symbols list
    1. In the _Project Explorer_ pane right click on _bare_ and select _Properties_
    2. Expand _C/C++ Build_ and select _Settings_
    3. Under _ARM v7 gcc compiler_ select _Symbols_ 
    4. Click the _+_ icon in the upper right portion of the _Defined Symbols (-D)_ pane
    5. Enter `APP_APPNAME` where `APPNAME` relates to our application purpose or function
    6. Click _OK_, then click _OK_ in the lower right of the Build Properties window 
2. Add the application in user_apps.c
    1. In the _Project Explorer_ pane locate user_apps.c under _bare -> usr -> user_apps.c_
    2. Using `APP_BLINK` as an example copy the `#ifdef APP_BLINK` section and paste it below. replacing `BLINK` with `APPNAME`. Don't worry the folders and files don't exist yet, we'll be adding them shortly. 
        ```C
        #ifdef APP_APPNAME
        #include "usr/appName/app_appName.h"
        #endif
        ```
    3. Similiarly add a section within the `user_apps_init()` function. 
        ```C
        #ifdef APP_APPNAME
        app_appName_init();
        #endif
        ```
3. Create the directory referenced in the _user_apps.c_ file
    1. In the _Project Explorer_ pane right click the _usr_ folder and select _new -> folder_
    2. name the folder **appName**
4. Create the application source and header files
    1. Using the Blink application as an example copy and paste the _app_blink.c_ and the _app_blink.h_ files into the **appName** folder
    2. Within _app_appName.c_ replace all references to `blink` with `appName`
    1. Delete the `task_vsi_init()` function call
    1. Comment out the `task_appName_init()` function call
    1. In the corresponding header file replace all references to `blink` with `appName`
5. Create the task files, where the work gets done
    1. Using the Blink application as an example copy and paste the _task_blink.c_ and the _task_blink.h_ files into the **appName** folder
    1. Starting with the header file, replace all references to `blink` with `appName`
        1. Note the define `TASK_APPNAME_UPDATES_PER_SEC (#)`  is a very useful feature that lets us define our repition rate of our task in Hz. It is not required. 
        1. The define `TASK_APPNAME_INTERNAL_USEC (#)`  is **required**. This is used by the system counters to know when to trigger our task callback function
        1. Either utilize the `*PER_SEC` define or the `*INTERNAL_USEC` define to trigger your task at the desired rate
    1. Turning to the source file, unless you are using REV_C AMDC hardware remove all sections of code that refer to `USER_CONFIG_HARDWARE_TARGET` in the top of the source file, within `task_appname_deinit()` and within `task_appname_callback()`. Also remove the `#include ...\hardware_targets.h` reference. 
    1. `task_appname_init()` is where you will place any initialization code for your task. This function will run once when we start the task. 
    1. `task_appname_callback()` will occur at the rate defined by `TASK_APPNAME_INTERNAL_USEC (#)` This is where we will place any code that needs to be run based on the task frequency. 
1. Create the command files for control via serial interface
    1. Create the folder **cmd** under the **appName** folder. See blink example file structure
    1. Copy and paste the _cmd_blink.c_ and _cmd_blink.h_ files into the newly created **cmd** folder and rename the files _cmd_appname.c and *.h_ respectively. 
    1. Replace all instances of `blink` with `appName`
    1. See the `hello` text parse sections of code for examples of multi level command parsing. 
        1. Remove `hello` parse section from `cmd_appName()` and from the `cmd_help[]` struct. 
1. Build the project. There should be no build errors, work through them if there are. 

## Summary

We now have a template file structure for adding custom user applications and task, and the framework to control these applications with the serial interface!
