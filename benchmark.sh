#!/bin/bash

echo "Start Benchmark!"

## start benchmark
chmod +x seq_benchmark.sh
chmod +x par_benchmark.sh

echo ">>>>>> START BENCHMARK"

./seq_benchmark.sh

./par_benchmark.sh