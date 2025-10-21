#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
int uart_init(const char* device, int baudrate) {
    // 与发送端相同的初始化代码
    int uart_fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (uart_fd == -1) {
        perror("打开串口失败");
        return -1;
    }
    
    struct termios options;
    tcgetattr(uart_fd, &options);
    
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    
    tcsetattr(uart_fd, TCSANOW, &options);
    return uart_fd;
}

int main() {
    int uart_fd = uart_init("/dev/ttyS6", B115200);
    if (uart_fd < 0) {
        return -1;
    }
    
    printf("UART接收端已启动，等待数据...\n");
    
    char buffer[256];
    while (1) {
        // 清空缓冲区
        memset(buffer, 0, sizeof(buffer));
        
        // 读取数据
        int bytes_read = read(uart_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // 确保字符串结束
            printf("接收到 %d 字节: %s\n", bytes_read, buffer);
        } else if (bytes_read < 0) {
            // 非阻塞模式下，没有数据是正常的
            if (errno!= EAGAIN && errno != EWOULDBLOCK) {
                perror("读取错误");
            }
        }
        
        usleep(100000); // 等待100ms
    }
    
    close(uart_fd);
    return 0;
}