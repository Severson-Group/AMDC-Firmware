# Firmware Architecture - System

[Firmware Architecture](00-Firmware-Architecture.md)
- [Drivers](00a-Firmware-Arch-Drivers.md)
- **System**
- [User Applications](00c-Firmware-Arch-UserApps.md)

## Background

The AMDC firmware is designed to include a "system" layer which functions as a **simple** real-time operating system (RTOS). Read about RTOS design principles [on Wikipedia](https://en.wikipedia.org/wiki/Real-time_operating_system) before continuing...

What makes AMDC's RTOS design **simple**? Why is it used instead of a "real" RTOS (e.g. [FreeRTOS](https://www.freertos.org/))? What features does it expose to the user application? This document aims at answering these questions and more.

## System Design

The AMDC *system* layer sits between the *driver* layer and the *user application* layer. Some system resources (such as the serial interface) are managed exclusively by the system layer -- for example, if a user application needs to print a message to the console for debugging, it must request the system to do this action on its behalf (via the `sys/debug.c` module). Other system resources, such as PWM outputs or analog inputs, can be accessed directly by the user application via the driver layer.

### Tasks

Tasks are the foundation upon which all system services are built. As described in previous documentation, a task is simply a block of code that runs periodically. Tasks can exist in the user space (used in user applications) or in the system space.

Example tasks could be:
- `task1` runs at 1Hz and blinks an LED
- `task2` runs at 10kHz and regulates current through an RL load
- `task3` runs at 1kHz and handles serial communciation with UART interface

Each example task includes a frequency of operation as well as a "high-level" goal. Usually, the task has *state* that is updated each time the task is run. In `task1`, the state might be the current LED status (on/off). Each time `task1` is run, the LED state toggles and the LED is refreshed.

Notice how each task is independent -- they all run at different frequencies and do different things -- but together, they perform complex actions as a complete system. This is the crux of designing firmware to use a RTOS: splitting code into tasks which work together to solve a complex goal. You will need to do this when building user applications with AMDC.

#### Cooperation Between Tasks

The system scheduler (`sys/scheduler.c`) is responsible for running the registered tasks of the system. Tasks are **non-preemptable** by design. This means that once a task starts, it runs until it *yields* the processor -- it cannot be interrupted by the system. Imagine a task which has a lot of work to do. It gets scheduled to run by the scheduler and starts execution. **It then has complete control of the entire system.** It can choose to run as long as it wants. Only when it stops doing work and *yields* the processor does the scheduler regain control. At this point, the scheduler chooses another task to run and the cycle repeats.

Why is it important to understand that tasks are **non-preemptable**? Well, because of this fact, the scheduler is therefore designed for **cooperative** tasks (one would say that AMDC uses "cooperative scheduling"). The scheduler *relies* on the assumption that each task can be trusted -- no tasks are malicous and will take over the system by not yielding the processor. It is up to the developer to ensure this is true, otherwise the system will not operate correctly.

In practice, how does one create a cooperative task? This boils down to keeping tasks short. Only do a small amount of work, then stop and yield the processor. The maximum frequency of tasks (typically 10kHz) determines the total amount of time available for tasks during a time slice. At 10kHz scheduler frequency, the elementary time quantum is 100us. This means at all tasks registered with the scheduler must complete within 100us. Note that 100us is the *combined* time -- if there are 10 tasks that need to run at 10kHz, the total sum of the run time for each task must be less than 100us otherwise the scheduler time quantum will be overrrun. This is bad and should be avoided. 

What if a task must perform complex actions that take too long? If a task must do a lot of work, the developer must break up the work into small chunks, then perform each chunk of work during its own time slice. Breaking up work into small chunks is an advanced topic and will be covered in later documentation.

Read more about cooperative scheduling of tasks [on Wikipedia](https://en.wikipedia.org/wiki/Cooperative_multitasking).

### Commands

TODO: write this.
