// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2025-2026 SPHARX Ltd. All Rights Reserved.
 *
 * test_error_codes.c — 验证 [SC] error.h 的错误码与故障码空间
 *
 * 错误码按子系统划分子空间：调用方只返回 -AIRY_E*，归因靠数值落在哪一段。
 * 两个子系统共用一个数值时，日志与裁决会把不同的失败解释成同一件事。
 * 这里检查每段边界、段内互异与段间不重叠，故障码单独成段且与错误码不相交。
 */

#include <stddef.h>
#include <stdio.h>

#include <linux/airymax/error.h>

#include "airy_test.h"

static void check_codes(const char *name, const int *codes, size_t n,
			int lo, int hi)
{
	char msg[160];
	size_t i, j;

	for (i = 0; i < n; i++) {
		snprintf(msg, sizeof(msg), "%s[%zu]=%d 落在 [%d, %d]",
			 name, i, codes[i], lo, hi);
		AIRY_TEST(codes[i] >= lo && codes[i] <= hi, msg);
	}

	for (i = 0; i < n; i++) {
		for (j = i + 1; j < n; j++) {
			snprintf(msg, sizeof(msg), "%s[%zu] 与 %s[%zu] 互异",
				 name, i, name, j);
			AIRY_TEST(codes[i] != codes[j], msg);
		}
	}
}

static void test_posix_codes(void)
{
	static const int codes[] = {
		AIRY_EACCES, AIRY_EEXIST, AIRY_EFAULT, AIRY_EINTR,
		AIRY_EINVAL, AIRY_EIO, AIRY_EISDIR, AIRY_ENOENT,
		AIRY_ENOMEM, AIRY_ENOSPC, AIRY_ENOTSUP, AIRY_EPERM,
		AIRY_ERANGE, AIRY_EBUSY, AIRY_ECANCELED, AIRY_EAGAIN,
	};

	AIRY_TEST(AIRY_EOK == 0, "AIRY_EOK == 0");
	AIRY_TEST(AIRY_EAGAIN == 35, "AIRY_EAGAIN == 35");
	check_codes("POSIX", codes, sizeof(codes) / sizeof(codes[0]), 1, 35);
}

static void test_ipc_codes(void)
{
	static const int codes[] = {
		AIRY_EIPC_MAGIC, AIRY_EIPC_OPCODE, AIRY_EIPC_PAYLOAD,
		AIRY_EIPC_HDRSIZE, AIRY_EIPC_RESERVED, AIRY_EIPC_FLAGS,
		AIRY_EIPC_NOTSUPP, AIRY_EIPC_KFIFO, AIRY_EIPC_RECLAIM,
		AIRY_EIPC_CONTEXT, AIRY_EIPC_CRC32, AIRY_EIPC_TIMEOUT,
		AIRY_EIPC_FROZEN,
	};

	AIRY_TEST(AIRY_EIPC_MAGIC == 41, "AIRY_EIPC_MAGIC == 41");
	AIRY_TEST(AIRY_EIPC_FROZEN == 53, "AIRY_EIPC_FROZEN == 53");
	check_codes("IPC", codes, sizeof(codes) / sizeof(codes[0]), 41, 70);
}

static void test_cap_codes(void)
{
	static const int codes[] = {
		AIRY_ECAP_MISSING, AIRY_ECAP_REVOKED, AIRY_ECAP_EXPIRED,
		AIRY_ECAP_MISMATCH, AIRY_ECAP_LSM_DENIED,
		AIRY_ECAP_RADIX_MISS, AIRY_ECAP_STATIC_KEY,
		AIRY_ECAP_BADGE, AIRY_ECAP_EPOCH, AIRY_ECAP_FORGED,
		AIRY_ECAP_PERM, AIRY_ECAP_FROZEN, AIRY_ESEC_D_THROTTLED,
		AIRY_ECAP_OVERFLOW,
	};

	AIRY_TEST(AIRY_ECAP_MISSING == 71, "AIRY_ECAP_MISSING == 71");
	AIRY_TEST(AIRY_ECAP_OVERFLOW == 84, "AIRY_ECAP_OVERFLOW == 84");
	check_codes("CAP", codes, sizeof(codes) / sizeof(codes[0]), 71, 100);
}

static void test_config_codes(void)
{
	static const int codes[] = {
		AIRY_ECFGVERSION, AIRY_ECFGSCHEMA, AIRY_ECFGBASE64,
		AIRY_ECFGJSON, AIRY_ECFGIO,
	};

	check_codes("CFG", codes, sizeof(codes) / sizeof(codes[0]),
		    101, 120);
}

static void test_sched_codes(void)
{
	static const int codes[] = {
		AIRY_ESCHED_POLICY, AIRY_ESCHED_BUDGET, AIRY_ESCHED_DEADLINE,
		AIRY_ESCHED_PERIOD, AIRY_ESCHED_PRIO, AIRY_ESCHED_WEIGHT,
		AIRY_ELIFECYCLE_STATE, AIRY_ELIFECYCLE_TRANS,
		AIRY_ELIFECYCLE_AGENT, AIRY_ELIFECYCLE_ZOMBIE,
	};

	check_codes("SCHED/LC", codes, sizeof(codes) / sizeof(codes[0]),
		    121, 140);
}

static void test_mem_codes(void)
{
	static const int codes[] = {
		AIRY_EMEM_TIER, AIRY_EMEM_GFP, AIRY_EMEM_PMEM,
		AIRY_EMEM_CXL, AIRY_EMEM_PAGE_CLASS, AIRY_EMEM_MMAP,
		AIRY_EMEM_ALLOC, AIRY_EMEM_OOM,
	};

	check_codes("MEM", codes, sizeof(codes) / sizeof(codes[0]),
		    141, 160);
}

static void test_cog_codes(void)
{
	static const int codes[] = {
		AIRY_ECOG_PHASE, AIRY_ECOG_MODE, AIRY_ECOG_Q16,
		AIRY_ECOG_TIMEOUT, AIRY_ECOG_ITERATIONS,
		AIRY_ECOG_CONFIDENCE,
	};

	check_codes("COG", codes, sizeof(codes) / sizeof(codes[0]),
		    161, 180);
}

static void test_log_codes(void)
{
	static const int codes[] = {
		AIRY_ELOG_RING, AIRY_ELOG_FULL, AIRY_ELOG_LEVEL,
		AIRY_ELOG_FACILITY, AIRY_ELOG_PERSIST, AIRY_ELOG_MAGIC,
	};

	check_codes("LOG", codes, sizeof(codes) / sizeof(codes[0]),
		    181, 200);
}

static void test_obj_codes(void)
{
	static const int codes[] = {
		AIRY_EOBJ_HANDLE, AIRY_EOBJ_REFCOUNT, AIRY_EOBJ_TYPE,
		AIRY_EOBJ_GONE,
	};

	check_codes("OBJ", codes, sizeof(codes) / sizeof(codes[0]),
		    201, 220);
}

static void test_sys_codes(void)
{
	static const int codes[] = {
		AIRY_ESYS_NUMBER, AIRY_ESYS_ARGS, AIRY_ESYS_DISABLED,
		AIRY_ESYS_ABI,
	};

	check_codes("SYS", codes, sizeof(codes) / sizeof(codes[0]),
		    221, 240);
}

static void test_enc_codes(void)
{
	static const int codes[] = {
		AIRY_EENC_INVALID, AIRY_EENC_OVERLONG, AIRY_EENC_SURROGATE,
		AIRY_EENC_NONCHAR, AIRY_EENC_RANGE, AIRY_EENC_BOM,
	};

	AIRY_TEST(AIRY_EENC_INVALID == 241, "AIRY_EENC_INVALID == 241");
	AIRY_TEST(AIRY_EENC_BOM == 246, "AIRY_EENC_BOM == 246");
	check_codes("ENC", codes, sizeof(codes) / sizeof(codes[0]),
		    241, 260);
}

static void test_lockdown_code(void)
{
	AIRY_TEST(AIRY_ELOCKDOWN == 261, "AIRY_ELOCKDOWN == 261");
}

static void test_subspaces_ordered(void)
{
	static const struct {
		const char *name;
		int lo;
		int hi;
	} subspaces[] = {
		{ "POSIX", 1, 35 },
		{ "IPC", 41, 70 },
		{ "CAP", 71, 100 },
		{ "CFG", 101, 120 },
		{ "SCHED/LC", 121, 140 },
		{ "MEM", 141, 160 },
		{ "COG", 161, 180 },
		{ "LOG", 181, 200 },
		{ "OBJ", 201, 220 },
		{ "SYS", 221, 240 },
		{ "ENC", 241, 260 },
		{ "LOCKDOWN", 261, 280 },
	};
	const size_t n = sizeof(subspaces) / sizeof(subspaces[0]);
	char msg[160];
	size_t i;

	for (i = 0; i + 1 < n; i++) {
		snprintf(msg, sizeof(msg), "%s 上界 %d 低于 %s 下界 %d",
			 subspaces[i].name, subspaces[i].hi,
			 subspaces[i + 1].name, subspaces[i + 1].lo);
		AIRY_TEST(subspaces[i].hi < subspaces[i + 1].lo, msg);
	}
}

static void test_fault_codes(void)
{
	static const int faults[] = {
		AIRY_FAULT_CAP_FORGED, AIRY_FAULT_CAP_LEAK,
		AIRY_FAULT_RING_CORRUPT, AIRY_FAULT_TIMEOUT,
		AIRY_FAULT_ABNORMAL_CAP, AIRY_FAULT_VM_FAULT,
		AIRY_FAULT_DOUBLE_FAULT,
	};
	const size_t n = sizeof(faults) / sizeof(faults[0]);
	char msg[160];
	size_t i, j;

	AIRY_TEST(AIRY_FAULT_CAP_FORGED == 0x1001,
		  "AIRY_FAULT_CAP_FORGED == 0x1001");
	AIRY_TEST(AIRY_FAULT_DOUBLE_FAULT == 0x100C,
		  "AIRY_FAULT_DOUBLE_FAULT == 0x100C");

	for (i = 0; i < n; i++) {
		snprintf(msg, sizeof(msg), "FAULT[%zu]=0x%X 高于错误码上界",
			 i, faults[i]);
		AIRY_TEST(faults[i] > 280, msg);
		for (j = i + 1; j < n; j++) {
			snprintf(msg, sizeof(msg), "FAULT[%zu] 与 FAULT[%zu] 互异",
				 i, j);
			AIRY_TEST(faults[i] != faults[j], msg);
		}
	}
}

int main(void)
{
	printf("=== AirymaxOS [SC] 错误码与故障码契约测试 ===\n");

	test_posix_codes();
	test_ipc_codes();
	test_cap_codes();
	test_config_codes();
	test_sched_codes();
	test_mem_codes();
	test_cog_codes();
	test_log_codes();
	test_obj_codes();
	test_sys_codes();
	test_enc_codes();
	test_lockdown_code();
	test_subspaces_ordered();
	test_fault_codes();

	airy_test_report("test_error_codes");
	return EXIT_SUCCESS;
}
