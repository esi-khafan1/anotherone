/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2020 Marvell International Ltd.
 */

#include <rte_trace_point_register.h>

#include <gro_trace.h>

RTE_TRACE_POINT_REGISTER(rte_gro_trace_ctx_create,
        lib.gro.ctx.create)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_ctx_destroy,
        lib.gro.ctx.destroy)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_reassemble_burst,
        lib.gro.reassemble.burst)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_reassemble,
        lib.gro.reassemble)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_timeout_flush,
        lib.gro.timeout.flush)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_get_pkt_count,
        lib.gro.get.pkt.count)

