for run in {1..2}
	do
	mpirun -hostfile lochost$run scriptForMeasureVariation 500.txt
	done
