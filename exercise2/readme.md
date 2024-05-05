# Exercise 2

This exercise is about comparing the performance of the matrix-matrix multiplication function (gemm) from three different libraries: MKL, OpenBLAS, and BLIS. The comparison is done using single and double-point precision and different thread affinity policies. The performance is measured in terms of the time taken to execute the function for different matrix sizes and thread configurations.

This directory contains the following:

gemm.c: A program that calls the gemm function and measures its performance.
Makefile: Used to compile gemm.c with different libraries and precisions.
The data collected on ORFEO's EPYC and THIN nodes are organized in the respective folders:

- fixed_cores/ and fixed_size/: Each containing:
  - thin/ : Contains data related to scalability with respect to thin parition. Inside it, there are two subdirectories:
      - close/: Contains csv files, job files and png images for each configuration using the 'close' thread affinity policy.
      - spread/: Contains csv files, job files and png images for each configuration using the 'spread' thread affinity policy.
  - epyc/: Contains data related to scalability with respect to epyc partition. Inside it, there are two subdirectories:
      - close/: Contains csv files, job files and png images for each configuration using the 'close' thread affinity policy.
      - spread/: Contains csv files, job files and png images for each configuration using the 'spread' thread affinity policy.
- plot/: Contains the plots for the report.
- plot.ipynb: A Jupyter notebook used to generate the plots.
- notes.md: My personal notes

This exercise started with a functional gemm.c code (called dgemm.c but referred to as gemm.c in the original course repository Makefile see [here](https://github.com/Foundations-of-HPC/Foundations_of_HPC_2022/tree/main/Assignment)). This code is designed to call the gemm function using single or double point precision from any of the three libraries and measure its performance. I modified it slightly to enable the output of results to a CSV file, differentiated by the library and precision used.

The provided Makefile (see here) compiles gemm.c using different libraries and double-point precision. I made slight modifications to it, allowing for compilation with single-point precision, output of results to a file, and correct directory compilation of the executables.