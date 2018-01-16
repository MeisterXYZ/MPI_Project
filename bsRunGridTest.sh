for run in {1..4}
	do
	mpirun -hostfile lochost$ru pixCheckTangle 500.txt
	done
