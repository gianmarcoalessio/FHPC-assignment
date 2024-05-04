# Exercise1
## introduction

Game of Life brief description, the cells can be “dead” or “alive” depending on the conditions of the neighbouring cells. If the cell has 2 to 3 neighbours the cell is alive, if not is dead. 

I implement: 
- static evolution: at each state the system is frozen and the evolution of cells to state si+1 is based only on the conditions of the previous state.
- ordered evolution: it’s completely serial, the evolution starts from cell (0,0) and moves forward by lines in order (next cell to evolve would be (0,1) and so on...). Changes are immediate and affect the evolution of the next cells.


## Methodology 

- Functional decomposition is not well suited for this problem, because essentially there is only one function that is called in a loop (either for the ordered and static methods).
- Playground distributed among MPI tasks (is a domain decomposition): store different chunks of the playground on different nodes:
  - pros: optimization under the scaling of the playground
  - reason: beacuse the cells are aware only of their neighbours
- Row-wise domain decomposition assuming that the neighbors are mostly in the same row of the considered cell. (what simo did)
- The choice of using a matrix (in order to rapresent pgm files) of unsigned char has been made to reduce the usage of RAM

To evaluate the performances I have run multiple test on the Epyc and Thin nodes of the orfeo cluster changing properly the number of **MPI processes** and **OpenMP threads**. In this section I am mainly interested in the **speed up** described by the formula below:

$$
speedup = \frac{t_{p}}{t_{np}}
$$

Where $t_{p}$ is the time taken by a single process and $t_{np}$ is the time taken by n processes (note that the ideal speedup is given by $speedup = n$($n$is the number of processes))


## Implementation


## MPI weak scalability
to do tomorrow night to get the data:
in order to understand better the code I have to run the static for tomorrow and finish mpi_weak_scaling. Completely. 

Monday night mpi_weak_scaling for thin: 
In this section the main idea is to mantain the workload (execution time) for each MPI Task constant.
MPI_Tasks = [1,2,3,4,5,6] = number of processes
Matrix_size = [10000,14143, 17321, 20000, 22361, 24495] 
Number of processes vs Mean Time 
Only with static evolution (find why?)

Simone:
For this study I fixed the number of OpenMP threads to 20 per socket and mapped the MPI processes by socket, I used 3 nodes for both Epyc and Thin.

Filippo:
using respectively 1 OpenMP thread and 32 OpenMP threads to show the time difference between the two configurations.



## Results & Discussion

## Conlcusion 





# Static evolution

NEED TO DO IT GOOD:
- OMP scalability Speedup (range) vs threads 
  - 2 cpus (socket) per node
  - I want more sockets I need to have more nodes, in that case I use `--map-by node --bind-to socket`
  - Fixed one MPI process per socket (So i measure the OMP threads scalability)
  - size playground: 25000 (personal choice)
  - number of iterations (n): 50 (personal choice)
  - Each OMP threads runs in 1 core
  - EPYC NODE (64 cores per socket it's the maximum number of threads)
  - THIN NODE (12 cores per socket it's the maximum number of threads)

NEED TO DO IT GOOD:
- MPI Strong scalability - Speedup vs MPI processes 
  - Fixed the number of OMP threads to 1 per socket (So i measure the MPI scalability)
  - Mapped the MPI tasks on cores
  - Allocate 2 Nodes
  - EPYC Speedup (range 50-100s) vs MPI processes (8 to 256 MPI processes, intervall: 8):
    - Did it for different size of the playground and different number of iterations (n):
      - size = 15000, n=50
      - size = 25000, n=50
      - size = 35000, n=50
      - size = 25000, n=100
      - size = 45000, n=100
  - THIN Speedup vs MPI processes (8 to 48 MPI processes, intervall: 2)
    - size = 25000, n=50

CAN DO IT FOR LAST:
- MPI Weak scalability - Mean time vs MPI processes  == is not even required 
    - Fixed number of OMP threads to 20 per socket (So i measure the MPI scalability) = can change but find a reason 
    - Mapped MPI processes by socket == don't know if I can change and why
    - Allocate 3 Nodes == can change beacuse we have 8 epyc nodes and 12 thin nodes that i can use, we don't have any indication on how many nodes
    - Different size of the playground (Matrix_size = [10000,14143, 17321, 20000, 22361, 24495]) == can't change
    - For 6 processes (1,2,3,4,5,6) == can't change depending on the different number of matrix size

