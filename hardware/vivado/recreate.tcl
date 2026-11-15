# New convenience script around the recovered Vivado 2023.1 BD; not an original export.
# Run: vivado -mode batch -source hardware/vivado/recreate.tcl
set source_dir [file dirname [file normalize [info script]]]
set repo_dir [file dirname [file dirname $source_dir]]
create_project axi_multiplier [file join $repo_dir build vivado] -part xc7z010clg400-1
set_property ip_repo_paths [list [file join $repo_dir hardware ip]] [current_project]
update_ip_catalog
add_files -norecurse [file join $source_dir lab4_block_design.bd]
open_bd_design [file join $source_dir lab4_block_design.bd]
validate_bd_design
generate_target all [get_files lab4_block_design.bd]
set wrapper [make_wrapper -files [get_files lab4_block_design.bd] -top]
add_files -norecurse $wrapper
set_property top lab4_block_design_wrapper [current_fileset]
update_compile_order -fileset sources_1
# Synthesis, implementation, bitstream generation and XSA export remain explicit steps.
