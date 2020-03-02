# Create Private Repo

This document explains the steps needed to set-up a private repo for AMDC firmware development. The following steps outline how to create an overarching private repo which can be cloned from git and has all the needed items to flash an AMDC.

1. Perform all the steps from the [first tutorial](https://github.com/Severson-Group/AMDC-Firmware/blob/develop/docs/01-Building-and-Running-Firmware.md), following the "private" code option.
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
    5. `.metadata/`
