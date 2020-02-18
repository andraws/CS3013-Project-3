all: problem

problem: problem.c
	gcc -o problem problem.c

clean:
	rm -f problem
