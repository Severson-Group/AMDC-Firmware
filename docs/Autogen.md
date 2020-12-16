# AMDC auto code generation using Simulink

**Guidelines for generating, simulating, and loading code for the AMDC using Simulink**

## Table of Contents

- [Purpose of Document](#purpose-of-document)
- [Simulink Autogen Code](#simulink-autogen-code)
- [Integrating Autogen Code](#integrating-autogen-code)

## Purpose of Document

This document should be read by anyone looking to streamline their AMDC code development. The document gives an overview of generating code using Simulink and integrating that code with the AMDC. It should be viewed in parallel with the examples in [Insert link here](./) 

## Simulink Autogen Code
Simulink is a popular MATLAB-based graphical programming environment. It is extensively used to model, simulate, and analyze complex dynamical systems including motor drives. The gui and block diagram environment in simulink makes it extremely user friendly, 
and easy to validate system performance and controller performance. The process of converting a user simulink model for a controller to equivalent C-code for an embedded system (such as the AMDC) is called Autogen. The autogen feature in Simulink can be used to conveniently convert complex controller implementations into C-code for implementing it on the AMDC.

### Model Configuration

This sections outlines the model configuration and settings to be made in Simulink to autogen code. 

**Pre-Requisites:** 
To autogen code using Simulink, the user should have installed the latest version of MATLAB along with the following toolboxes / features:
	- Simulink
	- Embedded coder
	- Simulink coder
In addition to the above, if the user desires to use any specialized functions / blocks, the corresponding toolboxes should be installed.

**Model Settings and Layout**
The simulink model should be split into three distinct subsystems as follows:
	1. Input / Output (I/O): This subsystem includes all Simulink I/O blocks such as signal inputs, scope, signal outputs etc. No C code should be generated for these blocks. 
	2. Plant - All the blocks corresponding to the physical system (eg: motor, R-L load), whose parameters are to be controlled is called the plant. C code is not generated for the plant model.
	3. Controller: The controller subsystem implements the logic necessary to affect the desired control action on the plant. The is the block to be implemented on the AMDC, and the C code is generated for using Simulink. 
		As the digital controllers (such as the AMDC) are all discrete time, the blocks within the controller subsystem in Simulink should all be discrete time implementations. More information about the discrete time blocks in Simulink can be found [here](https://www.mathworks.com/help/simulink/discrete.html). 	

Once the simulink model is separated into the distinct subsystems as described above, the following parameters are to be set:

	1. In `Model Settings -> Solver -> Solver selection`, the `type` should be set to `Fixed-step` 
	2. The coder to be used for autogen should be selected. By default simulink generates code for a generic real-time target. This should be changed to the embedded coder as follows:
		i. Navigate to `Model Settings -> Code Generation`.
		ii. Click on the `Browse` button under `Target selection`. A list will pop-up.
		iii. From the list select `ert.tlc`. The description field for this will read `Embedded coder`.
		iv. Click `OK` 
	
## Creating a Referenced Model
The controller subsystem for which autogen code will be generated, is to be converted to a referenced model. For simulation and code generation, blocks within a referenced model execute together as a unit. More information about referenced models can be found [here](https://www.mathworks.com/help/simulink/model-reference.html).	
The following steps must be followed to convert the controller subsystem to a referenced model:
	1. The controller subsystem should first be made an atomic subsystem. This can be done by right clicking on the controller subsystem and going to `Block Parameters (Subsystem)` and checking the `Treat as atomic unit` checkbox.
	2. The atomic subsystem can then be converted to a referenced model as follows:
		i. Right click on the controller subsystem.
		ii. Navigate to `Subsystem & Model Reference -> Convert to` and select `Referenced Model`.
		iii. Simulink will load the `Model Reference Conversion Advisor` and a pop-up will appear.
		iv. Keep the default options and click the `Convert` button.
		v. Simulink will start running a few checks and begin conversion. If there are errors, Simulink will indicate what the errors are. Please fix them and repeat steps i-iv.
		
Once the controller subsystem is successfully converted to a referenced model, it should appear as a distinct Simulink model (.slx file) in the same directory. The name of the model will be same as the controller subsystem name in the parent Simulink model.

### Auto-generated Code
After creating the referenced model for the controller subsystem, run a script that initializes the user defined variables in the model and builds the autogen code. The autogen code is built using the `slbuild(modelName.slx)` command, where `modelName.slx` is the referenced model created for the controller.
If the auto code genetation is successful, a folder named `modelName_ert_rtw` will be created in the same directory. This folder will have several files. However, only two files namely `modelName.c` and `modelName.h` will be used.

The following are to be noted about the generated files:
1. The `modelName.c` file contains the C code for the controller. Before using this with the AMDC, the user has to check if the code is implementing the expected logic, if reasonable to do.
2. The `modelName.c` file takes an input struct `modelName_U` and outputs a struct `modelName_Y`.
3. The main logic of the controller will be implemented in a function named `modelName_step` 
4. A `rate_scheduler()` function within `modelName.c` computes the subrates to run during the successive base time steps.
5. Initialization and terminate functions `modelName_initialize()` and `modelName_terminate` are also created by Simulink.  
6. The `modelName.h` file defines the external I/O, timing information for the model, entry points etc.
7. Both `modelName.h` and `modelName.c` will be generated with comments that are very userfriendly.

Once the autogen code is generated, it needs to be integrated with the AMDC. This will be explained in the following section.

## Integrating Autogen Code

