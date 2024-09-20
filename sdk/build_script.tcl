# Users can run this script by running the following command in the Xilinx Software Command-line Tools Console (Xilinx > XSCT Console):
#   source [getws]/config_sdk.tcl
# Change execution location to AMDC-Firmware/sdk directory
cd [getws]
puts "Configuring SDK projects in [getws]"
# Create new BSP for cpu0, add libraries
puts "Creating amdc_bsp_cpu0..."
set project_to_create amdc_bsp_cpu0
if {[file exist $project_to_create]} {
    # check that it's a directory
    if {! [file isdirectory $project_to_create]} {
        puts "amdc_bsp_cpu0 exists, but it's a file... skipping without modification."
    }
	puts "amdc_bsp_cpu0 already exists... skipping without modification."
} else {
	createbsp -name amdc_bsp_cpu0 -hwproject amdc_revf_wrapper_hw_platform_0 -proc  ps7_cortexa9_0 -os standalone
	projects -build -type bsp -name amdc_bsp_cpu0
	setlib -bsp amdc_bsp_cpu0 -lib lwip211
	setlib -bsp amdc_bsp_cpu0 -lib xilffs
	regenbsp -bsp amdc_bsp_cpu0
	projects -build -type bsp -name amdc_bsp_cpu0
	puts "amdc_bsp_cpu0 created!"
}
# Create new BSP for cpu1, add libraries, add extra compiler flag
puts "Creating amdc_bsp_cpu1..."
set project_to_create amdc_bsp_cpu1
if {[file exist $project_to_create]} {
    # check that it's a directory
    if {! [file isdirectory $project_to_create]} {
        puts "amdc_bsp_cpu1 exists, but it's a file... skipping without modification."
    }
	puts "amdc_bsp_cpu1 already exists... skipping without modification."
} else {
	createbsp -name amdc_bsp_cpu1 -hwproject amdc_revf_wrapper_hw_platform_0 -proc  ps7_cortexa9_1 -os standalone
	projects -build -type bsp -name amdc_bsp_cpu1
	setlib -bsp amdc_bsp_cpu1 -lib lwip211
	setlib -bsp amdc_bsp_cpu1 -lib xilffs
	configbsp -bsp amdc_bsp_cpu1 -append extra_compiler_flags "-DUSE_AMP=1"
	regenbsp -bsp amdc_bsp_cpu1
	projects -build -type bsp -name amdc_bsp_cpu1
	puts "amdc_bsp_cpu1 created!"
}
# Import SDK projects
puts "Importing app_cpu0 and app_cpu1 projects..."
importprojects [getws]
puts "Projects imported!"