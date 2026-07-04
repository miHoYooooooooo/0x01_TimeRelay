# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 15_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# 引入SDK(必须在project()之前执行)
include(pico_sdk_import.cmake)

# 设置项目名
project(0x01_TimeRelay C CXX ASM)

# 初始化SDK库
pico_sdk_init()

# 头文件路径
include_directories(./code/user)
include_directories(./code/hardware)
include_directories(./code/lib/lvgl-8.3.11/examples/porting)
# include_directories(./code/ui)

# 源文件路径
aux_source_directory(./code/user                                SRC_DIR)
aux_source_directory(./code/hardware                            SRC_DIR)
aux_source_directory(./code/lib/lvgl-8.3.11/examples/porting    SRC_DIR)

# 添加子目录
add_subdirectory(./code/lib/lvgl-8.3.11)
add_subdirectory(./code/ui)

# 生成可执行文件
add_executable(${PROJECT_NAME} ${SRC_DIR})

# 生成各种形式的可执行文件：map/bin/hex/uf2
pico_add_extra_outputs(${PROJECT_NAME})

# 设置当前程序版本
pico_set_program_version(${PROJECT_NAME} "V1.0.0")

# 选择打印输出
pico_enable_stdio_uart(${PROJECT_NAME} 1)
pico_enable_stdio_usb (${PROJECT_NAME} 0)

# 链接库
target_link_libraries(${PROJECT_NAME}
        pico_stdlib
        lvgl
        ui
        hardware_dma
        hardware_spi
        hardware_pwm
    )

# 每个函数/变量放独立段
target_compile_options(${PROJECT_NAME} PUBLIC
    -ffunction-sections
    -fdata-sections
)

# 把未引用段全部丢弃
target_link_options(${PROJECT_NAME} PUBLIC
    -Wl,--gc-sections
)

# 自定义编译完成后执行的命令
add_custom_command(
       TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_LIST_DIR}/bin                                    # 创建bin文件夹
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin ${CMAKE_CURRENT_LIST_DIR}/bin/     # 拷贝bin文件
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.uf2 ${CMAKE_CURRENT_LIST_DIR}/bin/     # 拷贝uf2文件

        COMMAND ${CMAKE_COMMAND} -E echo ""
        COMMAND ${CMAKE_COMMAND} -E echo "================= BUILD REPORT =================="

        # ELF size（FLASH / RAM）
        COMMAND arm-none-eabi-size ${PROJECT_NAME}.elf

        COMMAND ${CMAKE_COMMAND} -E echo "-------------------------------------------------"

        # Python 汇总报告
        COMMAND  ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tool/build_report.py
                ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.elf
                ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin
                2048

        COMMAND ${CMAKE_COMMAND} -E echo "================================================="
)
