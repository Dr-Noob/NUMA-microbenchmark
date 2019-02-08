#define _GNU_SOURCE
#include <numa.h>
#include <stdio.h>
#include <pthread.h>
#include <omp.h>
#include <assert.h>
#include <sys/time.h>

#define CORE_NODE_0  0
#define CORE_NODE_1  7
#define PRIME_NUMBER 3623

void pin_to_core(size_t core) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core, &cpuset);
  pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

void print_bitmask(const struct bitmask *bm) {
  for(size_t i=0; i<bm->size; ++i)
    printf("%d", numa_bitmask_isbitset(bm, i));
}

int main() {
  assert(numa_available() == 0);
  int num_cpus = numa_num_task_cpus();
  int nn = numa_max_node();
  numa_set_localalloc();

  struct bitmask *bm[nn+1];
  for (int i=0; i <= nn; i++) {
    bm[i] = numa_bitmask_alloc(num_cpus);
    numa_node_to_cpus(i, bm[i]);
    printf("NUMA node %d: ", i);
    print_bitmask(bm[i]);
    printf(" - %li MB\n", numa_node_size(i, 0) / (1024*1024));
  }

  char *x1;
  char *x2;
  long int array_size = 1<<28; /* 256 MB */

  #pragma omp parallel firstprivate(array_size)
  {
    struct timeval t1, t2;
    long int tid = omp_get_thread_num();
    long int numthreads = omp_get_num_threads();

    pin_to_core(tid);
    if(tid == CORE_NODE_0) {
      x1 = numa_alloc_local(array_size);
      for(long int i=0;i<array_size;i+=4096) x1[i] = 0;
    }
    if(tid == CORE_NODE_1) {
      x2 = numa_alloc_local(array_size);
      for(long int i=0;i<array_size;i+=4096) x2[i] = 0;
    }

    #pragma omp barrier

    #pragma omp critical
    {
      if(tid == CORE_NODE_0) {
        gettimeofday(&t1, NULL);

        for (long int i=0; i<array_size; i++) {
          x1[(i * PRIME_NUMBER) % array_size] += i;
        }

        gettimeofday(&t2, NULL);
        double e_time = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
        printf("Core %d: %.3fs\n",CORE_NODE_0, e_time);
      }
      if(tid == CORE_NODE_1) {
        gettimeofday(&t1, NULL);

        for (long int i=0; i<array_size; i++) {
          x2[(i * PRIME_NUMBER) % array_size] += i;
       }

       gettimeofday(&t2, NULL);
       double e_time = (t2.tv_sec - t1.tv_sec) + ((t2.tv_usec - t1.tv_usec)/1000000.0);
       printf("Core %d: %.3fs\n",CORE_NODE_1, e_time);
      }

    }

    #pragma omp barrier
    if(tid == CORE_NODE_0)numa_free(x1, array_size);
    if(tid == CORE_NODE_1)numa_free(x2, array_size);
  }

}
