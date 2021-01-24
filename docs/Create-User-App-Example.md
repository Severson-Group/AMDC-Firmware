# Creating a Custom User App for AMDC

**Guidelines for generating custom user app code for the AMDC**

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
    5. Enter "APP_**appName**" where **appName** relates to our application purpose or function
    6. Click _OK_, then click _OK_ in the lower right of the Build Properties window 
2. Add the application in user_apps.c
    1. In the _Project Explorer_ pane locate user_apps.c under _bare -> usr -> user_apps.c_
    2. Using _APP_BLINK_ as an example copy the #ifdef APP_BLINK section and paste it below. replacing **BLINK** with **appName**. Don't worry the folders and files don't exist yet, we'll be adding them shortly. 
        ```
        #ifdef APP_**APPNAME**
        #include "usr/**appName**/app_**appName**.h"
        #endif
        ```
    3. Similiarly add a section within the _user_apps_init()_ function. 
        ```
        #ifdef APP_**APPNAME**
        app_**appName**_init();
        #endif
        ```
3. Create the directory referenced in the _user_apps.c_ file
    1. In the _Project Explorer_ pane right click the _usr_ folder and select _new -> folder_
    2. name the folder **appName**
4. Create the application source and header files
    1. Using the Blink application as an example copy and paste the _app_blink.c_ and the _app_blink.h_ files into the **appName** folder
    2. Within _app_appName.c_ replace all references to _blink_ with **appName**
    1. Delete the _task_vsi_init()_ function call
    1. Comment out the _task_appName_init()_ function call
    1. In the corresponding header file replace all references to _blink_ with **appName**
5. Create the task files, where the work gets done
    1. Using the Blink application as an example copy and paste the _task_blink.c_ and the _task_blink.h_ files into the **appName** folder
    1. Starting with the header file, replace all references to _blink_ with **appName**
        1. Note the define _TASK_APPNAME_UPDATES_PER_SEC (#)_  is a very useful feature that lets us define our repition rate of our task in Hz. It is not required. 
        1. The define _TASK_APPNAME_INTERNAL_USEC (#)_  is **required**. This is used by the system counters to know when to trigger our task callback function
        1. Either utilize the _*PER_SEC_ define or the _*INTERNAL_USEC_ define to trigger your task at the desired rate
    1. Turning to the source file, unless you are using REV_C AMDC hardware remove all sections of code that refer to _USER_CONFIG_HARDWARE_TARGET_ in the top of the source file, within _task_appname_deinit()_ and within _task_appname_callback()_. Also remove the #include _*hardware_targets.h_ reference. 
    1. _task_appname_init()_ is where you will place any initialization code for your task. This function will run once when we start the task. 
    1. _task_appname_callback()_ will occur at the rate defined by _TASK_APPNAME_INTERNAL_USEC (#)_ This is where we will place any code that needs to be run based on the task frequency. 
1. Create the command files for control via serial interface
    1. Create the folder _cmd_ under the _appName_ folder. See blink example file structure
    1. Copy and paste the _cmd_blink.c_ and _cmd_blink.h_ files into the newly created _cmd_ folder and rename the files _cmd_appname.c and *.h_ respectively. 
    1. Replace all instances of _blink_ with _appName_
    1. See the _hello_ text parse sections of code for examples of multi level command parsing. 
        1. Remove _hello_ parse section within _cmd_appName()_ and within the _cmd_help[]_ struct. 
1. Build the project. There should be no build errors, work through them if there are. 

## Summary

We now have a template file structure for adding custom user applications and task, and the framework to control these applications with the serial interface!
        

- [Optional] Create a new task
- [Optional] Create a new command
