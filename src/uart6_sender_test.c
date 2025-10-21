#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define UART6_DEVICE "/dev/ttyS6"

int uart6_init() {
    int fd = open(UART6_DEVICE, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("打开UART6失败");
        return -1;
    }
    
    struct termios options;
    tcgetattr(fd, &options);
    
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    
    tcsetattr(fd, TCSANOW, &options);
    return fd;
}

int main() {
    printf("=== UART6中断触发测试（发送端）===\n");
    
    int uart_fd = uart6_init();
    if (uart_fd < 0) {
        return -1;
    }
    
    printf("输入要发送的消息（触发对端中断）:\n");
    printf("输入'quit'退出\n\n");
    
    char buffer[256];
    int message_count = 0;
    
    while (1) {
        printf("发送消息 %d: ", ++message_count);
        
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }
        
        // 移除换行符
        buffer[strcspn(buffer, "\n")] = 0;
        
        if (strcmp(buffer, "quit") == 0) {
            break;
        }
        
        // 添加换行符以便接收端容易解析
        strcat(buffer, "\n");
        
        // 发送数据（触发对端中断）
        int bytes_sent = write(uart_fd, buffer, strlen(buffer));
        if (bytes_sent > 0) {
            printf("✅ 已发送 %d 字节: %s\n", bytes_sent, buffer);
        } else {
            perror("发送失败");
        }
        
        // 短暂延迟
        usleep(50000);
    }
    
    close(uart_fd);
    printf("发送端程序退出\n");
    return 0;
}