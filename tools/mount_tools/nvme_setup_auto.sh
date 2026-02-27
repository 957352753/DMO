#!/usr/bin/expect -f

set timeout -1
set password "114514"
set target_sn "15091712"

# === 获取 NVMe 设备节点 ===
set cmd "nvme list | awk -v sn=$target_sn '\$0 ~ sn {print \$1}'"
set device [exec bash -c "$cmd"]

puts "Detected NVMe device: $device"

# === 判断执行哪个脚本 ===
if {$device == "/dev/nvme0n1"} {
    set setup_script "./setup_csd_nvme0n1.sh"
} else {
    set setup_script "./setup_csd_nvme1n1.sh"
}

puts "Executing: $setup_script"

# === 开始执行脚本 ===
spawn bash $setup_script

expect "是否继续对"
send "yes\r"

expect {
    "密码" {
        send "$password\r"
        exp_continue
    }
    eof
}
