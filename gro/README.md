# Playing with DPDK Global Receive Offload

## Setup

We use dpdk-testpmd to forward packets between two virtual ethernet (veth) ports and experiment with various offloading options of dpdk including gro. 

Two namespaces and two veth interfaces are created, with one end of each veth interface in each of the namespaces and the other end in the host side. The host end of veth interfaces is connected to dpdk-testpmd and traffic is sent from one namespace to the other using iperf3. 

**Create Two veth Interfaces**
```shell
sudo ip link add veth-ns1 type veth peer name veth-host1
sudo ip addr add 10.1.1.2/24 dev veth-host1 
sudo ip link set veth-host1 up

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

**Run and Configure dpdk-testpmd**
```shell
sudo dpdk-testpmd -l 0-3 -n4 --vdev 'eth_af_packet0,iface=veth-host1' --vdev 'eth_af_packet1,iface=veth-host2' -- -i --forward-mode=io

testpmd> set port 0 gro on
testpmd> set port 1 gro on
testpmd> set gro flush 1
testpmd> start
```

**Generate Traffic**
```shell
# On namespace ns1 run the client side
sudo ip netns exec ns1 iperf3 -c 10.1.1.3 -t 0

# On namespace ns2 run the server side
sudo ip netns exec ns2 iperf3 -s -B 10.1.1.3
```

## Common Issues

**Turn off tx/rx offloads on the kernel driver**
Some offload features on the kernel side mess up testpmd when forwarding tcp traffic. Turn them off:

```shell
sudo ethtool -K veth-host1 tx off rx off
sudo ethtool -K veth-host2 tx off rx off
sudo ip netns exec ns1 ethtool -K veth-ns1 tx off rx off
sudo ip netns exec ns2 ethtool -K veth-ns2 tx off rx off
```

**Disable reverse path filtering rp_filter**
