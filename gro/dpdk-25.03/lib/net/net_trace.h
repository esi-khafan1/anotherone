/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2023 Marvell International Ltd.
 */

#ifndef NET_TRACE_H
#define NET_TRACE_H

/**
 * @file
 *
 * API for net trace support
 */

#include <dev_driver.h>
#include <rte_trace_point.h>

#include "rte_net.h"

#ifdef __cplusplus
extern "C" {
#endif

RTE_TRACE_POINT(
    rte_net_trace_msg_type,
    RTE_TRACE_POINT_ARGS(uint8_t msg_type),
    rte_trace_point_emit_u8(msg_type);
)

RTE_TRACE_POINT(
    rte_net_trace_port_number,
    RTE_TRACE_POINT_ARGS(uint16_t port_number),
    rte_trace_point_emit_u16(port_number);
)

RTE_TRACE_POINT(
    rte_net_trace_protocol,
    RTE_TRACE_POINT_ARGS(uint16_t proto),
    rte_trace_point_emit_u16(proto);
)


#ifdef __cplusplus
}
#endif

#endif /* NET_TRACE_H */