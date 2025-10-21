#include <stdio.h>
#include <unistd.h>
#test
int main() {
    printf("=== AIO-3588L Cross Compile Test ===\n");
    printf("Program compiled on: %s\n", __DATE__);
    printf("Running on: ");
    
    // 检测运行架构
    #ifdef __x86_64__
        printf("x86_64 PC\n");
    #elif __aarch64__
        printf("ARM64 AIO-3588L\n");
    #else
        printf("Unknown architecture\n");
    #endif
    
    printf("Process ID: %d\n", getpid());
    return 0;
}
