# Creating a Custom User App for AMDC

**Guidelines for generating custom user app code for the AMDC**

## Table of Contents

- [Purpose of Document](#purpose-of-document)
- [Example](#example)
- [Integration with AMDC](#integration-with-amdc)

## Purpose of Document

This document gives an overview of generating custom user app code for use with the AMDC. 

## Background

AMDC utilizes an RTOS to coordinate system resources for the various tasks that need to be performed. It's important to understand these concepts and can be reviewed [here](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/Firmware-Arch-System.md).

In short our _application_ has some _task_ (where the work actually gets done) and we, the end user manage the task via serial _commands_. This structure requires 3 files to be constructed in a manner that the AMDC RTOS will recognize. In order to do that we will be basing our custom application off of the Blink application that is included on a clean clone of the AMDC repo, found [here](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/Create-Private-Repo.md). 

## Procedure

1. Define our application in the compiler symbols list
    1. In the _Project Explorer_ pane right click on _bare_ and select _Properties_
    2. Expand _C/C++ Build_ and select _Settings_
    3. Under _ARM v7 gcc compiler_ select _Symbols_ 
    4. Click the _+_ icon in the upper right portion of the _Defined Symbols (-D)_ pane
    5. Enter "APP_**appName**" where **appName** relates to our application purpose or function
    6. Click _OK_, then click _OK_ in the lower right of the Build Properties window 
    
2. Add our application in user_apps.c
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

3. Lets create the folder we just pointed our _user_apps.c_ file to
    1. In the _Project Explorer_ pane right click the _usr_ folder and select _new -> folder_
    2. name the folder **appName**

4. Create our application source and header files
    1. Using the Blink application as an example copy and paste the _app_blink.c_ and the _app_blink.h_ files into our **appName** folder
    2. Within _app_appName.c_ replace all references to _blink_ with **appName**
    1. Delete the _task_vsi_init()_ function call
    1. Comment out the _task_appName_init()_ function call
    1. In the corresponding header file replace all references to _blink_ with **appName**
    
5. Create our task files, where the work gets done
    1. Using the Blink application as an example copy and paste the _task_blink.c_ and the _task_blink.h_ files into our **appName** folder
    1. Starting with the header file, replace all references to _blink_ with **appName**

- [Optional] Create a new task
- [Optional] Create a new command
