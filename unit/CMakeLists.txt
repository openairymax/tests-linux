# ===========================================================================
# unit tests CMakeLists.txt
# 单元测试目录（[IND]）
# 决策 F1：真实可编译实现（非桩）
#
# 每个用例是一个独立可执行文件与一条 CTest 用例。目标与源文件写成字面量，
# 让结构校验能在静态层面看到构建入口确实引用了源文件；标签与描述集中登记。
# 新增用例：加一条 add_executable、一条 add_test 与一条 set_tests_properties。
# ===========================================================================

add_executable(test_ipc_magic test_ipc_magic.c)
add_executable(test_log_ring_uapi test_log_ring_uapi.c)
add_executable(test_superv_uapi test_superv_uapi.c)
add_executable(test_error_codes test_error_codes.c)

foreach(tgt IN ITEMS
	test_ipc_magic
	test_log_ring_uapi
	test_superv_uapi
	test_error_codes
)
	target_compile_definitions(${tgt} PRIVATE _GNU_SOURCE)
	add_test(NAME ${tgt} COMMAND ${tgt})
endforeach()

set_tests_properties(test_ipc_magic PROPERTIES
	LABELS "unit;sc;ipc"
	DESCRIPTION "[SC] ipc.h 的 AIRY_IPC_MAGIC 与消息头布局")
set_tests_properties(test_log_ring_uapi PROPERTIES
	LABELS "unit;sc;log"
	DESCRIPTION "[SC] log_types.h/log_ring.h 的记录与映射布局")
set_tests_properties(test_superv_uapi PROPERTIES
	LABELS "unit;sc;superv"
	DESCRIPTION "[SC] superv.h 的故障通道与裁决契约")
set_tests_properties(test_error_codes PROPERTIES
	LABELS "unit;sc;error"
	DESCRIPTION "[SC] error.h 的错误码子空间与故障码")
