/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2023 Marvell International Ltd.
 */

#ifndef GRO_TRACE_H
#define GRO_TRACE_H

/**
 * @file
 *
 * API for gro trace support
 */

#include <dev_driver.h>
#include <rte_trace_point.h>

#include "rte_gro.h"

#ifdef __cplusplus
extern "C" {
#endif

RTE_TRACE_POINT(
	rte_gro_trace_ctx_create,
	RTE_TRACE_POINT_ARGS(const struct rte_gro_param *param),
	rte_trace_point_emit_u64(param->gro_types);
	rte_trace_point_emit_u16(param->max_flow_num);
	rte_trace_point_emit_u16(param->max_item_per_flow);
)

RTE_TRACE_POINT(
        rte_gro_trace_ctx_destroy,
        RTE_TRACE_POINT_ARGS(const void *ctx),
        rte_trace_point_emit_ptr(ctx);
)

RTE_TRACE_POINT(
        rte_gro_trace_reassemble_burst,
        RTE_TRACE_POINT_ARGS(struct rte_mbuf **pkts,
		uint16_t nb_pkts,
		const struct rte_gro_param *param),
        rte_trace_point_emit_ptr(pkts);
	rte_trace_point_emit_u16(nb_pkts);
	rte_trace_point_emit_u64(param->gro_types);
        rte_trace_point_emit_u16(param->max_flow_num);
        rte_trace_point_emit_u16(param->max_item_per_flow);
)

RTE_TRACE_POINT(
        rte_gro_trace_reassemble,
        RTE_TRACE_POINT_ARGS(struct rte_mbuf **pkts,
		uint16_t nb_pkts,
		const void *ctx),
        rte_trace_point_emit_ptr(pkts);
	rte_trace_point_emit_u16(nb_pkts);
	rte_trace_point_emit_ptr(ctx);
)

RTE_TRACE_POINT(
        rte_gro_trace_timeout_flush,
        RTE_TRACE_POINT_ARGS(const void *ctx,
		uint64_t timeout_cycles,
		uint64_t gro_types,
		struct rte_mbuf **out,
		uint16_t max_nb_out),
        rte_trace_point_emit_ptr(ctx);
	rte_trace_point_emit_u64(timeout_cycles);
	rte_trace_point_emit_u64(gro_types);
	rte_trace_point_emit_ptr(out);
	rte_trace_point_emit_u16(max_nb_out);
)

RTE_TRACE_POINT(
        rte_gro_trace_get_pkt_count,
        RTE_TRACE_POINT_ARGS(const void *ctx),
        rte_trace_point_emit_ptr(ctx);
)


#ifdef __cplusplus
}
#endif

#endif /* GRO_TRACE_H */

