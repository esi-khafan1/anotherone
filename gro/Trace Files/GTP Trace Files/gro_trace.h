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

RTE_TRACE_POINT(
        rte_gro_trace_macro_values_reassemble_burst,
        RTE_TRACE_POINT_ARGS(uint32_t ptype, uint8_t is_ipv4_tcp, uint8_t is_ipv6_tcp, 
		uint8_t is_ipv4_udp, uint8_t is_ipv4_vxlan_tcp4, uint8_t is_ipv4_vxlan_udp4,
		uint8_t is_ipv4_gtp_udp4, uint8_t is_ipv4_gtp_tcp4),
        rte_trace_point_emit_u32(ptype);
	rte_trace_point_emit_u8(is_ipv4_tcp);
	rte_trace_point_emit_u8(is_ipv6_tcp);
	rte_trace_point_emit_u8(is_ipv4_udp);
	rte_trace_point_emit_u8(is_ipv4_vxlan_tcp4);
	rte_trace_point_emit_u8(is_ipv4_vxlan_udp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_udp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_tcp4);
)

RTE_TRACE_POINT(
        rte_gro_trace_macro_values_reassemble,
        RTE_TRACE_POINT_ARGS(uint32_t ptype, uint8_t is_ipv4_tcp, uint8_t is_ipv6_tcp, 
		uint8_t is_ipv4_udp, uint8_t is_ipv4_vxlan_tcp4, uint8_t is_ipv4_vxlan_udp4,
		uint8_t is_ipv4_gtp_udp4, uint8_t is_ipv4_gtp_tcp4),
        rte_trace_point_emit_u32(ptype);
	rte_trace_point_emit_u8(is_ipv4_tcp);
	rte_trace_point_emit_u8(is_ipv6_tcp);
	rte_trace_point_emit_u8(is_ipv4_udp);
	rte_trace_point_emit_u8(is_ipv4_vxlan_tcp4);
	rte_trace_point_emit_u8(is_ipv4_vxlan_udp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_udp4);
	rte_trace_point_emit_u8(is_ipv4_gtp_tcp4);
)

RTE_TRACE_POINT(
        rte_gro_trace_gtp_tcp4_headers,
        RTE_TRACE_POINT_ARGS(struct rte_mbuf *pkt, void *outer_eth_hdr, void *outer_ipv4_hdr,
		void *udp_hdr, void *gtp_hdr, void *eth_hdr, void *ipv4_hdr, void *tcp_hdr),
        rte_trace_point_emit_ptr(pkt);
	rte_trace_point_emit_ptr(outer_eth_hdr);
	rte_trace_point_emit_ptr(outer_ipv4_hdr);
	rte_trace_point_emit_ptr(udp_hdr);
	rte_trace_point_emit_ptr(gtp_hdr);
	rte_trace_point_emit_ptr(eth_hdr);
	rte_trace_point_emit_ptr(ipv4_hdr);
	rte_trace_point_emit_ptr(tcp_hdr);
)

RTE_TRACE_POINT(
        rte_gro_trace_gtp_udp4_headers,
        RTE_TRACE_POINT_ARGS(struct rte_mbuf *pkt, void *outer_eth_hdr, void *outer_ipv4_hdr,
		void *udp_hdr, void *gtp_hdr, void *eth_hdr, void *ipv4_hdr),
        rte_trace_point_emit_ptr(pkt);
	rte_trace_point_emit_ptr(outer_eth_hdr);
	rte_trace_point_emit_ptr(outer_ipv4_hdr);
	rte_trace_point_emit_ptr(udp_hdr);
	rte_trace_point_emit_ptr(gtp_hdr);
	rte_trace_point_emit_ptr(eth_hdr);
	rte_trace_point_emit_ptr(ipv4_hdr);
)

RTE_TRACE_POINT(
        rte_gro_trace_tcp4_headers,
        RTE_TRACE_POINT_ARGS(struct rte_mbuf *pkt, void *eth_hdr, void *ipv4_hdr, void *tcp_hdr),
        rte_trace_point_emit_ptr(pkt);
	rte_trace_point_emit_ptr(eth_hdr);
	rte_trace_point_emit_ptr(ipv4_hdr);
	rte_trace_point_emit_ptr(tcp_hdr);
)


#ifdef __cplusplus
}
#endif

#endif /* GRO_TRACE_H */

