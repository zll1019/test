# 设置CMake最低版本要求
cmake_minimum_required(VERSION 3.10)

# 定义项目名称和版本（可自定义）
project(HelloWorld VERSION 1.0)

# 添加 utils 子目录（会自动调用 utils/CMakeLists.txt）
add_subdirectory(utils)

# 添加可执行文件
add_executable(main
    main.c  # 你的源代码文件名
)

# 链接 utils 库和 pthread（如果 logger 需要）
target_link_libraries(main PRIVATE
    utils_library  # 链接 utils 库
)