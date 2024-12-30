threader: threaded_sum.o
	g++ -o threader threaded_sum.o -pthread

threaded_sum.o: threaded_sum.c
	g++ -c -Wall -pthread threaded_sum.c

clean:
	rm *.o threader