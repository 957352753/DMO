set ip_dir [file normalize [file dirname [info script]]]
set src_dir [file join $ip_dir src]
set rtl_file [file join $src_dir filter_accel.v]

create_project -in_memory -part xczu17eg-ffvc1760-2-e
add_files -norecurse $rtl_file
set_property top filter_accel [current_fileset]
update_compile_order -fileset sources_1

ipx::package_project -root_dir $ip_dir -vendor enclab -library user -taxonomy /UserIP -import_files -set_current true

set core [ipx::current_core]
set_property name filter_accel $core
set_property display_name {Filter Accelerator} $core
set_property description {Minimal AXI4 memory-mapped filter accelerator for page predicate scanning} $core
set_property version 1.0 $core
set_property vendor enclab $core
set_property library user $core
set_property core_revision 2 $core

ipx::infer_bus_interfaces xilinx.com:interface:aximm_rtl:1.0 $core
ipx::infer_bus_interfaces xilinx.com:signal:clock_rtl:1.0 $core
ipx::infer_bus_interfaces xilinx.com:signal:reset_rtl:1.0 $core

set s_axi [ipx::get_bus_interfaces s_axi -of_objects $core]
if {$s_axi ne ""} {
    set_property interface_mode slave $s_axi
    set_property abstraction_type_vlnv xilinx.com:interface:aximm_rtl:1.0 $s_axi
    set_property bus_type_vlnv xilinx.com:interface:aximm:1.0 $s_axi
}

set m_axi [ipx::get_bus_interfaces m_axi -of_objects $core]
if {$m_axi ne ""} {
    set_property interface_mode master $m_axi
    set_property abstraction_type_vlnv xilinx.com:interface:aximm_rtl:1.0 $m_axi
    set_property bus_type_vlnv xilinx.com:interface:aximm:1.0 $m_axi
}

set clk_if [ipx::get_bus_interfaces aclk -of_objects $core]
if {$clk_if ne ""} {
    set_property interface_mode slave $clk_if
    set clk_busif_param [ipx::get_bus_parameters ASSOCIATED_BUSIF -of_objects $clk_if]
    if {$clk_busif_param ne ""} {
        set_property value {m_axi:s_axi} $clk_busif_param
    }
    set clk_reset_param [ipx::get_bus_parameters ASSOCIATED_RESET -of_objects $clk_if]
    if {$clk_reset_param ne ""} {
        set_property value {aresetn} $clk_reset_param
    }
}

set rst_if [ipx::get_bus_interfaces aresetn -of_objects $core]
if {$rst_if ne ""} {
    set_property interface_mode slave $rst_if
}

ipx::update_checksums $core
ipx::save_core $core
close_project
