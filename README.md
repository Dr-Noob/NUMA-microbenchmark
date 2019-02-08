# NUMA-microbenchmark

Minimal benchmark to show NUMA effects

### NUMA short explanation

On NUMA system, in "first touch" policy, a page is mapped to the node which contains
the core that first touches this page. Moreover, if cores accesses memory mapped at other NUMA node,
they will suffer lower bandwith and higher latency.
This benchmark tries to show this behaviour (can test two NUMA nodes only).

### How it works
At the beggining, there are two defines:

```
#define CORE_NODE_0
#define CORE_NODE_1
```

Which tells one core which is on NUMA node 0, and other core which is on NUMA node 1. Thus, this test will make
both cores to allocate one array(x1, x2), so x1 will be mapped to node 0 and x2 will be mapped to node 1. By default,
each core will be doing operations on self mapped array, but one can change this and test what happens. Access to arrays are
made somehow unpredictable by using some prime number, therefore, making hardware prefetcher useless. If you do not
try to hide hardware prefetching, NUMA effects may not be visible at all.

### Compiling
You will need libnuma and OpenMP. Simply compile using ``make``

### Execution examples
Here you can check the results from running this microbenchmark in a Intel Xeon E5-2698 v4, which is a
2 NUMA node CPU. First node's CPU is 0, while second node's CPU is 20, so:

```
#define CORE_NODE_0  0
#define CORE_NODE_1  20
```

So, x1 is at NUMA node 0, along with core 0. x2 is at node 1, with core 20. If core 0 accesses x1 and core 1
accesses x2, we obtain:

```
NUMA node 0: 11111111111111111111000000000000000000001111111111111111111100000000000000000000 - 65425 MB
NUMA node 1: 00000000000000000000111111111111111111110000000000000000000011111111111111111111 - 65536 MB
Core 0: 8.791s
Core 20: 8.560s
```

If we make core 20 access x1 too, we obtain:

```
NUMA node 0: 11111111111111111111000000000000000000001111111111111111111100000000000000000000 - 65425 MB
NUMA node 1: 00000000000000000000111111111111111111110000000000000000000011111111111111111111 - 65536 MB
Core 20: 13.205s
Core 0: 8.662s
```

We can see core 20 is penalized because accesing NUMA node 0's memory. If we also make core 0 access x2, making
both cores accesing each others memory, we have:

```
NUMA node 0: 11111111111111111111000000000000000000001111111111111111111100000000000000000000 - 65425 MB
NUMA node 1: 00000000000000000000111111111111111111110000000000000000000011111111111111111111 - 65536 MB
Core 20: 13.185s
Core 0: 13.216s
```

__NOTE__: Running this on a non NUMA system should work too, showing same time access by both cores
