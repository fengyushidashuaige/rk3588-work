#ifndef UART6_INTERRUPT_SYSTEM_H
#define UART6_INTERRUPT_SYSTEM_H
#define _POSIX_C_SOURCE 199309L
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>

// UART6设备路径
#define UART6_DEVICE "/dev/ttyS6"

// 中断类型定义
typedef enum {
    IRQ_UART_DATA_READY = 1,
    IRQ_UART_ERROR,
    IRQ_UART_TIMEOUT,
    IRQ_SOFTWARE_COMMAND
} interrupt_type_t;

// 中断数据结构
typedef struct {
    interrupt_type_t type;
    int source;
    char data[256];
    struct timespec timestamp;
} interrupt_event_t;

// 中断回调函数类型
typedef void (*interrupt_handler_t)(interrupt_event_t* event);

// 中断控制器
typedef struct {
    int uart_fd;
    pthread_t irq_thread;
    volatile int running;
    interrupt_handler_t handlers[10];
    pthread_mutex_t event_mutex;
} uart6_interrupt_controller_t;

// 函数声明
int uart6_interrupt_init(uart6_interrupt_controller_t* controller);
void uart6_interrupt_register_handler(uart6_interrupt_controller_t* controller, 
                                    interrupt_type_t type, 
                                    interrupt_handler_t handler);
void uart6_interrupt_start(uart6_interrupt_controller_t* controller);
void uart6_interrupt_stop(uart6_interrupt_controller_t* controller);
int uart6_send_command(uart6_interrupt_controller_t* controller, const char* command);

#endif