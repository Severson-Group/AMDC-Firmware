# Simulink + AMDC Integration

**Guidelines for generating, simulating, and loading code for the AMDC using Simulink**

## Table of Contents

- [Purpose of Document](#purpose-of-document)
- [Software in the Loop Simulation](#sil-simulation)
- [Simulink Autogen Code](#simulink-autogen-code)
- [Integrating Autogen Code](#integrating-autogen-code)

## Purpose of Document

This document should be read by anyone looking to streamline their AMDC code development. The document gives an overview of three important tasks: generating code with Simulink, testing that code with software in the loop (SIL) simulation, and ultimately integrating that code with the AMDC. It should be viewed in parallel with the examples in [Simulink_SIL_Example](https://github.com/Severson-Group/Simulink_SIL_Example) 

## SIL Simulation
Software in the loop simulation refers to a testing methodolgy in which executable code (in our context control c-code) is tested/verified within a modelling environment. The primary benefit is it allows for testing of control code without the necessity of hardware testing, it can also take advantage of an already existing simulation model. It is useful for both simulink autogen code, as well as manually written code. 

### Structure

This sections outlines the implementation of SIL and the relevant files needed. Every SIL simulation should include four important components: a simulation model (.slx), s-function, simulation driver (.m) script, and control c-code to be verified (generated or manually written)

**S Function:** 

S-Functions are the top-level file for Simulink SIL, they make it possible to integrate c-code into a Simulink model. An S-Function simply interfaces your custom code with your Simulink model, by taking your control inputs in the model, sending those inputs into your custom code, then finally taking the code outputs and putting them back into the model. They exist in the model as blocks labelled "S-functions", where the code is connected. The block diagram below shows a basic Simulink for for current control of an RL load – the same as given in the SIL repo <LINK>. The bottom loop displays a conventional simulink model (controller is made using graphical blocks) while the top loop shows an SIL implementation. Where the S-function is implementated in place of the conventional controller block. This displays that any block you have written/generated c-code for can be included in your model with the use of an S-function block.
    <img src="images/s-function-simulink.svg" />
 A more detailed description and examples can be found here-[https://www.mathworks.com/help/simulink/sfg/what-is-an-s-function.html](https://www.mathworks.com/help/simulink/sfg/what-is-an-s-function.html). 
If writing code manually, the steps are to write an S-function in MATLAB, then place it's name within an S-function block (in the user-defined functions library from Simulink). This block takes control inputs, runs your c-code algorithms and outputs the values to the rest of the model. This process is run for each simulation time-step. Multiple S-function blocks can be used in a single simulation, though restricting to one per model is generally preferred.

To demonstrate the important sections of an S-function, the S-function from the example SIL repo <LINK> will be explained. Though it should be noted that every S-function should contain these parts. The sections below explain how you can customize an S-function to suit your code's needs (e.g., number of inputs, sample time, etc)

- **Includes**-required MATLAB defines

    `#define S_FUNCTION_LEVEL 2`

    `#define S_FUNCTION_NAME control_sil_sfunc`

    `#include "simstruc.h"`

- **Header Files**-define all your c-code header files

    `#define control.h`

- **Function definitions**

    `static void mdlInitializeSizes(SimStruct *S)`: specifies the number of input/output ports that will appear in the simulink block, as well as other characteristics of the S-function

    `static void mdlInitializeSampleTimes(SimStruct *S)`: specifies the sample time for the simulink block

    `static void mdlStart(SimStruct *S)`: function that runs once during startup. Can be used to run initialization functions in your c-code

    `static void mdlOutputs(SimStruct *S, int_T tid)`: computes the outputs 

    `static void mdlUpdate(SimStruct *S, int_T tid)`: runs at each simulation time step. Where to place your repetitive control functions from your c-code

    `static void mdlTerminate(SimStruct *S):` performs any actions needed at the simulation end

**Simulink Driver:**

MATLAB script used to initialize simulation variables. Should be similar to any other simulation script with the addition of section to compile the c-code. Before the c-code may be used for simulation, it has to be compiled as a mex-function with the following command  (a MATLAB c-compiler must be installed on your computer):

`mex -g ...`

.`/c_code/control_sil_sfunc.c ...` % S-function

`./c_code/control.c` % user C-code

All of your .c files should be included in this command. It should be noted that the use of a simulation driver is not entirely necessary, these compilation commands can be run from the command line.

**User C-code:**

This is the c-code used for your control implementation, i.e., what you are testing with this simulation. This code is compiled with the command above, initialized and then ran every sample period.

**Simulation Model:**
### Running the Simulation
