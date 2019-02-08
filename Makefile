CCX=gcc
CXX_FLAGS=-O3 -fopenmp -std=c99
SANITY_FLAGS=-Wall -Wextra -Werror -fstack-protector-all -pedantic -Wno-unused -Wfloat-equal -Wshadow -Wpointer-arith -Wstrict-overflow=5 -Wformat=2

numa_bench: numa_bench.c Makefile
	$(CCX) $(SANITY_FLAGS) $(CXX_FLAGS) numa_bench.c -lnuma -o numa_bench

clean:
	rm numa_aware no_numa_aware
