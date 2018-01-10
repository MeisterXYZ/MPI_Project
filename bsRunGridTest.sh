for run in {1..10}
	do
	let res=$run*1+62050;
	mpirun gridtest2 $res
	done
