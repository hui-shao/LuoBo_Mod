cmake_minimum_required(VERSION 3.23)

project(LuoBo_Mod C)

add_executable(LuoBo_Mod_Colorful main.c main.h main.h print.h print.c res/logo.rc)  # 彩色版本
add_executable(LuoBo_Mod_NoColor main.c main.h main.h print.h print.c res/logo.rc)   # 无色版本

# 添加UAC请求管理员权限
set_target_properties(LuoBo_Mod_Colorful LuoBo_Mod_NoColor PROPERTIES LINK_FLAGS " /MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" ")

# 根据目标配置颜色类型
target_compile_definitions(LuoBo_Mod_Colorful PRIVATE PRINT_COLORFUL=1)
target_compile_definitions(LuoBo_Mod_NoColor PRIVATE PRINT_COLORFUL=0)

# 根据编译类型选择日志等级
if (CMAKE_BUILD_TYPE AND (CMAKE_BUILD_TYPE STREQUAL "Debug"))
    add_compile_definitions(PRINT_LEVEL=LEVEL_DEBUG)
else ()
    add_compile_definitions(PRINT_LEVEL=LEVEL_INFO)
endif ()
