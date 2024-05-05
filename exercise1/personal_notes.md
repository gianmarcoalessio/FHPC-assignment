# Bash command used:

- Compile serial.c:
gcc -o serial.o serial.c

- Initialize serial.o with an image named starting.pgm of 100x100 pixels:
./serial.o -i -k 100 -f starting.pgm

- Run the simulation for ordered evolution for 1000 iterations:
./serial.o -r -n 1000 -e 0 -f starting.pgm

- Run the simulation for static evolution for 1000 iterations:
./serial.o -r -n 1000 -e 1 -f starting.pgm

```sh
salloc -n1 -N1 -p THIN --time=0:15:00
module load architecture/Intel

module load openMPI/4.1.5/gnu/12.2.1 

srun mpicc --march=native hello_world.c -o hello_world.x
mpirun -np 4 ./hello_world.x #4 processes


```c

MPI_send( buffer, count, data_type, destination, tag, communicator)
MPI_recv( buffer, count, data_type, source,  tag, communicator, status)

```

BUFFER: data to send
COUNT: number of elements in buffer . 
DATA_TYPE: which kind of data types in buffer ?  
DESTINATION: the receiver
TAG: the label of the message 
COMMUNICATOR: set of processors involved
SOURCE: is the sender, can be set as MPI_any_source (receive a message from any processor within the  communicator )
TAG: the label of message ( can be set as MPI_any_tag: receive any kind of message )
STATUS: integer array with information on message in  case of error

THIS IS A DEADLOCK EXAMPLE

```c
if( rank == 0 ) {
 Call MPI_send( buffer1, 1, MPI_integer, 1, 10, &MPI_comm_world, error )
 Call MPI_recv( buffer2, 1, MPI_integer, 1, 20, &MPI_comm_world, status, error )  
}
else if( rank == 1 ) {
 Call MPI_send( buffer2, 1, MPI_integer, 0, 20, &MPI_comm_world, error )
 Call MPI_recv( buffer1, 1, MPI_integer, 0, 10, &MPI_comm_world, status, error )
}
```

Devo immaginare che quando vengono mandati i processi, iniziano tutti a leggere il codice e a eseguirlo. Quindi rank 0 manda un messaggio e allo stesso momento per come è scritto il codice lo può fare anche rank 1 entrambi sono impegnati a mandare il messaggio e quindi non possono riceverlo. Essendo bloccanti.


# testing:



