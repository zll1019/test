#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <pthread.h>

// 日志级别枚举
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_CRITICAL
} LogLevel;

// 日志配置结构体
typedef struct {
    LogLevel level;
    FILE* stream;
    char filename[256];
    size_t max_size;
    int backup_count;
    pthread_mutex_t mutex;
} LoggerConfig;

// 初始化日志系统
void log_init(const char* filename, size_t max_size, int backup_count, LogLevel level);

// 核心日志函数
void log_write(LogLevel level, const char* file, int line, const char* format, ...);

// 日志宏（自动捕获文件名和行号）
#define LOG_DEBUG(...)   log_write(LOG_LEVEL_DEBUG,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    log_write(LOG_LEVEL_INFO,    __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) log_write(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   log_write(LOG_LEVEL_ERROR,   __FILE__, __LINE__, __VA_ARGS__)
#define LOG_CRITICAL(...) log_write(LOG_LEVEL_CRITICAL,__FILE__, __LINE__, __VA_ARGS__)

#endif