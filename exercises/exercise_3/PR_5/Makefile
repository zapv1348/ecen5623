all: EX5.exe
 
# $@ matches the target; $< matches the first dependent
EX5.exe: EX5.o
	gcc -o $@ $< -pthread -lrt

EX5.o: EX5.c
	gcc -c $< -pthread -lrt
     
clean:
	rm EX5.o EX5.exe
