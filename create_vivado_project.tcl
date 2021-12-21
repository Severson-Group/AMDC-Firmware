# ------------------------------
# AMDC Vivado Project Generation
# ------------------------------
#
# Created by Nathan Petersen
# February 26, 2019
# 
# ----------------------------------------
#
# Use this tcl script to re-create the AMDC Vivado
# project after checking out from source control.
#
# In order to run this tcl script, please source
# this file in the Vivado Tcl Shell:
#
# 1) Open Vivado Application
# 2) Tools > Run Tcl Script...
# 3) Select this script and run
#
# ----------------------------------------
#
# Script tested with Vivado 2019.1 (64-bit)
#
# Vivado automatically created the base version
# of this script. Nathan then modified it to fit
# the needs of the AMDC project. To recreate the
# base script, use File > Write Project Tcl...
#
# ----------------------------------------

set proj_name      amdc
set proj_ip_path   "./ip_repo"

# This variable (proj_revision) should be set from calling script...
#set proj_revision reve
if {![info exists proj_revision]} {
  return -code error "Error: No project revision specified. Please run the appropriate import_rev*.tcl scripts."
}

# Find location of the running tcl script
# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir [file dirname [file normalize [info script ]]]

# Set the directory path for the original project from where this script was exported
set orig_proj_dir "[file normalize "$origin_dir/$proj_name"]"

# Create project
create_project $proj_name $orig_proj_dir -part xc7z030sbg485-1

# Set the directory path for the new project
set proj_dir [get_property directory [current_project]]

set design_name $proj_name

# Reconstruct message rules
# None

# Set project properties
set obj [get_projects $proj_name]
set_property -name "board_part" -value "em.avnet.com:picozed_7030_fmc2:part0:1.1" -objects $obj
set_property -name "default_lib" -value "xil_defaultlib" -objects $obj
set_property -name "ip_cache_permissions" -value "read write" -objects $obj
set_property -name "ip_output_repo" -value "$proj_dir/${proj_name}.cache/ip" -objects $obj
set_property -name "sim.ip.auto_export_scripts" -value "1" -objects $obj
set_property -name "simulator_language" -value "Mixed" -objects $obj
set_property -name "xpm_libraries" -value "XPM_CDC" -objects $obj

# Create 'sources_1' fileset (if not found)
if {[string equal [get_filesets -quiet sources_1] ""]} {
  create_fileset -srcset sources_1
}

# Set IP repository paths
set obj [get_filesets sources_1]
set_property "ip_repo_paths" "[file normalize "$origin_dir/$proj_ip_path"]" $obj

# Rebuild user ip_repo's index before adding any source files
update_ip_catalog -rebuild

# Set 'sources_1' fileset object
set obj [get_filesets sources_1]
set files [list \
 "[file normalize "$origin_dir/hw/${proj_name}_${proj_revision}.bd"]"\
]
add_files -norecurse -fileset $obj $files


# Create Verilog wrapper file for block diagram
make_wrapper -files [get_files $origin_dir/hw/${proj_name}_${proj_revision}.bd] -top

# Add newly created wrapper to project
add_files -norecurse $origin_dir/hw/hdl/${proj_name}_${proj_revision}_wrapper.v
update_compile_order -fileset sources_1


# Set 'sources_1' fileset properties
set obj [get_filesets sources_1]
set_property -name "top" -value "${proj_name}_${proj_revision}_wrapper" -objects $obj

# Create 'constrs_1' fileset (if not found)
if {[string equal [get_filesets -quiet constrs_1] ""]} {
  create_fileset -constrset constrs_1
}

# Set 'constrs_1' fileset object
set obj [get_filesets constrs_1]

# Add/Import constrs file and set constrs file properties
set file "[file normalize "$origin_dir/hw/constraints_amdc_${proj_revision}.xdc"]"
set file_added [add_files -norecurse -fileset $obj $file]
set file "$origin_dir/hw/constraints_amdc_${proj_revision}.xdc"
set file [file normalize $file]
set file_obj [get_files -of_objects [get_filesets constrs_1] [list "*$file"]]
set_property -name "file_type" -value "XDC" -objects $file_obj

# Set 'constrs_1' fileset properties
set obj [get_filesets constrs_1]

# Create 'sim_1' fileset (if not found)
if {[string equal [get_filesets -quiet sim_1] ""]} {
  create_fileset -simset sim_1
}

# Set 'sim_1' fileset object
set obj [get_filesets sim_1]
# Empty (no sources present)

# Set 'sim_1' fileset properties
set obj [get_filesets sim_1]
set_property -name "top" -value "${proj_name}_${proj_revision}_wrapper" -objects $obj

# Create 'synth_1' run (if not found)
if {[string equal [get_runs -quiet synth_1] ""]} {
  create_run -name synth_1 -part xc7z030sbg485-1 -flow {Vivado Synthesis 2017} -strategy "Vivado Synthesis Defaults" -constrset constrs_1
} else {
  set_property strategy "Vivado Synthesis Defaults" [get_runs synth_1]
  set_property flow "Vivado Synthesis 2017" [get_runs synth_1]
}
set obj [get_runs synth_1]

# set the current synth run
current_run -synthesis [get_runs synth_1]

# Create 'impl_1' run (if not found)
if {[string equal [get_runs -quiet impl_1] ""]} {
  create_run -name impl_1 -part xc7z030sbg485-1 -flow {Vivado Implementation 2017} -strategy "Vivado Implementation Defaults" -constrset constrs_1 -parent_run synth_1
} else {
  set_property strategy "Vivado Implementation Defaults" [get_runs impl_1]
  set_property flow "Vivado Implementation 2017" [get_runs impl_1]
}
set obj [get_runs impl_1]
set_property -name "steps.write_bitstream.args.readback_file" -value "0" -objects $obj
set_property -name "steps.write_bitstream.args.verbose" -value "0" -objects $obj

# set the current impl run
current_run -implementation [get_runs impl_1]

# Open the block diagram
open_bd_design $origin_dir/hw/${proj_name}_${proj_revision}.bd

puts "INFO: Project created: $proj_name"
