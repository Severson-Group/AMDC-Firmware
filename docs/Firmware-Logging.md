# Logging with the AMDC
The ability to log and extract variables of interest out of the AMDC is a critical feature needed for debugging, testing, and general data recording. Because of this, the AMDC has logging capabilities built into the firmware. The intent of this document is to show a user how to implement logging in their code. 

There are two interfaces that can be used for logging: 1) the standard serial terminal interface that is typically used with user commands (logging can be thought of as an application that can be included in your project) and 2) a Python interface that is built on top of and wraps the serial interface. It is highly recommended that users use the Python interface as it provides certain convenience methods and abstractions that make logging much more intuitive and less error prone.

## General Flow

The general flow for logging data in the AMDC is simple and uses the following procedure:

1. Register variable(s) to log
1. Start logging
1. Stop logging
1. Dump all of the collected data

## C-Code Modifications

The firmware has been design specifically to limit the amount of changes users have to make to their C-code to log variables of interest. 

## Terminal Interface

## Python Interface
