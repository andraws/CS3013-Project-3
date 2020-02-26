all: problem

problem: problem.c
	gcc -o problem problem.c -pthread -lm

clean:
	rm -f problem
