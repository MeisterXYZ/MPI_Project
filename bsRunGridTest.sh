for run in {1..2}
	do
	mpirun -hostfile lochost$run pixCheckTangle 500.txt
	done
