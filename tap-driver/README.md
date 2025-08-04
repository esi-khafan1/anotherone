# Dpdk-TAP-Driver


This project is intended to evaluate network performance by utilizing DPDK (testpmd), virtual TAP interfaces, and the tcpreplay utility. The structured workflow for implementation and analysis is outlined below:

## 1. Installing and Building DPDK with Function Tracing Support
1. **Download the Latest DPDK Version**
  
    Retrieve the latest release from the [official DPDK website](https://core.dpdk.org/download/)

<br>

2. **Extract the Archive**
    ```shell
    tar xJf dpdk-<version>.tar.xz
    cd dpdk-<version>
    ```
<br>

3. **Configure the Build Environment Using Meson**
    ```shell
    meson setup build \
    -Dexamples=all \
    -Dlibdir=lib \
    -Denable_trace_fp=true \
    -Dc_args="-finstrument-functions"
    ```
    > *The `-Dc_args="-finstrument-functions"` Meson configuration flag ensures that function entry and exit points are instrumented during compilation. This is essential for LTTng to capture user-space function traces; without it, the resulting trace data may be incomplete or empty.*

<br>

4. **Build and Install Using Ninja**
    ```shell
    cd build
    ninja
    meson install
    ldconfig
    ```
    > Note: Root privileges are required for the last two commands.
  
  <br>

  The compiled binaries will be located in the /build/app directory.

  <br>

## 2. Configure hugepages and mount 1GB pagesize

```shell
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages
mkdir /mnt/huge
mount -t hugetlbfs pagesize=1GB /mnt/huge
```

Alternatively, the following method can be used:

```shell
sudo sysctl -w vm.nr_hugepages=1024
mount -t hugetlbfs none /dev/hugepages
```


<br>

## 3. Create two TAP interfaces for DPDK's TAP Poll Mode Driver (PMD)

Within the dpdk-<version>/build directory, execute the testpmd application using the following command:

```shell
sudo LD_PRELOAD=/usr/lib/x86_64-linux-gnu/liblttng-ust-cyg-profile.so.1 ./app/dpdk-testpmd -l 0-1 -n 2   --vdev=net_tap0,iface=tap0   --vdev=net_tap1,iface=tap1   --   -i
 ```
 **Functionality:**

+ Creates net_tap0 and net_tap1 virtual devices

+ Assigns 2 CPU cores (-l 0-1)

+ Starts in interactive mode (-i)

<br>

> LD_PRELOAD=/usr/lib/x86_64-linux-gnu/liblttng-ust-cyg-profile.so forces the DPDK application to load LTTng's function tracing library first, enabling detailed profiling of function calls for performance analysis. This allows tracking exact timing and frequency of every function call in DPDK (like packet processing functions) to identify bottlenecks.

<br>

**Note:**

Depending on your kernel version, the exact `liblttng-ust-cyg-profile.so` file may not be present, even if the `liblttng-ust-dev` package has been installed. To determine the specific version available on your system, you can use the following command:
```shel
ls /usr/lib/x86_64-linux-gnu/ | grep liblttng-ust-cyg-profile.so
```
For example, the output on one system may appear as follows:

![liblttng-ust-cyg-profile.so](Pics/liblttng-ust-cyg-profile.so.png)

<br>

The terminal output should appear as follows after executing the testpmd command:

![testpmd](Pics/testpmd1.png)

By executing the command `show port stats all`, you will be able to view the statistics for all ports.

![show-port-stats-all](Pics/show-port-stats-all.png)

<br>

## 4. Create Additional RX/TX Queues
In the following step, we will add a new queue while operating in TAP mode. However, before proceeding, it is recommended to retrieve relevant configuration details using the `show config fwd` command.

![show-config-fwd1](Pics/show-config-fwd1.png)

- `Forwarding Mode: io` -> This means packets received on a port are simply transmitted out through the corresponding transmit queue of another port.

- `Ports: 2` -> Two physical or virtual ports are active.

- `Cores: 1` -> One logical core (Core 5 in this case) is being used to process all forwarding tasks.

- `Streams: 2` -> Two forwarding streams are configured, each representing a receive/transmit (RX/TX) path between ports.

- `NUMA support: Enabled` -> The application is aware of NUMA (Non-Uniform Memory Access) node placement.

- `MP Allocation Mode: native` -> Memory pools are allocated in native DPDK mode.

<br>

To add a new queue, we need to perform the following actions in testpmd:

- Stoping all ports: `port stop all`

- Creating new RX and TX queues: `port config all rxq 2` & `port config all txq 2`

- Start the ports again: `port start all`

![New-RX-TX-Queue](Pics/New-RX-TX-Queue.png)

<br>

After the queues have been created, the output of `show config fwd` should appear as follows:

![show-config-forward-after-creating-queues](Pics/show-config-forward-after-creating-queues.png)


## 5. Create Flow Filtering Rule in testpmd

To direct specific types of traffic to designated queues, you can create a flow filtering rule in testpmd using the following command.

```shell
flow create 0 ingress pattern eth / ipv4 / udp / end actions queue index 0 / end
```
<br>

If the operation completes successfully, you should observe an output similar to the following:

![Flow-Rule](Pics/Flow-Rule.png)


This command creates a rule on port 0 that:

  - Matches any incoming UDP over IPv4 over Ethernet traffic.

  - Routes the matched packets to RX queue 0.

<br>

## 6. Install and Run tcpreplay
Subsequently, we need to clone tcpreplay from the [TCP-Replay](https://github.com/appneta/tcpreplay/releases/tag/v4.5.1) repository to send the traffic to one of the interfaces we've created using `testpmd`.
After downloading the `tcpreplay-4.5.1.tar.gz` archive, compile and install it. Once the installation is complete, open a new terminal session and execute the following command within that terminal.

```shell
./configure --disable-tuntap
make
sudo make install
```

### Note:
 *You may need to install some build tools, such as `make` or `automake`, to successfully compile the TCPReplay project.*
  *You can typically install them on Ubuntu with:* <br>
  >*`sudo apt update`* <br>
  *`sudo apt install build-essential automake`*

<br>

After completing the installation, you can run the previously generated pcap file using the command shown below.

```shell
tcpreplay -i tap0 --loop=1000 ./Capture.pcap 
```
<br>

![TCPReplay](Pics/TCPReplay.png)

<br>

Next, in the `testpmd` terminal window, enter the command `start`, followed by `show port stats all` to observe the packet flow on TAP interface 0.

![show-port-stats-all-after-running-tcpreplay](Pics/show-port-stats-all-after-running-tcpreplay.png)

<br>

## 7. Setting Up an LTTng Trace Session
  In order to Automate the LTTng capture, create a shell script to configure the LTTng session. The script initializes the session, adds the necessary context fields, starts tracing, sleeps for a specified duration, and then stops and destroys the session.


  ```shell
  touch script.sh
  chmod +x script.sh
  nano script.sh
  ```
  Paste the following commands into the file:
      
 

```shell
#!/bin/bash
lttng create libpcap
lttng enable-channel --userspace --num-subbuf=4 --subbuf-size=40M channel0
#lttng enable-channel --userspace channel0
lttng enable-event --channel channel0 --userspace --all
lttng add-context --channel channel0 --userspace --type=vpid --type=vtid --type=procname
lttng start
sleep 2
lttng stop
lttng destroy
```
<br>

# 1  Tracing Analysis - UDP Filtering

---

### Executive summary  
The 2-second LTTng capture (~11 million events) indicates that **most of the additional CPU time observed after installing the software flow rule *eth / ipv4 / udp → queue 0* is consumed by the software packet-type (PTYPE) parser on the receive hot-path of the TAP PMD**.  
Because the rule requires classification of every frame’s L3/L4 headers to decide whether it is UDP, the parser executes entirely in software on the same logical core (Core 5) that hosts the `dpdk-worker` thread.

> ▸ _The flame-graph slice covering **42 – 54 µs** on the timeline displays a wide orange frame (`pkt_burst_io_forward`) with a dense stack of six short helper functions repeated for each packet, clearly indicating execution inside the burst Rx/Tx loop._

---

![Event-Density0](Pics/Event-Density0.png)

<br>

## 1. What the trace indicates

| Metric                  | Dominant functions                                                                                                                                                 | Evidence within TraceCompass                                      |
| ----------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------ | ----------------------------------------------------------------- |
| **Call count**          | `pmd_rx_burst` (768 k) → `rte_net_get_ptype` (1 881) → helper chain (`rte_pktmbuf_read`, `rte_constant_bswap16`, `rte_ipv4_hdr_len`, `ptype_l3_ip`, `ptype_l4`, …) | *Descriptive-Statistics* and *Function-Duration-Statistics* views |
| **Self-time**           | `pmd_rx_burst` ≈ 156 ms of 203 ms                                                                                                                                  | Tooltip in the Flame-Graph                                        |
| **Event-density peaks** | Peaks dominated by the above helpers (highlighted in red in the *Call-graph Analysis* table)                                                                       | Event-Density view in combination with the *Statistics* table     |
| **Weighted call tree**  | Six PTYPE helpers account for ≈ 60 % of weighted wall-time                                                                                                         | *Weighted Tree Viewer*                                            |

> ▸ _Red rows in the *Statistics* table correspond to functions highlighted after drawing a region in the Event-Density view; the same helper symbols dominate both, corroborating the conclusion that classification logic is responsible for burst-time spikes._

---

![Statistics2](Pics/Statistics2.png)

<br>
 
## 2. Location of the hot-path code in **DPDK 25.03**

| File                                | Function                                                          | Role in the hot path                                                  |
| ----------------------------------- | ----------------------------------------------------------------- | --------------------------------------------------------------------- |
| **`lib/net/rte_net.c`**             | `uint32_t rte_net_get_ptype(...)`                                 | Linear walk over L2/L3/L4 headers (≈ lines 560 – 850).                |
| `lib/net/rte_ip.h`                  | `rte_ipv4_hdr_len(...)`                                           | Extracts IPv4 IHL on each IPv4 packet.                                |
| `lib/net/rte_byteorder.h`           | `rte_constant_bswap16(...)`                                       | Endian swap used in every UDP/TCP port check.                         |
| `lib/mbuf/rte_mbuf.c`               | `rte_pktmbuf_read(...)`                                           | Safely copies header bytes into cache.                                |
| **`drivers/net/tap/rte_eth_tap.c`** | `tap_trigger_cb()` → `tap_parse_packet()` → `rte_net_get_ptype()` | TAP Rx path; the callback is installed when the rule is accepted.     |
| `drivers/net/tap/tap_flow.c`        | `tap_flow_validate`, `tap_flow_create`                            | Determines that L4 protocol match is required and enables the parser. |

> ▸ _In the opened object address `0x61abdac09a90` within the Symbol-Viewer (part of the *Call-graph Analysis* perspective) TraceCompass resolved the symbol name to **`rte_net_get_ptype`** in **`lib/net/rte_net.c`**, fully confirming the mapping in the table above._

---

![PMD_RX_Burst](Pics/PMD_RX_Burst.png)

<br>
 
## 3. Reasons for the high cost of the rule

1. **TAP is a purely software device**; no hardware RSS or flow-director off-load is available.  
2. **`rte_flow` therefore falls back to software steering**; `tap_trigger_cb()` is invoked for every received frame.  
3. **`rte_net_get_ptype()` is branch-heavy and cache-sensitive**, causing the helper chain to execute on each packet and producing the latency spread visible in the trace.

> ▸ _The *Standard Deviation* column for `rte_net_get_ptype` and its helpers exhibits wide spreads, a hallmark of branch mis-prediction and cache-miss behaviour in a header parser._

---

![Flame-Graph02](Pics/Flame-Graph02.png)

<br>

## 4. Bottleneck ranking

![Flame-Graph01](Pics/Flame-Graph01.png)


| Rank | Dominant cost                                 | Trace symptom                             | Source location                     |
| ---- | --------------------------------------------- | ----------------------------------------- | ----------------------------------- |
| 1    | **`rte_net_get_ptype()`**                     | 15 ms total / 7.8 ms self for 1 881 calls | `lib/net/rte_net.c:≈560-850`        |
| 2    | `rte_pktmbuf_read()` + `rte_pktmbuf_headroom` | 2.36 ms self                              | `lib/mbuf/rte_mbuf.c:≈500-580`      |
| 3    | `rte_constant_bswap16`                        | 2.45 ms self                              | `include/rte_byteorder.h`           |
| 4    | `rte_ipv4_hdr_len`                            | 0.76 ms self                              | `lib/net/rte_ip.h`                  |
| 5    | Mempool get/put bursts                        | Visible spikes when mbuf cache empties    | `lib/mempool/rte_mempool_generic.c` |

---

<br>
 
## 5. Practical mitigation options

| Mitigation                            | Applicable context     | Implementation                                              |
| ------------------------------------- | ---------------------- | ----------------------------------------------------------- |
| Hardware off-load (`rte_flow` in NIC) | Physical NIC available | Intel E810, Mellanox CX-6, etc.                             |
| Disable software PTYPE parsing        | Remain on TAP          | `port config 0 ptype_parse off`; filter in forwarding loop. |
| Vectorised TAP Rx path                | Willing to patch       | Use AVX2/SSE sample in `drivers/net/ixgbe`.                 |
| Remove `-finstrument-functions`       | Performance runs       | Replace with manual UST probes or `perf record`.            |
| Pre-slice traffic in generator        | Synthetic workload     | Send UDP frames to a dedicated TAP.                         |

> ▸ _The capture employed `--subbuf-size=40M`, yet some events were lost during the busiest 30 µs sections, indicating that instrumentation overhead itself is non-negligible._

---

<br>
 
## 6. Hotspot file references

```text
lib/net/rte_net.c             : rte_net_get_ptype()        (≈ 560-850)
lib/net/rte_ip.h              : rte_ipv4_hdr_len()
include/rte_byteorder.h       : rte_constant_bswap16()
lib/mbuf/rte_mbuf.c           : rte_pktmbuf_read()
drivers/net/tap/rte_eth_tap.c : tap_trigger_cb()
drivers/net/tap/tap_flow.c    : software flow helpers
```

---

<br>
 
# Detailed observations for each TraceCompass component

| TraceCompass view              | Observation                                                                        | Analytical implication                                                              |
| ------------------------------ | ---------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------- |
| *Statistics → Event Types*     | Entry/exit events are exactly 50 / 50 %.                                           | Confirms high volume caused by `-finstrument-functions`; helper timing is inflated. |
| *Event Density*                | Saw-tooth pattern of ~44–50 events per burst; valleys align with mbuf allocations. | Indicates classification and occasional mempool refills dominate burst latency.     |
| *Flame Graph*                  | Wide `pkt_burst_io_forward`; underneath, repeating stack of six helpers.           | Helper set is invoked once per frame, characteristic of software parsing.           |
| *Flame Graph tooltip*          | `rte_net_get_ptype` mean ≈ 8 µs; max ≈ 54 µs.                                      | Long tail reflects branch/cold-cache penalties.                                     |
| *Function Duration Statistics* | Helpers: `rte_pktmbuf_read` 422 ns, `rte_constant_bswap16` 391 ns, etc.            | Figures match the ∼400 ns overhead introduced by the rule.                          |
| *Weighted Tree Viewer*         | Six helpers form ≈ 60 % of weighted time.                                          | Parsing confirmed as the principal hotspot.                                         |
| *Call-graph Analysis*          | Selected peaks consist exclusively of helper symbols; no system calls present.     | Processing overhead lies entirely within userspace DPDK code.                       |

---

<br>
 
# Adding PMU contexts

| PMU counter      | Projected change with the flow rule active | Explanation                                                                                                    |
| ---------------- | ------------------------------------------ | -------------------------------------------------------------------------------------------------------------- |
| **cpu-cycles**   | Increase by ~5 – 10 %                     | Additional helper instructions execute per frame.                                                              |
| **instructions** | Increase by ~3 – 5 %                       | Header checks add 50 – 100 instructions per packet.                                                           |
| **cache-misses** | Noticeable rise on the Rx core             | `rte_pktmbuf_read` touches payload across cache-line boundaries, incurring extra L1 and occasional LLC misses. |

*Predicted secondary effects*

* IPC (instructions / cycle) remains ≈ 1 because the helpers are short and branch-dominated.  
* LLC-miss percentage rises modestly; most misses are served after the first line fill.  
* The perf sample set should validate the time-domain analysis by showing higher retired instructions and cache-miss stalls exclusively on the userspace threads.

<br>

<br>
 
## Test Scenario: Two Queues, Dropping TCP, UDP, or Both Packet Types

In this test, we evaluate the impact of selectively dropping certain types of packets.
Initially, only UDP packets were dropped, followed by the dropping of both TCP and UDP packets.


We noticed that regardless of the flow rule, the `burst_forward` function is always called.  
This is expected since the driver operates in poll mode — the CPU is always active, and the specified core maintains 100% utilization.

![Flame-Graph03](Pics/Flame-Graph03.png)

If there is data to transmit, the call stack changes to:

![Flame-Graph04](Pics/Flame-Graph04.png)

Upon analyzing the function calls, we did not observe any functions explicitly responsible for filtering or dropping packets.
This observation appears unusual; therefore, we will conduct a more in-depth investigation.


**Note:** `LTTng` captures function calls; however, it does not trace internal control flow structures such as `if` or `else` branches.


Let us examine the DPDK source code for further clarity.
In [`tap_flow.c`](https://github.com/DPDK/dpdk/blob/main/drivers/net/tap/tap_flow.c) at line 1065, if the flow action is configured to drop, the implementation modifies the packet data in such a way that the kernel subsequently discards it.
As a result, no user-space function is explicitly invoked, which explains why `LTTng` is unable to trace this behavior.


![Code-Review1](Pics/Code-Review1.png)

The same principle applies to queue redirection for specific packet types:
When a flow rule redirects packets to a particular queue, the necessary configuration is handled at a lower level—typically by modifying kernel-level or driver-level settings. As such, no explicit user-space function is called during the redirection process, and consequently, tools like `LTTng` are unable to capture this action.

![Code-Review2](Pics/Code-Review2.png)

It simply modifies the socket buffer (SKB) directly, without invoking any additional function calls.

<br>
 
## Conclusion

With the current setup, tracing the actual packet filtering logic through function calls is not feasible, as the operations occur at a lower level without explicit invocation of user-space functions.
However, it is still possible to gain valuable insights by analyzing low-level performance metrics—such as CPU cycles, cache misses, branch mispredictions, and memory access patterns.
These metrics can help us better understand the performance characteristics and efficiency of the filtering mechanism, even in the absence of direct function call traces.
