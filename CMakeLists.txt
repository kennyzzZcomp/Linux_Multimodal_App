# CMakeLists.txt - 最简单的版本
cmake_minimum_required(VERSION 3.10)
project(conv_demo)

# 设置C++标准
set(CMAKE_CXX_STANDARD 11)

# 设置包含目录
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/external
    ${CMAKE_SOURCE_DIR}/src
)

# 添加可执行文件
add_executable(conv_demo
    src/main.cpp
    src/conversation_handler.cpp
    src/audio_handler.cpp
    external/jsoncpp.cpp
)

## 先查找 libconversation（优先项目 lib，然后系统目录）
find_library(CONV_LIB NAMES conversation
    PATHS ${CMAKE_SOURCE_DIR}/lib /usr/local/lib /usr/lib
)
if (NOT CONV_LIB)
    message(FATAL_ERROR "libconversation not found; please place libconversation.so in ${CMAKE_SOURCE_DIR}/lib or install the SDK")
endif()

# 链接库（使用 find_library 返回的绝对路径更可靠）
target_link_libraries(conv_demo
    PRIVATE ${CONV_LIB} dl pthread z
)

# 设置输出路径（可选）
# set_target_properties(conv_demo PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})