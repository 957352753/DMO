#!/bin/bash
# =========================================================
# NVMe Device Setup Script
# Author: Li Changshou
# Description:
#   This script automates checking, partitioning, formatting,
#   and mounting an NVMe device (e.g., /dev/nvme1n1)
# =========================================================

set -e  # 出错立即退出
DEVICE="/dev/nvme1n1"
MOUNT_POINT="/media/nvme"

echo "=== Step 1: 检查 PCI 设备中是否存在 Xilinx NVMe 控制器 ==="
lspci | grep -i "Xilinx" || echo "⚠️ 未检测到 Xilinx NVMe 控制器"

echo
echo "=== Step 2: 检查 NVMe 设备是否存在 ==="
ls /dev | grep "nvme" || echo "⚠️ 未找到 NVMe 设备"

# 确认是否继续
read -p "是否继续对 ${DEVICE} 进行分区和格式化？这将清空磁盘数据！(yes/no): " confirm
if [[ "$confirm" != "yes" ]]; then
    echo "❌ 用户取消操作"
    exit 1
fi

echo
echo "=== Step 3: 创建新分区 ==="
sudo fdisk "$DEVICE" <<EOF
n
p
1


w
EOF

echo
echo "=== Step 4: 检查新分区是否创建成功 ==="
ls /dev | grep "nvme1n1p1" || {
    echo "❌ 分区创建失败"
    exit 1
}

echo
echo "=== Step 5: 格式化为 ext4 文件系统 ==="
sudo mkfs -t ext4 /dev/nvme1n1p1

echo
echo "=== Step 6: 创建挂载点并挂载 ==="
sudo mkdir -p "$MOUNT_POINT"
sudo mount /dev/nvme1n1p1 "$MOUNT_POINT"

echo
echo "=== Step 7: 检查挂载状态 ==="
lsblk | grep "nvme"

echo
echo "✅ 完成！NVMe 设备已挂载到: $MOUNT_POINT"
