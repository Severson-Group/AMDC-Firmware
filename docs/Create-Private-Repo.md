# Create Private Repo

This document explains the steps needed to set-up a private repo for AMDC firmware development. The following steps outline how to create an overarching private repo which can be cloned from git and has all the needed items to flash an AMDC.

1. Perform all the steps from the [first tutorial](docs/Building-and-Running-Firmware.md), following the "private" code option.
2. Create folders in your repo root (called `$REPO_ROOT` in this doc) for all generated outputs from Vivado and SDK
    1. Create folder `$REPO_ROOT/gen/sdk_gen/`
    2. Create folder `$REPO_ROOT/gen/vivado_gen/`
3. Copy the following from `$REPO_ROOT/AMDC-Firmware/` to `$REPO_ROOT/gen/vivado_gen/`
    1. `hw/`
    2. `amdc/`
4. Copy the following from `$REPO_ROOT/AMDC-Firmware/sdk` to `$REPO_ROOT/gen/sdk_gen/`
    1. `design_1_wrapper_hw_platform_0/`
    2. `amdc_bsp/`
    3. `fsbl/`
    4. `design_1_wrapper.hdf`
5. Create a local workspace folder. This can reside anywhere you want on your local user space (outside of your private AMDC firmware repo).

Now, your private repo contains all needed files. Each time you clone it, you will need to follow the following steps to use it.

## Cloning Private Repo / First-Time Set-Up

1. Clone your repo from git
2. Make sure the submodules are checked out (`git submodule update --init` etc)
3. Open SDK
4. Select workspace as `$REPO_ROOT/gen/sdk_gen/`
5. Your projects will be imported, but you will need to fix their locations since you are on a new PC (this is how SDK works...)
6. To fix, delete all projects from SDK and reimport them (open projects from file system, etc)
7. Wait for them to compile...
8. Redo the steps from the previous tutorial, [starting here](docs/Building-and-Running-Firmware.md#fix-common-code-compilation). This links the `common` AMDC firmware code to your private project.

You can now use your private repo!
