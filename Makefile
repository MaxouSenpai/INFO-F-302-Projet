G++ = g++-8
FLAGS=-Wall -g 

all: rush 

rush: rush.o Solver.o
	$(G++) $(FLAGS) *.o -o rush

Solver.o: Solver.cpp Solver.hpp SolverTypes.hpp Global.hpp Heap.hpp VarOrder.hpp
	$(G++) $(FLAGS) -c Solver.cpp 

rush.o: Main.cpp  Solver.hpp 
	$(G++) $(FLAGS) -c Main.cpp  -o rush.o

clean:
	$(RM) *.o rush
