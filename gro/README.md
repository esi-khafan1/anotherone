# DPDK Generic Receive Offload (GRO)
This document outlines the architecture and operation of the Generic Receive Offload (GRO) library within the Data Plane Development Kit (DPDK).

## Architecture

Generic Receive Offload files are on `<dpdk-folder>/lib/gro/` directory. `rte_gro` is responsible for reading packets in bursts, containing 512 packets of different protocols at most then it will pass the packets with the same type to relative gro_ptype file to be merged. GRO on DPDK supports different packet types: **TCP4**, **TCP6**, **UDP4**, **VXLAN_TCP4**, **VXLAN_UDP4**

### 1. Naming Convention
- each packet is called an item in source code (both merged and unmerged).
- common key of an item is a tuple of (source/destination IP, source/destination port, protocol)
- each flow contains items with the same common key
- two items with same common key with consecutive sequence numbers are called neighbors
- each reassembly table contains flows of the same protocol

### 2. Parameters
GRO has 3 modes based on its [Documentation](https://doc.dpdk.org/dts-20.02/test_plans/dpdk_gro_lib_test_plan.html#test-case1-dpdk-gro-lightmode-test-with-tcp-ipv4-traffic). Different modes can be set via flush parameter in testpmd interactive mode. `rte_gro_timeout_flush` sends the packet to upper layer when timeout occurs after flush parameter value of receive burst calls. it will also update the header if the packet is merged. If any merged packet reaches maximum size of its type (like 64KB for TCP4) then it is sent to upper layer

### 3. Merge Workflow
General reassembly table of `rte_gro` passes different packet types to protocol-specific reassembly tables like TCP where merging is handled. The main responsibilites of reassembly tables' associated functions are:
- **Check TCP sequence numbers** to ensure packets are contiguous.
- **Compare the TCP 5-tuple** (source/destination IP, source/destination port, protocol) for flow matching.
- **Handle TCP flags** like FIN and PUSH correctly, which often signal the end of a merge opportunity.

# Instrumentation
Trace points can be put inside function bodies of both header and C files that their name start with `rte_`.These trace points may emit the input parameters of functions or extra arguments in `gro_trace.h` and each of their names should be regestered in `gro_trace_points.c`, determining dumped trace point name. As `rte_gro.h` functions do not contain bodies, we shall use normal instrumentation instead of fast path. It is neccessary to add file names to `meson.build` located in `lib/gro/`. Changed and new files are accessible via gro folder of this repo.

# Setup

We use dpdk-testpmd to forward packets between two virtual ethernet (veth) ports and experiment with various offloading options of dpdk including gro.

Two namespaces and two veth interfaces are created, with one end of each veth interface in each of the namespaces and the other end in the host side. The host end of veth interfaces is connected to dpdk-testpmd and traffic is sent from one namespace to the other using iperf3.

**Create Two veth Interfaces**
```shell
sudo ip netns del ns1
sudo ip link add veth-ns1 type veth peer name veth-host1
sudo ip addr add 10.1.1.2/24 dev veth-host1 
sudo ip link set veth-host1 up

sudo ip netns del ns2
sudo ip link add veth-ns2 type veth peer name veth-host2
sudo ip addr add 10.1.1.4/24 dev veth-host2 
sudo ip link set veth-host2 up
```

**Create and Configure Namespace ns1**
```shell
sudo ip netns add ns1
sudo ip link set veth-ns1 netns ns1
sudo ip netns exec ns1 ip link set veth-ns1 up
sudo ip netns exec ns1 ifconfig veth-ns1 10.1.1.1/24 up
```

**Create and Configure Namespace ns2**
```shell
sudo ip netns add ns2
sudo ip link set veth-ns2 netns ns2
sudo ip netns exec ns2 ip link set veth-ns2 up
sudo ip netns exec ns2 ifconfig veth-ns2 10.1.1.3/24 up
```

**Turn off tx/rx offloads on the kernel driver:**
Some offload features on the kernel side mess up testpmd when forwarding tcp traffic. Turn them off:

```shell
sudo ip netns exec ns1 ethtool -K veth-ns1 tso on
sudo ethtool -K veth-host2 gro off
sudo ethtool -K veth-host1 tx off rx off
sudo ethtool -K veth-host2 tx off rx off
sudo ip netns exec ns1 ethtool -K veth-ns1 tx off rx off
sudo ip netns exec ns2 ethtool -K veth-ns2 tx off rx off
```

**Meson Configuration of Build Environment**
```shell
meson setup build \
-Dexamples=all \
-Dlibdir=lib \
-Denable_trace_fp=true \
-Dc_args="-finstrument-functions"
```

**Ninja for Build and Install**
```shell
cd build
ninja
sudo meson install
sudo ldconfig
```

**Hugepages Configuration**
```shell
sudo sysctl -w vm.nr_hugepages=1024
mount -t hugetlbfs none /dev/hugepages
```


**Run dpdk-testpmd**
```shell
sudo dpdk-testpmd -l 0-3 -n4 --vdev 'eth_af_packet0,iface=veth-host1' --vdev 'eth_af_packet1,iface=veth-host2' --trace=lib.gro.* --trace-mode=overwrite --trace-bufsz=20M -- -i --forward-mode=io
```

**Interactive configure dpdk-testpmd**
```shell
stop
port stop 0
port stop 1
set port 0 gro on
set port 1 gro on
set gro flush 1
port start 0
port start 1
start
```

**Generate Traffic**

On namespace ns2 run the server side
```shell
sudo ip netns exec ns2 iperf3 -s -B 10.1.1.3
```

On namespace ns1 run the client side
```shell
sudo ip netns exec ns1 iperf3 -c 10.1.1.3 -t 0
```

# Optimization

# Analysis
