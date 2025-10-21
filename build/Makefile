# 编译器配置
CROSS_COMPILE = aarch64-linux-gnu-
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++

# 编译选项
CFLAGS = -Wall -O2 -static
TARGET_IP = 192.168.137.3
TARGET_USER = firefly

# 源文件和目标
SRC_DIR = ../src
BUILD_DIR = .
DEPLOY_DIR = ../deploy

# 目标程序
TARGETS = hello_arm uart_test_arm

# 默认目标：编译所有
all: $(TARGETS)

# 编译hello_arm
hello_arm: $(SRC_DIR)/hello.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ 编译完成: $@"

# 编译receiver和sender
receiver: $(SRC_DIR)/receiver.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ 编译完成: $@"


# 编译sender
sender: $(SRC_DIR)/sender.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ 编译完成: $@"


# 编译UART测试程序
uart_test_arm: $(SRC_DIR)/uart_test.c
	$(CC) $(CFLAGS) -o $@ $<
	@echo "✅ 编译完成: $@"

# 部署目标 - 依赖于hello_arm
deploy: hello_arm
	@echo "🚀 部署到目标板..."
	@cd $(DEPLOY_DIR) && ./deploy.sh
	@echo "🎉 部署完成!"

# 部署UART测试程序
deploy-uart: uart_test_arm
	@echo "🚀 部署UART测试程序..."
	scp uart_test_arm $(TARGET_USER)@$(TARGET_IP):/tmp/
	ssh $(TARGET_USER)@$(TARGET_IP) "chmod +x /tmp/uart_test_arm && /tmp/uart_test_arm"
	@echo "🎉 UART测试程序部署完成!"

# 清理编译结果
clean:
	rm -f $(TARGETS)
	@echo "🧹 清理完成"

# 显示帮助信息
help:
	@echo "可用目标:"
	@echo "  all         - 编译所有目标程序"
	@echo "  hello_arm   - 编译Hello World程序"
	@echo "  uart_test_arm - 编译UART测试程序"
	@echo "  deploy      - 编译并部署Hello World程序"
	@echo "  deploy-uart - 编译并部署UART测试程序"
	@echo "  clean       - 清理编译结果"
	@echo "  help        - 显示此帮助信息"

# 声明伪目标
.PHONY: all deploy deploy-uart clean help