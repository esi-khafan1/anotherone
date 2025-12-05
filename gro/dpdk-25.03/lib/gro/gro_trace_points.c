/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2020 Marvell International Ltd.
 */

#include <rte_trace_point_register.h>

#include <gro_trace.h>

RTE_TRACE_POINT_REGISTER(rte_gro_trace_macro_values_reassemble_burst,
        lib.gro_csum.gro.macro.values.reassemble.burst)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_macro_values_reassemble,
        lib.gro_csum.gro.macro.values.reassemble)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_tcp4_reassemble_output,
        lib.gro_csum.gro.tcp4.reassemble.output)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_tcp4_reassemble_error_line,
        lib.gro_csum.gro.tcp4.reassemble.error.line)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_reassemble_start,
        lib.gro_csum.gro.reassemble.start)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_reassemble_end,
        lib.gro_csum.gro.reassemble.end)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_timeout_start,
        lib.gro_csum.gro.timeout.start)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_timeout_end,
        lib.gro_csum.gro.timeout.end)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_pkt_count_start,
        lib.gro_csum.gro.pkt.count.start)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_pkt_count_end,
        lib.gro_csum.gro.pkt.count.end)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_create_start,
        lib.gro_csum.gro.create.start)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_create_end,
        lib.gro_csum.gro.create.end)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_destroy_start,
        lib.gro_csum.gro.destroy.start)

RTE_TRACE_POINT_REGISTER(rte_gro_trace_destroy_end,
        lib.gro_csum.gro.destroy.end)
