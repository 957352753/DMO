# DMO CSD Operator Offload

This repository contains the FPGA project and the `touying` CSD operator
offload implementation. The data path supports filter scan, projection,
aggregate (`SUM`, `MIN`, and `MAX`), bounded Top-K sort, and phased Hash Join.
Projection and aggregate execution can use the CSD ARM cores or FPGA; Hash Join
uses persistent ARM hash-table state with an FPGA-assisted probe path.

## Repository layout

- `cosm-plus-sys.xpr`, `cosm-plus-sys.srcs/`, `cosm-plus-sys.ipdefs/`: Vivado
  project sources and local IP required by the FPGA design.
- `operators/host/`: PostgreSQL comparison client, CSD operator client, result
  decoding, and canonical correctness checks.
- `operators/ws2/ftl/run-gr3ftl/src/`: CSD ARM firmware and NVMe operator ABI.
- `operators/rtl/`: filter/projection/compute FPGA RTL and simulation testbench.
