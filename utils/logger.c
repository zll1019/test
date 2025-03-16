#include "logger.h"
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <libgen.h>

static LoggerConfig g_logger = {
    .level = LOG_LEVEL_INFO,
    .stream = NULL,
    .filename = "",
    .max_size = 0,
    .backup_count = 0,
    .mutex = PTHREAD_MUTEX_INITIALIZER
};

// 轮转日志文件
static void rotate_log() {
    if (g_logger.backup_count <= 0) return;

    // 关闭当前文件
    if (g_logger.stream && g_logger.stream != stderr) {
        fclose(g_logger.stream);
    }

    // 轮转旧日志文件
    for (int i = g_logger.backup_count - 1; i > 0; --i) {
        char old_name[512];
        char new_name[512];
        snprintf(old_name, sizeof(old_name), "%s.%d", g_logger.filename, i);
        snprintf(new_name, sizeof(new_name), "%s.%d", g_logger.filename, i + 1);
        rename(old_name, new_name);
    }

    // 重命名当前日志
    char first_backup[512];
    snprintf(first_backup, sizeof(first_backup), "%s.1", g_logger.filename);
    rename(g_logger.filename, first_backup);

    // 重新打开文件
    g_logger.stream = fopen(g_logger.filename, "a");
}

// 检查文件大小
static void check_file_size() {
    if (!g_logger.stream || g_logger.max_size == 0) return;

    struct stat st;
    if (stat(g_logger.filename, &st) == 0 && st.st_size > g_logger.max_size) {
        rotate_log();
    }
}

void log_init(const char* filename, size_t max_size, int backup_count, LogLevel level) {
    pthread_mutex_lock(&g_logger.mutex);
    
    if (filename) {
        strncpy(g_logger.filename, filename, sizeof(g_logger.filename)-1);
        g_logger.stream = fopen(filename, "a");
    } else {
        g_logger.stream = stderr;
    }
    
    g_logger.max_size = max_size;
    g_logger.backup_count = backup_count;
    g_logger.level = level;
    
    pthread_mutex_unlock(&g_logger.mutex);
}

void log_write(LogLevel level, const char* file, int line, const char* format, ...) {
    if (level < g_logger.level) return;

    // 获取时间
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[32];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    // 日志级别名称
    const char* level_names[] = {
        "DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"
    };

    pthread_mutex_lock(&g_logger.mutex);
    
    // 检查日志文件大小
    check_file_size();

    // 输出日志头
    fprintf(g_logger.stream, "[%s] %-8s [%s:%d] ", 
            timestamp, level_names[level], basename((char*)file), line);

    // 输出日志内容
    va_list args;
    va_start(args, format);
    vfprintf(g_logger.stream, format, args);
    va_end(args);

    // 确保换行
    fputc('\n', g_logger.stream);
    fflush(g_logger.stream);

    pthread_mutex_unlock(&g_logger.mutex);
}