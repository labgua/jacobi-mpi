# bash

date >> benchmark.dat

echo "Sequencial Benchmark"
echo "Sequencial Benchmark" >> benchmark.dat

./JacobiSeq 1202  data/jacobi_1202x1202.dat >> benchmark.dat
./JacobiSeq 2402  data/jacobi_2402x2402.dat >> benchmark.dat
./JacobiSeq 3602  data/jacobi_3602x3602.dat >> benchmark.dat
./JacobiSeq 4802  data/jacobi_4802x4802.dat >> benchmark.dat
./JacobiSeq 6002  data/jacobi_6002x6002.dat >> benchmark.dat
./JacobiSeq 7202  data/jacobi_7202x7202.dat >> benchmark.dat
./JacobiSeq 8402  data/jacobi_8402x8402.dat >> benchmark.dat
./JacobiSeq 9602  data/jacobi_9602x9602.dat >> benchmark.dat

echo "----------------------" >> benchmark.dat