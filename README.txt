Noah Olson
nvolson
CS 3516 Computer Networks

                                 Programming Assignment 3: Distance Vector Routing Protocol

---Compiling---
        1. Open a new terminal at the "project3" folder
        2. Type "make" in the terminal
        3. This will create an executable and an object files
        4. Client is fully compiled

---Running---
        1. Open a new terminal at the "project3" folder
        2. enter ./project3
        5. choose tracing level
        7. project3 will run and output distance tables, packets, and times of function calls

---Additional Notes---
    Table is in format: Row = source, column = destination
    As described in the piazza post: "What Should our Final Table Look Like?"
   
    Example: 
                D0 |    dst 0 dst 1 dst 2 dst 3
                ----|-------------------------------
                src 0 |     0     1     2     4
                src 1 |     1     0     1     3
                src 2 |     2     1     0     2
                src 3 |     4     3     2     0

                The cost from 1 to 0 is 1
                The cost from 1 to 1 is 0
                The cost from 1 to 2 is 1
                The cost from 1 to 3 is 3
                etc.
