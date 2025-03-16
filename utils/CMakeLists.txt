# 定义工具库的源代码和头文件
set(UTILS_SOURCES
    utils.c
    # 可添加其他工具文件，如 other.c
)

# 创建静态库（默认）或共享库
add_library(utils_library STATIC ${UTILS_SOURCES})

# 设置库的头文件目录（允许主程序找到 utils.h）
target_include_directories(utils_library PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}  # 暴露 utils 目录为头文件路径
)