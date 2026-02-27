#!/bin/bash

# ---------------- 配置区域 ----------------
PGDATA="/media/czt/learn/data"        # 你的 PGDATA 目录
PGCTL="/home/cjj/module/pgsql/bin/pg_ctl"   # 你的 pg_ctl 路径
PSQL="/home/cjj/module/pgsql/bin/psql"      # 你的 psql 路径
SUDO_PASS="114514"                          # sudo 密码（注意安全）
TPCH_SQL_DIR="/home/cjj/TPCH/tpch-sql-plan-100G/tpch-sql-100G"
# TPCH_SQL_DIR="/home/cjj/TPCH/tpch-sql-100G"      # SQL 文件目录
# ------------------------------------------

# 检查参数
if [ -z "$1" ]; then
    echo "用法: $0 <sql_file>"
    echo "示例: $0 q1.sql"
    exit 1
fi

SQL_FILE="$TPCH_SQL_DIR/$1"

if [ ! -f "$SQL_FILE" ]; then
    echo "错误: 找不到 SQL 文件: $SQL_FILE"
    exit 1
fi

echo "[1] sync 写回所有脏页..."
echo $SUDO_PASS | sudo -S sync

echo "[2] 清空 Linux PageCache / dentries / inodes..."
echo $SUDO_PASS | sudo -S sh -c 'echo 3 > /proc/sys/vm/drop_caches'

echo "[3] 重启 PostgreSQL（清空 PG buffer cache）..."
$PGCTL restart -D "$PGDATA"
sar -P ALL 1 3600 > cpu_usage_per_core.txt &
echo "[4] 执行 SQL 并开启 timing..."
$PSQL -d postgres -c "\timing on" -f "$SQL_FILE"
kill $!

