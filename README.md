# FHPC - Final Assignment
This repository houses the final assignment for the "Foundations of High Performance Computing" course (Academic Year 2022/2023) offered through the Master's Degree in Data Science and Scientific Computing at the University of Trieste.

For detailed instructions and more information, please refer to the official [GitHub page of the course](https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/tree/main/Assignment).

The assignment comprises two primary exercises:

## Exercise 1: Conway's Game of Life (GOL) Implementation

This exercise involves implementing Conway's Game of Life using a hybrid MPI+OpenMP approach. The MPI component is responsible for distributing the initial "playground" across various MPI tasks, whereas the OpenMP component leverages multiple threads to parallelize execution within each task. We conducted three scalability studies on this implementation: OpenMP Scalability, Strong MPI Scalability, and Weak MPI Scalability.

## Exercise 2: Performance Comparison of Mathematical Libraries

The second exercise focuses on comparing the performance of three mathematical libraries—MKL, openBLAS, and BLIS—using the gemm.c function for both single and double precision floating-point operations to execute matrix-matrix multiplication. We explored different thread allocation policies (close and spread) and tested various scenarios, including increasing the matrix size with a fixed number of cores and increasing the number of cores with a fixed matrix size.


Additional details are available in the specific directories for each exercise and within the Report.pdf file.

## The structure of this github directory is:

```
README.md 
report.pdf
exercise1/README.md 
exercise1/all the files for exercise 1 
exercise2/README.md 
exercise2/all the files for exercise 2
```

