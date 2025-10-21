#!/bin/bash

# 配置参数
TARGET_IP="192.168.137.3"
TARGET_USER="firefly"
LOCAL_FILE="hello_arm"
REMOTE_PATH="/tmp"
BUILD_DIR="../build"

echo "=== Enhanced Deployment Script ==="

# 检查文件是否存在
if [ ! -f "$BUILD_DIR/$LOCAL_FILE" ]; then
    echo "❌ Error: $BUILD_DIR/$LOCAL_FILE not found!"
    echo "Available files in $BUILD_DIR:"
    ls -la "$BUILD_DIR" || echo "Build directory doesn't exist"
    exit 1
fi

echo "✅ Found: $BUILD_DIR/$LOCAL_FILE"

# 测试网络连接
echo "Testing connection to $TARGET_IP..."
ping -c 1 -W 2 $TARGET_IP > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "❌ Network unreachable: $TARGET_IP"
    exit 1
fi
echo "✅ Network connection OK"

# 传输文件
echo "Transferring $LOCAL_FILE to $TARGET_USER@$TARGET_IP:$REMOTE_PATH/"
scp "$BUILD_DIR/$LOCAL_FILE" $TARGET_USER@$TARGET_IP:$REMOTE_PATH/
if [ $? -ne 0 ]; then
    echo "❌ File transfer failed"
    exit 1
fi
echo "✅ File transfer successful"

# 设置执行权限并运行
echo "Executing on target..."
ssh $TARGET_USER@$TARGET_IP "chmod +x $REMOTE_PATH/$LOCAL_FILE && echo '=== Program Output ===' && $REMOTE_PATH/$LOCAL_FILE"

if [ $? -eq 0 ]; then
    echo "🎉 Deployment completed successfully!"
else
    echo "❌ Execution failed on target"
    exit 1
fi