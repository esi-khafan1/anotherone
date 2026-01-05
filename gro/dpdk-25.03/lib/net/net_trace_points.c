/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2020 Marvell International Ltd.
 */

#include <rte_trace_point_register.h>

#include <net_trace.h>

RTE_TRACE_POINT_REGISTER(rte_net_trace_msg_type,
        lib.net.gprs.hdr.massage.type)

RTE_TRACE_POINT_REGISTER(rte_net_trace_port_number,
        lib.net.gprs.hdr.port.number)

RTE_TRACE_POINT_REGISTER(rte_net_trace_protocol,
        lib.net.protocol)
