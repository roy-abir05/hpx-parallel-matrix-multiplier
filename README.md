# HPX Parallel Matrix Multiplier

This repository contains a C++ implementation of matrix multiplication using the [HPX](https://hpx.stellar-group.org/) library for multithreading and parallelism.

To optimize performance and make better use of the CPU cache, the algorithm uses a tiled (or blocked) matrix multiplication approach rather than the standard naive three-loop method.

## Prerequisites

To build and run this code, you will need:

- A C++17 (or higher) compatible compiler (GCC, Clang, or MSVC).
- CMake for building the project.
- The HPX library installed and configured on your system.

## Build Instructions

You can build this project using standard CMake commands. Create a build directory, run CMake, and then compile:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

The executable uses HPX's program options to handle command-line arguments. You can pass the dimensions of the matrices directly when running the program.

By default, the program multiplies two 1000x1000 matrices.

_(Following commands to be run from inside `build` directory)_

```bash
./hpx-parallel-matrix-multiplier
```

### Specifying Matrix Dimensions

You can provide custom values for the matrix sizes using the `--n`, `--m`, and `--k` flags:

- `--n`: Number of rows in the first matrix.
- `--m`: Number of columns in the first matrix (and rows in the second matrix).
- `--k`: Number of columns in the second matrix.

Example: Multiplying a 500x1000 matrix with a 1000x200 matrix:

```bash
./hpx-parallel-matrix-multiplier --n 10 --m 10 --k 10
```

### HPX Threading Options

Since this is an HPX application, you can also use standard HPX command-line flags. For example, to specify the number of OS threads HPX should use, add `--hpx:threads`:

Bash

```
./hpx_matrix_mul --n=2048 --m=2048 --k=2048 --hpx:threads=4

```

## Implementation Details

- **Parallelism:** The code uses `hpx::experimental::for_loop` with `hpx::execution::par` to parallelize the computation over the row blocks of the resulting matrix.
- **Tiling:** The matrices are divided into smaller blocks (tiles) of size 64x64. This ensures that the data being worked on fits nicely into the CPU cache, significantly reducing memory access times for large matrices.
- **Output:** For matrices smaller than 10x10, the program prints the resulting matrix to the console. For larger matrices, it suppresses the print output to avoid flooding the terminal, but it will always output the total elapsed time.
