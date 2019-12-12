# Random Input Generator - Undirected Graph Creator - Shortest Path Between Two Vertices Pipeline

Simple set of programs built in both Python and C++ that do the following:
* `ece650-a3.cpp`
   * This is the driver program that uses Inter-Process Communication (IPC) to link the output of the random input generator(`rgen.cpp`) to the input of the Python script(`ece650-a1.py`), and the output of the Python script to the input of the C++ program that calculates the shortest path (`ece650-a2.cpp`)
* `rgen.cpp`
   * Creates random street inputs for the python script
* `ece650-a1.py`
   * Python script that turns a set of streets and points into an undirected graph as specified in repo (https://github.com/gaberch/Graph_Creator)
* `ece650-a2.cpp`
   * Takes an undirected graph as input and calculates the shortest path between two vertices as specified in repo (https://github.com/gaberch/Undirected_Graph_Shortest_Path)

## CMake

The CMake instructions are contained in `CMakeLists.txt`. To compile the program, do the following, starting in the main repo:
```
mkdir build
cd build
cmake ../
make install
```

## Resources

For the creation of the inter-process communication system, the book "Advanced Linux Programming" was used, more specifically, Chapter 3 on Processes and Chapter 5.4 on Pipes was found most useful.

## Sample Run

Assume that the executable is called `ece650-a3`. In the following, `$` is the command-prompt.

```
$ cd build
$ cmake ../
$ make install
$ cd ./run/bin
$ ./ece650-a3 -s 5 -n 4 -l 5
V 8
E {<0,2>,<0,3>,<0,4>,<1,3>,<4,7>,<5,2>,<5,6>}
s 2 4
2-0-4
```

In the above sample run, the lines "V = ...", "E = ...." and "2-0-4" are output by the program.

## Input and output
The program takes input from standard input and output is sent to standard putput. Errors are sent to standard error.

## Random input generator

As you can see in the sample run, the program can be started with four command line arguments that are completely optional. All four arguments are for the random input generator. The commands mean the following:
* `-s k` - where `k` is an integer >= 2. The number of streets is chosen to be a random integer [2,k]. If this option is not specified, the default value for k is set to `10`; that is, the number of streets to be generated is a random integer in [2,10]
* `-n k` - where k is an integer >= 1. The number of line-segments in each street is chosen to be a random integer in [1,k]. Default: k = 5.
* `-l k` - where k is an integer >= 5. The process waits a random number `w` seconds, where `w` is in [5,k] before generating the next (random) input. Default: k = 5.
* `-c k` - where k is an integer >= 1. The process will generate *(x,y)* coordinates such that every *x* and *y* value is in the range [-k,k]. For example, if k = 15, all the coordinate values will be integers between -15 and 15. Default: k = 20.

If a line-segment is generated that overlaps with another line-segment (across all streets) generated earlier, the random generator will regenerate that line segment. Also, any zero-length line segments are regenerated.

In order to prevent the random generator from going into an infinite loop trying to fit within the previously mentioned constraints, a limited number of total tried are implemented (in this case it is 25 total attempts). When this limit is reached, an error output similar to the following will be output: `Error: Failed to generate valid input for 25 simultaneous attempts`.
