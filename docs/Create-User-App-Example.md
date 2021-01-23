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

We will be adding a new user application, the corresponding task and enabling the command line interface of the user app. We will be starting with a clean private AMDC repo that is described [here](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/Create-Private-Repo.md). The Blink app will be used as the basis for creating our custom user app. 


- Create the new define in the compiler settings
- In user_apps.c , add a section which is for your new application. Follow the format of the other sections (e.g. blink)
- Create a new folder to house your application files. Call it the name of the application (e.g. controller )
- Create the app_controller.c and app_controller.h files. Populate them with the appropriate contents (see example in blink)
- [Optional] Create a new task
- [Optional] Create a new command
