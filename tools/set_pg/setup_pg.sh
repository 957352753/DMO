#!/usr/bin/env bash
set -e

# -------------------------------
# 用户要求的配置
# -------------------------------
PGUSER=czt
PGPASSWORD=114514
PGDATA=/media/czt/learn/data
PGSQL_BIN=/home/cjj/module/pgsql/bin
#TBL_DIR=/home/cjj/TPCH/TPC-H-3.0.1/dbgen/tbl/
#TBL_DIR=/home/cjj/TPCH/tpch-1G/
#TBL_DIR=/home/cjj/TPCH/tpch-10M/
#TBL_DIR=/home/cjj/TPCH/tpch-500M/
TBL_DIR=/home/cjj/TPCH/tpch-100G/

SQL_FILE=/home/cjj/TPCH/TPC-H-3.0.1/dbgen/tpch.sql
#SQL_FILE=/home/cjj/TPCH/StudentSQL/student.sql
echo "[INFO] 创建数据目录（需要 sudo）"
sudo mkdir -p $PGDATA
sudo chown -R $PGUSER:$PGUSER $PGDATA
sudo chmod 700 $PGDATA

# 进入 PG bin 目录（你原来的操作方式）
cd $PGSQL_BIN

# 若 PGDATA 未初始化则 initdb
if [ ! -f "$PGDATA/PG_VERSION" ]; then
    echo "[INFO] initdb 初始化..."
    ./initdb -D $PGDATA
else
    echo "[INFO] PGDATA 已初始化，跳过 initdb"
fi

# 🟢 **使用你要求的命令启动数据库**
echo "[INFO] 启动 PostgreSQL..."
./pg_ctl -D $PGDATA start

# 等待数据库启动
echo "[INFO] 等待数据库就绪..."
sleep 3

# 使用 psql 执行 TPCH SQL（使用用户指定密码）
export PGPASSWORD=$PGPASSWORD

echo "[INFO] 执行 psql 加载 TPC-H 数据..."
./psql -v tbl_dir=$TBL_DIR -U $PGUSER -d postgres -f $SQL_FILE
#./psql -U $PGUSER -d postgres -f $SQL_FILE
echo "[INFO] 完成！"
