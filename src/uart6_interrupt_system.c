#include "uart6_interrupt_system.h"

// 全局中断控制器
static uart6_interrupt_controller_t g_irq_controller;

// UART6初始化
int uart6_interrupt_init(uart6_interrupt_controller_t* controller) {
    // 打开UART6设备
    controller->uart_fd = open(UART6_DEVICE, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (controller->uart_fd < 0) {
        perror("无法打开UART6设备");
        return -1;
    }

    // 配置UART6
    struct termios options;
    tcgetattr(controller->uart_fd, &options);
    
    // 设置波特率
    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);
    
    // 8N1配置
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag |= (CLOCAL | CREAD);
    
    // 原始模式
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_oflag &= ~OPOST;
    
    // 非阻塞读取，立即返回
    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 0;
    
    tcsetattr(controller->uart_fd, TCSANOW, &options);
    
    // 初始化互斥锁
    pthread_mutex_init(&controller->event_mutex, NULL);
    
    // 清空handler数组
    memset(controller->handlers, 0, sizeof(controller->handlers));
    
    controller->running = 0;
    
    printf("UART6中断控制器初始化成功\n");
    return 0;
}

// 注册中断处理函数
void uart6_interrupt_register_handler(uart6_interrupt_controller_t* controller, 
                                    interrupt_type_t type, 
                                    interrupt_handler_t handler) {
    if (type >= 0 && type < 10) {
        controller->handlers[type] = handler;
        printf("注册中断处理函数: 类型=%d\n", type);
    }
}

// 中断服务线程
void* uart6_irq_thread(void* arg) {
    uart6_interrupt_controller_t* controller = (uart6_interrupt_controller_t*)arg;
    struct pollfd fds;
    char buffer[256];
    
    fds.fd = controller->uart_fd;
    fds.events = POLLIN;
    
    printf("UART6中断服务线程启动\n");
    
    while (controller->running) {
        // 等待UART数据（模拟硬件中断等待）
        int ret = poll(&fds, 1, 100); // 100ms超时
        
        if (ret > 0 && (fds.revents & POLLIN)) {
            // 读取UART数据
            int bytes_read = read(controller->uart_fd, buffer, sizeof(buffer) - 1);
            
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                
                // 创建中断事件
                interrupt_event_t event;
                event.type = IRQ_UART_DATA_READY;
                event.source = controller->uart_fd;
                strncpy(event.data, buffer, sizeof(event.data) - 1);
                clock_gettime(CLOCK_MONOTONIC, &event.timestamp);
                
                printf("🔔 UART6数据中断: 收到 %d 字节\n", bytes_read);
                
                // 调用注册的中断处理函数
                if (controller->handlers[IRQ_UART_DATA_READY]) {
                    controller->handlers[IRQ_UART_DATA_READY](&event);
                }
            }
        } else if (ret == 0) {
            // 超时，继续等待
            continue;
        } else {
            // 错误处理
            if (controller->running) {
                perror("poll错误");
                usleep(100000); // 100ms后重试
            }
        }
    }
    
    printf("UART6中断服务线程结束\n");
    return NULL;
}

// 启动中断控制器
void uart6_interrupt_start(uart6_interrupt_controller_t* controller) {
    controller->running = 1;
    pthread_create(&controller->irq_thread, NULL, uart6_irq_thread, controller);
    printf("UART6中断控制器启动\n");
}

// 停止中断控制器
void uart6_interrupt_stop(uart6_interrupt_controller_t* controller) {
    controller->running = 0;
    pthread_join(controller->irq_thread, NULL);
    close(controller->uart_fd);
    pthread_mutex_destroy(&controller->event_mutex);
    printf("UART6中断控制器停止\n");
}

// 发送命令（模拟软件中断）
int uart6_send_command(uart6_interrupt_controller_t* controller, const char* command) {
    if (write(controller->uart_fd, command, strlen(command)) < 0) {
        perror("发送命令失败");
        return -1;
    }
    
    printf("📤 发送命令: %s\n", command);
    return 0;
}