# bash

date >> benchmark.dat 

#loading list of private ip and making the hostfiles
. ip_private_list.array

for (( i=1; i<=${#ip_private_list[@]}; i++ ))
do
	for each in "${ip_private_list[@]:0:$i}"
	do
	  echo $each >> "myhostfile_$i"
	done
done

#start banchmark

echo "Strong Scaling"
echo "Strong Scaling" >> benchmark.dat

#for (( i=1; i<=${#ip_private_list[@]}; i++ ))
#do
#	mpirun -np "$((i*2))" --hostfile "myhostfile_$i" JacobiPar 4802 data/jacobi_12802x12802.dat >> benchmark.dat
#done

mpirun -np 2  --hostfile myhostfile_1 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 4  --hostfile myhostfile_2 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 6  --hostfile myhostfile_3 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 8  --hostfile myhostfile_4 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 10 --hostfile myhostfile_5 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 12 --hostfile myhostfile_6 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 14 --hostfile myhostfile_7 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat
mpirun -np 16 --hostfile myhostfile_8 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat


echo "Weak Scaling"
echo "Weak Scaling" >> benchmark.dat

mpirun -np 2  --hostfile myhostfile_1 JacobiPar 1202  data/jacobi_1202x1202.dat >> benchmark.dat
mpirun -np 4  --hostfile myhostfile_2 JacobiPar 2402  data/jacobi_2402x2402.dat >> benchmark.dat
mpirun -np 6  --hostfile myhostfile_3 JacobiPar 3602  data/jacobi_3602x3602.dat >> benchmark.dat
mpirun -np 8  --hostfile myhostfile_4 JacobiPar 4802  data/jacobi_4802x4802.dat >> benchmark.dat
mpirun -np 10 --hostfile myhostfile_5 JacobiPar 6002  data/jacobi_6002x6002.dat >> benchmark.dat
mpirun -np 12 --hostfile myhostfile_6 JacobiPar 7202  data/jacobi_7202x7202.dat >> benchmark.dat
mpirun -np 14 --hostfile myhostfile_7 JacobiPar 8402  data/jacobi_8402x8402.dat >> benchmark.dat
mpirun -np 16 --hostfile myhostfile_8 JacobiPar 9602  data/jacobi_9602x9602.dat >> benchmark.dat

echo "----------------------" >> benchmark.dat