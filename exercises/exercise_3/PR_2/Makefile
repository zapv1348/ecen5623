all: EX3_Q2.exe
 
# $@ matches the target; $< matches the first dependent
EX3_Q2.exe: EX3_Q2.o
	gcc -o $@ $< -pthread -lrt

EX3_Q2.o: EX3_Q2.c
	gcc -c $< -pthread -lrt
     
clean:
	rm EX3_Q2.o EX3_Q2.exe
