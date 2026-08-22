# ===========================================================================
# agent-linux (AirymaxOS) tests-linux submodule CMakeLists.txt
# 构建入口：unit + integration + fuzz + perf 测试
# 决策 F1：最小可编译骨架（非桩）
# ===========================================================================

cmake_minimum_required(VERSION 3.20)

project(airy_tests
	VERSION 0.1.1
	LANGUAGES C
	DESCRIPTION "AirymaxOS tests-linux submodule — unit + integration + fuzz + perf")

# ---------------------------------------------------------------------------
# C 标准：C11（用户态测试）
# ---------------------------------------------------------------------------
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# [SC] 头文件引用（OS-IRON-014 / 决策 G1）
# 唯一物理宿主：kernel/include/uapi/linux/airymax/
# 禁止物理副本，通过 -I 引用
#
# 三条引用路径覆盖两种 include 风格：
#   - <airymax/foo.h>           ← kernel/include/uapi/linux（旧风格，部分测试沿用）
#   - <linux/airymax/foo.h>     ← kernel/include/uapi（UAPI 标准风格，[SC] 头文件内部使用）
#   - <linux/foo.h>             ← kernel/include（内核内部头文件，仅用于 <linux/types.h> 等）
# ---------------------------------------------------------------------------
include_directories(${CMAKE_SOURCE_DIR}/../kernel/include)
include_directories(${CMAKE_SOURCE_DIR}/../kernel/include/uapi)
include_directories(${CMAKE_SOURCE_DIR}/../kernel/include/uapi/linux)

# ---------------------------------------------------------------------------
# 编译选项（OS-STD-FMT-001/002）
# 测试编译允许 -Wno-unused（部分测试可能不使用所有参数）
# ---------------------------------------------------------------------------
add_compile_options(
	-Wall
	-Wextra
	-Werror
	-Wno-unused-parameter
	-Wno-unused-function
	-fstack-protector-strong
	-D_FORTIFY_SOURCE=2
)

# ---------------------------------------------------------------------------
# 启用 CTest 测试运行器
# ---------------------------------------------------------------------------
enable_testing()

# ---------------------------------------------------------------------------
# 子目录
# 0.1.1 仅 unit/test_ipc_magic 可编译，其余 1.0.1 补齐
# ---------------------------------------------------------------------------
add_subdirectory(unit)

# ---------------------------------------------------------------------------
# 自定义目标：运行全部测试
# ---------------------------------------------------------------------------
add_custom_target(check
	COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
	DEPENDS test_ipc_magic
	COMMENT "运行 AirymaxOS 全部测试"
)
