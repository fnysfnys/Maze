# Maze Generation:

For creating the maze, I used Wikipedia to find a suitable algorithm
which I thought would be easy to implement. I chose randomized depth-first search
source: https://en.wikipedia.org/wiki/Maze_generation_algorithm

I chose the iterative implementation. the steps to this are:

Choose the initial cell, mark it as visited and push it to the stack
While the stack is not empty

    Pop a cell from the stack and make it a current cell
    If the current cell has any neighbours which have not been visited
        Push the current cell to the stack
        Choose one of the unvisited neighbours
        Remove the wall between the current cell and the chosen cell
        Mark the chosen cell as visited and push it to the stack

the implementation of this can be seen in the function createMaze

## Implementation:

To implement this, I needed to make a grid of cells, each with four walls to start with
and these cells need to have coordinates and be able to be marked as visited.

To do this I created a data structure called Cell which would hold the information for each wall
and whether it has removed, its coordinates and whether or not it has been visited.
each value was represented with integers and for the walls and visited values, 1 represented yes and 0 no.

I then created a 2d array of cells to represent the grid of cells which would become the maze.

to implement the stack I made an array of cells and an integer top, which was incremented if something was
pushed to the stack and decremented if something was popped, this acted as an index.

The size of grid would not be known until run time, so I used this article to find out how
I could pass this 2d array into a function.

Source: https://stackoverflow.com/questions/3911400/how-to-pass-2d-array-matrix-in-a-function-in-c

# Maze Solving:

This part I found tricky, I again used Wikipedia to find an algorithm
source: https://en.wikipedia.org/wiki/Maze_solving_algorithm

I initially chose dead-end filling, where you take each dead end and fill until
you reach a crossroads, which I thought would be easy to implement but as I began
planning it started to become increasingly more difficult to implement.

I then decided to go for an easier option, the left-hand rule which I learned from
one of Mark Handley’s lectures. this was easy to implement, however the solution included
dead ends where the program would 'turn around' and again I found it very difficult to 
implement a solution to remove these that was guaranteed, my solution only worked in certain circumstances

Finally, I did some more searching and found this Quora question on an algorithm
to solve a maze. 
Source: https://www.quora.com/How-do-I-solve-a-maze-with-depth-first-search?share=1

the response was to use depth first search and the answer gave the following steps

Enter the maze
If you have multiple ways, choose anyone and move forward
Keep choosing a way which was not seen so far till you exit the maze or reach dead end
If you exit maze, you are done.
If you reach dead end, this is wrong path, so take one step back, choose different path.
If all paths are seen in this, take one step back and repeat

I then converted this into a lower-level algorithm:

create a stack to store nodes.

a node in this case is defined as a cell where the maze makes a change, i.e., it turns a corner,
reaches a dead end or has multiple options.

set the current cell to the start position and make it a node by pushing it to the stack

mark the cell as visited.

while the end has not been reached
    if the cell is a node
        while the cell is a dead end
            pop from the stack and make it the current cell
        push the current cell
        pick a random direction
        make the current cell the next cell in the chosen random direction
        mark the cell as visited

When the end has been reached, all the decision points or nodes that lead to the exit
will remain in the stack.

The implementation of this can be seen in the function solveMaze

# drawing the maze and solution:

to draw the maze, I used a nested loop to go through each element in grid. and if the wall value was 1
id draw the wall using the coordinates of the cell.

to draw the solution I simply looped through the stack and used the draw line function to draw a line
between the current cell in the stack and the next cell in the stack using their coordinates.

# Running the program:

compile using: gcc maze.c graphics.c
to run it: ./a.out | java -jar drawapp.jar

When the program runs the java application will open but before anything is drawn
there will be instructions from the command line where you will be able to set your
desired preferences

these instructions will prompt you when to view the java application.

