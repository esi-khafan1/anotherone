# Dpdk-TAP-Driver

This repository contains dpdk TAP driver and test-pmd application analysis with lttng tool and trace compass gui.

## System Specifications

The project was executed on the following system:

```bash
Architecture: x86_64
CPU op-mode(s): 32-bit, 64-bit
Byte Order: Little Endian
Address sizes: 39 bits physical, 48 bits virtual
CPU(s): 8
On-line CPU(s) list: 0-7
Thread(s) per core: 2
Core(s) per socket: 4
Socket(s): 1
NUMA node(s): 1
Vendor ID: GenuineIntel
Model name: Intel(R) Core(TM) i7-1065G7 CPU @ 1.30GHz
CPU min MHz: 400.000
CPU max MHz: 3900.0000
...
```

## Core Isolation

IRQs (both software and hardware) may cause context switches, degrading dpdk aplication performance. This Effect can be decreasesd by defining the rules for each CPU core but not fully deleted as dpdk does not fully bypass kernel.

```bash
sudo nano /etc/default/grub
```

Grub configuration file values should be changed to assign differnet jobs to different cores.

```bash
GRUB_CMDLINE_LINUX_DEFAULT="quiet splash isolcpus=4,5 nohz_full=4,5 rcu_nocbs=4,5 irqaffinity=0,1,2,3,6,7"
```

Value of `GRUB_CMDLINE_LINUX_DEFAULT` variable changes:

`isolcpus=4,5` isolates CPUs 4 and 5 from the general Linux scheduler.

`nohz_full=4,5` enables full tickless mode on CPUs 4 and 5.

`rcu_nocbs=4,5` offloads RCU (Read-Copy-Update) callbacks from CPUs 4 and 5.

`irqaffinity=0,1,2,3,6,7` restricts hardware interrupt handling to CPUs 0-3 and 6-7.

```bash
sudo update-grub
sudo reboot
```

`sudo update-grub` updates the grub bootloader configuration file and regenarates `/boot/grub/grub.cfg` file. Changes will finally take place after `sudo reboot` that can be assured by `dmesg | grep isolcpus`.

# Tracing Analysis - UDP Filtering

13 milion Events captured with 887 nanosecond average standard deviation. Optimization rule increased events captured by 11.0% and decreased average of standard deviation by 88.6%.

## Alternative Density Shift

Event density drops to zero when context switch happens for lcore 5 without regrads to the function that is being called, it should wait for another switch to load dpdk-worker5 application variables. It is worthy to point out pmd_rx_burst helper chain contains more events against pmd_tx_bursts' that will cause sigmoid dense shift while their call stack is conversed.

> ▸ _As specified OS uses CFS (Completely Fair Scheduling) that works based on priority scheduling, it is more probable for some threads that are being ran with lower priority will experince more context switches (the reason of high standard deviation for several functions or huge self time/duration in flame graph) for instance: `rte_net_get_ptype`, `pmd_rx_burst`, `pmd_tx_burst`_

---

![image1](Pics/image1.png)


<br>

## Hot Code Paths

| Function                        | ratio | Responsibility                           |
| ------------------------------- |-------| ---------------------------------------- |
| rte_net_get_ptype               | 52.8% | Determine packet type by parsing headers |
| rte_ethdev_trace_rx_burst_empty | 60.0% | Trace an empty RX burst (0 packets)      |
| pmd_rx_burst                    | 97.7% | Poll RX queue and fetch received packets |

> ▸ _These paths can be found via functions with two layers upper towards `tap_trigger_cb` (can be called at last layer of call stack because of its event driven nature) that have bigger ratio of self time over duration than others._

---

![image8](Pics/image8.png)

<br>

## Practical mitigation options

| Mitigation                     | Applicable Context        | Implementation                                     |
| ------------------------------ | ------------------------- | -------------------------------------------------- |
| Process priority surge         | networking environments   | nice() or setpriority() to change process priority |
| CPU core isolation             | deterministic performance | At boot time, isolate core via kernel parameters   |
| Pre-slice traffic in generator | Synthetic workload        | Send UDP frames to a dedicated TAP.                |

> ▸ _The fourth core is dedicated for running app and 5 to 7 are in charge of forwarding packets, but only core 5 was used.This can be increased as described in [Intel Manual](https://www.bing.com/ck/a?!&&p=133bafcb2e934382baeacdda3640a23fc7aff520198bc1d12ebcd43320968603JmltdHM9MTc1MzkyMDAwMA&ptn=3&ver=2&hsh=4&fclid=1272d906-47db-60ab-2561-cd0446d26150&psq=dpdk+bad+througput&u=a1aHR0cHM6Ly93d3cuaW50ZWwuY29tL2NvbnRlbnQvZGFtL2RldmVsb3AvZXh0ZXJuYWwvdXMvZW4vZG9jdW1lbnRzL3Rlc3RpbmctZHBkay1wZXJmb3JtYW5jZS1hbmQtZmVhdHVyZXMtd2l0aC10ZXN0cG1kLTcyMTA5MC5wZGY&ntb=1)._

---

<br>

## Context Switch

Another trace was done with both kernel and userspace involved to prove the idea that was discussed earlier.

> ▸ _interrupt and followed context switches of core 5 causes delay in dpdk-worker5 functions and causes the thread to wait for CPU cycles._

---

![image5](Pics/image5.png)

<br>

## Throughput

Each `tap_trigger_cb` means a frame is received  from NIC (tap driver) causing `rte_pktmbuf_alloc`, `rte_net_get_ptype` subroutine activation per each frame. The worst case happens when tap trigger is called on dpdk/worker5 directly.

> ▸ _Processing 8 packets took 176.3 microseconds (12 nanoseconds of wasted time between `pkt_burst_io_forward` calls is negligible) concluding application is capable of receiving and sending 5672 packets per second. Assuming mtu is set to 1500 as default the maximum forwarding would be 8.5 gigabyte per second. VPP makes great difference becuase of linear rate packet processing (larger packets mean higher throughput)._

![image11](Pics/image11.png)

<br>

## Receiving Helper Chain

`pmd_rx_burst` is called whenever `tap_trigger_cb` interrupt occurs. Within one burst there could be different calls of `rte_pktmbuf_alloc` with `rte_net_get_ptype` followed which matches the number of frames received on tap driver. 

> ▸ _Flame chart includes `rte_constant_bswap16` calls inside ptype function but source code does not include this function. Actually they are called via `rte_cpu_to_be16`. This routine is neccessary becuase CPU byte order (little Endian as specified) differs from Network byte order._

---

![image9](Pics/image9.png)

<br>

## Challenges and Solutions

### 1. Missing Meson Module

* **Issue**: Encountering No module named 'mesonbuild.machinefile' in running `meson install` after `ninja` command.

* **Solution**: uninstall all meson versions that are installed with pip or apt and their relative libraries in usr/ folder, then install it with root priviledges (not as a superuser) by pip.
  
  ```bash
  sudo pip install meson
  ```

* **Outcome**: meson successfully installs the compiled artifacts.

### 2. Empty Trace Folder

* **Issue**: After running lttng script, the ust folder of trace is empty.
* **Solution**: Clean install lttng stable ppa version with [LTTng Stable 2.13 PPA installation Guide](https://lttng.org/docs/v2.13/#doc-ubuntu-ppa) 
* **Outcome**: Trace has captured preloaded fuctions.

### 3. Null Trace Compass Views

* **Issue**: Call stack views ( Flame Graph & Flame Graph Selection) are empty.

* **Solution**: install java-21 and set as deafult version of java.
  
  ```bash
  sudo apt install openjdk-21-jdk -y
  ```

* **Outcome**: Call stack gets visible (still needs sometime based on host ram and CPU).

### 4. Incomplete Symbol Mapping

* **Issue**: some functions are available only by their address not their names.
* **Solution**: click on symbol mapping button shown below (available for flame chart, flame graph, ...) then check `Use custom target root directory` without choosing any directory and press ok.

  ![image7](Pics/image7.png)

* **Outcome**: Function names  will be loaded completely for each window that is opened afterwards.
