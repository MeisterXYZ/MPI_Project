for run in {1..50}
	do
	mpirun -hostfile myhostfile$run pixCheckTangle grid1.txt
	done
