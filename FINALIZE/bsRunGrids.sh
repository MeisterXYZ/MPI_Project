for run in {1..50}
	do
	mpirun -hostfile myhostfile$run pixCheckTangle 1000AB.txt
	done
