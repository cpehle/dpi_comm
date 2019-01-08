run:
	irun -Wcxx,"-std=c++11" test.sv test.cpp

test:
	python test.py 7337 send &
	sleep 1s
	python test.py 7338 recv
