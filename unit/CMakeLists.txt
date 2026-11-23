# ===========================================================================
# unit tests CMakeLists.txt
# 单元测试目录（[IND]）
# 决策 F1：真实可编译实现（非桩）
#
# 每个用例是一个独立可执行文件与一条 CTest 用例：
#   <目标>|<标签，逗号分隔>|<描述>
# 断言失败即非零退出，CTest 按用例名归因；新增用例只加一行登记。
# ===========================================================================

foreach(spec IN ITEMS
	"test_ipc_magic|unit,sc,ipc|[SC] ipc.h 的 AIRY_IPC_MAGIC 与消息头布局"
	"test_log_ring_uapi|unit,sc,log|[SC] log_types.h/log_ring.h 的记录与映射布局"
	"test_superv_uapi|unit,sc,superv|[SC] superv.h 的故障通道与裁决契约"
	"test_error_codes|unit,sc,error|[SC] error.h 的错误码子空间与故障码"
)
	string(REPLACE "|" ";" fields "${spec}")
	list(GET fields 0 tgt)
	list(GET fields 1 labels)
	list(GET fields 2 desc)
	string(REPLACE "," ";" labels "${labels}")

	add_executable(${tgt} ${tgt}.c)
	target_compile_definitions(${tgt} PRIVATE _GNU_SOURCE)

	add_test(NAME ${tgt} COMMAND ${tgt})
	set_tests_properties(${tgt} PROPERTIES
		LABELS "${labels}"
		DESCRIPTION "${desc}")
endforeach()
