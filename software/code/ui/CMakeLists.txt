# 递归查找源码
file(GLOB_RECURSE UI_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/custom/*.c
    ${CMAKE_CURRENT_SOURCE_DIR}/generated/*.c
)

# 创建库（推荐：直接带 sources）
add_library(ui STATIC ${UI_SOURCES})

# 头文件路径
target_include_directories(ui PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/custom
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
)

# 链接 LVGL
target_link_libraries(ui PUBLIC lvgl)