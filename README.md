# Sudoku-Solver-Using-AI

### Sudoku: 
The Sudoku board consists of 9 × 9 cells divided into 3 × 3 distinct blocks. 
Some cells already have numbers (1 to 9) assigned to them initially.

The goal is to find assignments (1 to 9) for the empty cells so that every row, column, and 
3 × 3 block contains all the digits from 1 to 9. 
Each of the 9 digits, therefore, can only appear once in every row, column, and block. 

In this implementation, 0's will represent blank cells that need to be assigned. 

### How To Run: 
In source code, in line 650, where it says, 
`ifs.open("Input1.txt");`
change “Input1.txt” to whichever input file you wish to read from. 

In source code, in line 722, where it says, 
`ofstream ofs("Output1.txt");`
change “Output1.txt” to whichever output file you wish to write to. 

To compile, and then run, first, in command line, `cd` to the directory of this file and its input file. 

Then type 
```
g++ -o sudoku sudoku.cpp 
./sudoku
```

### Project Description: 
Implement the Backtracking Algorithm to solve Sudoku puzzles.

### Input/Output Format: 
The program will read in the initial Sudoku board cell values from an input text file and 
produce an output text file that contains the solution. The input file contains 9 rows (or 
lines) of integers. Each row contains 9 integers ranging from 0 to 9, separated by blank spaces. 
Digits 1-9 represent the cell values and 0’s represent blank cells. 

Similarly, the output file contains 9 rows of integers, as shown in Figure 5 below. Each row contains 
9 integers ranging from 1 to 9 (without 0, the blank cell) separated by blank spaces.
