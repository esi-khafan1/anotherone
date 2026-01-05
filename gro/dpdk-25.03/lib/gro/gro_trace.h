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
    rte_gro_trace_macro_values_reassemble_burst,
    RTE_TRACE_POINT_ARGS(uint32_t ptype, uint8_t is_ipv4_tcp, uint8_t is_ipv6_tcp, 
    uint8_t is_ipv4_udp, uint8_t is_ipv4_vxlan_tcp4, uint8_t is_ipv4_vxlan_udp4),
    rte_trace_point_emit_u32(ptype);
	rte_trace_point_emit_u8(is_ipv4_tcp);
	rte_trace_point_emit_u8(is_ipv6_tcp);
	rte_trace_point_emit_u8(is_ipv4_udp);
	rte_trace_point_emit_u8(is_ipv4_vxlan_tcp4);
	rte_trace_point_emit_u8(is_ipv4_vxlan_udp4);
)

RTE_TRACE_POINT(
    rte_gro_trace_macro_values_reassemble,
    RTE_TRACE_POINT_ARGS(uint32_t ptype, uint8_t is_ipv4_tcp, uint8_t is_ipv6_tcp, 
    uint8_t is_ipv4_udp, uint8_t is_ipv4_vxlan_tcp4, uint8_t is_ipv4_vxlan_udp4,
	uint8_t is_ipv4_gtp_tcp4, uint8_t is_ipv4_gtp_udp4),
    rte_trace_point_emit_u32(ptype);
	rte_trace_point_emit_u8(is_ipv4_tcp);
	rte_trace_point_emit_u8(is_ipv6_tcp);
	rte_trace_point_emit_u8(is_ipv4_udp);
	rte_trace_point_emit_u8(is_ipv4_vxlan_tcp4);
	rte_trace_point_emit_u8(is_ipv4_vxlan_udp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_tcp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_udp4);
)

RTE_TRACE_POINT(
	rte_gro_trace_gtp4_reassemble_output,
	RTE_TRACE_POINT_ARGS(int32_t answer),
	rte_trace_point_emit_i32(answer);
)

RTE_TRACE_POINT(
	rte_gro_trace_types,
	RTE_TRACE_POINT_ARGS(uint64_t gro_ctx_type),
	rte_trace_point_emit_u64(gro_ctx_type);
)

RTE_TRACE_POINT(
	rte_gro_trace_do_gtp_tcp,
	RTE_TRACE_POINT_ARGS(uint8_t do_gtp_tcp),
	rte_trace_point_emit_u8(do_gtp_tcp);
)

RTE_TRACE_POINT(
	rte_gro_trace_tcp4_reassemble_error_line,
	RTE_TRACE_POINT_ARGS(int32_t line),
	rte_trace_point_emit_i32(line);
)

RTE_TRACE_POINT(
	rte_gro_trace_reassemble_start,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_reassemble_end,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_timeout_start,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_timeout_end,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_pkt_count_start,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_pkt_count_end,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_create_start,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_create_end,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_destroy_start,
	RTE_TRACE_POINT_ARGS(),
)

RTE_TRACE_POINT(
	rte_gro_trace_destroy_end,
	RTE_TRACE_POINT_ARGS(),
)

#ifdef __cplusplus
}
#endif

#endif /* GRO_TRACE_H */
