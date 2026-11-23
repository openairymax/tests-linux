// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * airy_test.h — 单元测试共用断言与汇总
 *
 * 每个用例都是独立可执行文件：断言失败立即以非零码退出，CTest 据此归因。
 * 宏与计数器集中在这里，避免每个用例各抄一份判定；写坏一处时不会出现
 * 「有的用例失败即退出、有的只打印」两种语义。
 */

#ifndef AIRY_TESTS_UNIT_AIRY_TEST_H
#define AIRY_TESTS_UNIT_AIRY_TEST_H

#include <stdio.h>
#include <stdlib.h>

static int g_test_count;
static int g_test_passed;

#define AIRY_TEST(cond, msg)						\
	do {								\
		g_test_count++;						\
		if (cond) {						\
			g_test_passed++;				\
			printf("[PASS] %s:%d %s\n",			\
			       __FILE__, __LINE__, (msg));		\
		} else {						\
			fprintf(stderr, "[FAIL] %s:%d %s\n",		\
				__FILE__, __LINE__, (msg));		\
			exit(EXIT_FAILURE);				\
		}							\
	} while (0)

/*
 * airy_test_report — 打印汇总并确认计数自洽
 *
 * 断言失败在宏内即退出，这里只剩一种可观察结果；计数不自洽说明有人绕过
 * 宏直接改计数器，按失败处理，避免汇总掩盖漏跑。
 */
static void airy_test_report(const char *name)
{
	printf("\n=== %s: %d/%d 通过 ===\n",
	       name, g_test_passed, g_test_count);
	if (g_test_passed != g_test_count)
		exit(EXIT_FAILURE);
	printf("[OK] %s 全部断言通过\n", name);
}

#endif /* AIRY_TESTS_UNIT_AIRY_TEST_H */
