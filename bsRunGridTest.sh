for run in {1..4}
	do
	mpirun -hostfile lochost$run mpitest 500.txt
	done
