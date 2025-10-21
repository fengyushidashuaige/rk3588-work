#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

int uart_init(const char* device, int baudrate) {
    int uart_fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (uart_fd == -1) {
        perror("打开串口失败");
        return -1;
    }
    
    struct termios options;
    tcgetattr(uart_fd, &options);
    
    // 设置波特率
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    
    // 8N1配置
    options.c_cflag &= ~PARENB;   // 无校验
    options.c_cflag &= ~CSTOPB;   // 1位停止位
    options.c_cflag &= ~CSIZE;    // 清除数据位
    options.c_cflag |= CS8;       // 8位数据
    
    // 启用接收，忽略调制解调器状态
    options.c_cflag |= (CLOCAL | CREAD);
    
    // 原始模式输入
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    
    // 原始模式输出
    options.c_oflag &= ~OPOST;
    
    tcsetattr(uart_fd, TCSANOW, &options);
    return uart_fd;
}

int main() {
    int uart_fd = uart_init("/dev/ttyS6", B115200);
    if (uart_fd < 0) {
        return -1;
    }
    
    printf("UART发送端已启动，输入消息发送（输入'quit'退出）:\n");
    
    char buffer[256];
    while (1) {
        printf("发送: ");
        fgets(buffer, sizeof(buffer), stdin);
        
        // 移除换行符
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        // 发送数据
        int bytes_written = write(uart_fd, buffer, strlen(buffer));
        if (bytes_written < 0) {
            perror("发送失败");
        } else {
            printf("已发送 %d 字节: %s\n", bytes_written, buffer);
        }
        
        usleep(100000); // 等待100ms
    }
    
    close(uart_fd);
    return 0;
}