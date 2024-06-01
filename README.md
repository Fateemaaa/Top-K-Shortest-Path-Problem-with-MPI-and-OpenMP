# Top-K-Shortest-Path-Problem-with-MPI-and-OpenMP
Parallel and Distributed Computing 
 # Graph Pathfinding with MPI and OpenMP

## Project Description
This project implements a parallel algorithm to find the k-shortest paths in a network graph. It utilizes MPI for distributed processing across multiple nodes and OpenMP for multi-threading within each node.
## Features
- **MPI Integration**: Distributes tasks across multiple computing nodes to handle large graphs efficiently.
- **OpenMP Optimization**: Utilizes multi-threading within each node to accelerate the pathfinding process.
- **Dynamic Load Balancing**: Implements strategies to evenly distribute work among all processors to minimize idle time and maximize resource utilization.
- **Robust Error Handling**: Includes comprehensive error checking to ensure stability and reliability during execution.

## Prerequisites
To run this project, you'll need:
- An MPI implementation (e.g., MPICH, OpenMPI)
- A C++ compiler with OpenMP support (e.g., GCC)

How to Compile: 
for c++
mpic++ -fopenmp [filename.cpp] -o [executable file] -lstdc++ 

mpirun -np [no.ofprocessess] ./[executable file]

for c: 
mpicc [filename.c] -o [executable file]
./[executable file]
