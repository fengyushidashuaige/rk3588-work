#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

int main() {
    printf("=== UART Test Program ===\n");
    
    // 尝试打开UART设备
    int uart_fd = open("/dev/ttyS6", O_RDWR | O_NOCTTY);
    if (uart_fd < 0) {
        printf("❌ 无法打开 /dev/ttyS6: %s\n", strerror(errno));
        printf("可用的串口设备:\n");
        system("ls /dev/ttyS* 2>/dev/null");
        return -1;
    }
    
    printf("✅ 成功打开 /dev/ttyS6\n");
    
    // 简单的发送测试
    char message[] = "Hello UART from AIO-3588L!\n";
    ssize_t bytes_written = write(uart_fd, message, strlen(message));
    
    if (bytes_written > 0) {
        printf("✅ 发送了 %zd 字节: %s", bytes_written, message);
    } else {
        printf("❌ 发送失败: %s\n", strerror(errno));
    }
    
    close(uart_fd);
    printf("UART测试完成\n");
    return 0;
}