#!/bin/bash
export OMP_NUM_THREADS=4
time ./g_omp $1 $2 $3
