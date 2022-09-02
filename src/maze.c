#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "graphics.h"

int gridSize;
int cellSize;

int colourScheme;
int willViewAnimation;

int top = -1;

typedef struct Cell
{
    int x;
    int y;

    //wall values are bools, for wether they should be drawn or not;

    int topWall;
    int leftWall;
    int rightWall;
    int bottomWall;

    int visited; //bool

}Cell;

void push(Cell stack[], Cell cell)
{
    top++;
    stack[top] = cell;
}

Cell updateCell(Cell** grid, Cell cell)
{
   /*  what is stored in the stack is the instance of the cell
    at that time, so wall values may have changed but this
    will not have been updated in the stack.
    so we need to update these values */

    int xIndex = cell.x/cellSize;
    int yIndex = cell.y/cellSize;

    cell = grid[yIndex][xIndex];

    return cell;
}

Cell pop(Cell** grid, Cell stack[]){
    Cell cell = stack[top];
    top--;     
    return updateCell(grid, cell); 
}

int stackIsEmpty()
{
    if (top == -1)
    {
        return 1;
    }
    return 0;
}


Cell** mallocGrid() //googled how to pass a 2d array into functions, source: https://stackoverflow.com/questions/3911400/how-to-pass-2d-array-matrix-in-a-function-in-c
{ 
    Cell **grid;
    grid = malloc(gridSize * sizeof *grid);
    for (int i = 0; i < gridSize; i++)
    {
        grid[i] = malloc(gridSize * sizeof *grid[i]);
    }
    return grid;
}

void initializeWalls(int x, int y, Cell** grid)
{  
    grid[y][x].topWall = 1;  
    grid[y][x].bottomWall = 1;

    if (y == 0 && x == 0) //set start
    {
        grid[y][x].leftWall = 0;
        grid[y][x].rightWall = 1;
    }
    else if (x == gridSize - 1 && y == gridSize - 1) //set finish
    {
        grid[y][x].leftWall = 1;
        grid[y][x].rightWall = 0;
    }
    else
    {
        grid[y][x].leftWall = 1;
        grid[y][x].rightWall = 1;
    }
}

void initializeCells(Cell** grid)
{
    for (int y = 0; y < gridSize; y++)
    {
        for (int x = 0; x < gridSize; x++)
        {
            grid[y][x].x = x*cellSize;
            grid[y][x].y = y*cellSize;

            initializeWalls(x, y, grid); 

            grid[y][x].visited = 0;           
        }        
    }
    
}

void drawWalls(Cell** grid, int x, int y, int xIndex, int yIndex)
{ 
    if (grid[yIndex][xIndex].topWall == 1)
    {
        drawLine(x, y, x + cellSize, y);      
    }
    if (grid[yIndex][xIndex].leftWall == 1)
    {
        drawLine(x, y, x, y + cellSize);
    }
    if (grid[yIndex][xIndex].rightWall == 1)
    {
        drawLine(x + cellSize, y, x + cellSize, y + cellSize);
    }
    if (grid[yIndex][xIndex].bottomWall == 1)
    {
        drawLine(x, y+cellSize, x+cellSize, y+cellSize);
    }
}

void drawMaze(Cell** grid)
{
    for (int j = 0; j < gridSize; j++)
    {
        for (int i = 0; i < gridSize; i++)
        {
            int x = grid[j][i].x;
            int y = grid[j][i].y;

            drawWalls(grid, x, y, i, j);
        }       
    }
}

int validNeighbour(Cell** grid, Cell currentCell, int direction)
{
    int xIndex = currentCell.x / cellSize;
    int yIndex = currentCell.y / cellSize;
    if (direction == 0)//check up
    {
        if (yIndex-1 > -1)
        {
            if (grid[yIndex-1][xIndex].visited == 0)
            {
                return 1;
            }       
        }
        return 0;  
    }

    else if (direction == 1) //check right
    {
        if (xIndex+1 < gridSize)
        {
            if (grid[yIndex][xIndex+1].visited == 0)
            {
                return 1;
            }       
        }
        return 0;  
    }

    else if (direction == 2) // check down
    {
        if (yIndex + 1 < gridSize)
        {
            if (grid[yIndex+1][xIndex].visited == 0)
            {
                return 1;
            }       
        }
        return 0;
    }

    else //check left
    {
        if (xIndex-1 > -1)
        {
            if (grid[yIndex][xIndex-1].visited == 0)
            {
                return 1;
            }       
        }
        return 0; 
    }       
}

int hasNeighbours(Cell** grid, Cell currentCell)
{
    for (int i = 0; i < 4; i++) //checks for valid neighbour in each direction
    {
        if (validNeighbour(grid, currentCell, i))
        {
            return 1;
        }       
    }
    return 0;   
}

Cell pickRandomNeighbour(Cell** grid, Cell currentCell)
{
    int xIndex = currentCell.x / cellSize;
    int yIndex = currentCell.y / cellSize;
    
    int neighbourFound = 0;
    Cell randomNeighbour = currentCell;
    while (!neighbourFound)
    {
        int randomDirection = rand() % 4;
        if (randomDirection == 0)//up
        {
            if (validNeighbour(grid, currentCell, randomDirection))
            {
                randomNeighbour = grid[yIndex-1][xIndex];
                neighbourFound = 1;
            }           
        }
        else if (randomDirection == 1)//right
        {
            if (validNeighbour(grid, currentCell, randomDirection))
            {
                randomNeighbour = grid[yIndex][xIndex+1];
                neighbourFound = 1;
            }           
        }
        else if (randomDirection == 2)//down
        {
            if (validNeighbour(grid, currentCell, randomDirection))
            {
                randomNeighbour = grid[yIndex + 1][xIndex];
                neighbourFound = 1;
            }           
        }
        else//left
        {
            if (validNeighbour(grid, currentCell, randomDirection))
            {
                randomNeighbour = grid[yIndex][xIndex-1];
                neighbourFound = 1;
            }     
        }
    }
    return randomNeighbour;
}

void removeWalls(Cell** grid, Cell stack[], Cell currentCell, Cell nextCell)
{
    int currentXIndex = currentCell.x / cellSize;
    int currentYIndex = currentCell.y / cellSize;

    int nextXIndex = nextCell.x / cellSize;
    int nextYIndex = nextCell.y / cellSize;

    if (nextYIndex == currentYIndex - 1)//above current
    {
        currentCell.topWall = 0;
        nextCell.bottomWall = 0;
    }
    if (nextXIndex == currentXIndex + 1)//right of current
    {
        currentCell.rightWall = 0;
        nextCell.leftWall = 0;
    }
    if (nextYIndex == currentYIndex + 1)//bellow current
    {
        currentCell.bottomWall = 0;
        nextCell.topWall = 0;
    }
    if (nextXIndex == currentXIndex - 1)//left of current
    {
        currentCell.leftWall = 0;
        nextCell.rightWall = 0;
    }
    nextCell.visited = 1;  
    
    push(stack, nextCell);

    grid[currentYIndex][currentXIndex] = currentCell;
    grid[nextYIndex][nextXIndex] = nextCell;
}

void createMaze(Cell** grid, Cell stack[]) //algorithm used here is randomized depth-first search. source: https://en.wikipedia.org/wiki/Maze_generation_algorithm
{
    Cell startCell = grid[0][0];
    startCell.visited = 1;
    grid[0][0] = startCell;
    push(stack, startCell);
    while (!stackIsEmpty())
    {
        Cell currentCell = pop(grid, stack);
        if (hasNeighbours(grid, currentCell))
        {
            push(stack, currentCell);
            Cell nextCell = pickRandomNeighbour(grid, currentCell);
            removeWalls(grid, stack, currentCell, nextCell);
        }       
    }
    
}

int endReached(Cell**grid, Cell currentCell)
{
    if (currentCell.x == grid[gridSize-1][gridSize-1].x && currentCell.y == grid[gridSize-1][gridSize-1].y)
    {
        return 1;
    }
    return 0;
}

int wallCount(Cell currentCell)
{
    int wallCount = currentCell.topWall + currentCell.leftWall + currentCell.rightWall + currentCell.bottomWall;
    return wallCount; 
}

int decisionPoint(Cell currentCell){
    if (wallCount(currentCell) < 2) 
    {
        return 1;
    }
    return 0;
}

int isNode(Cell currentCell, int direction)
{
    if (direction == 0) //going up
    {
        if (currentCell.topWall == 1) //either a dead end or a turn, both nodes.
        {
            return 1;
        }     
    }
    if (direction == 1) //going right
    {
        if (currentCell.rightWall == 1) 
        {
            return 1;
        }     
    }
    if (direction == 2) //going down 
    {
        if (currentCell.bottomWall == 1) 
        {
            return 1;
        }     
    }
    if (direction == 3) //going left
    {
        if (currentCell.leftWall == 1)
        {
            return 1;
        }     
    }

    if (decisionPoint(currentCell)) //if can go in multiple directions, then its a node
    {
        return 1;
    }
    return 0;
}

int neighbourVisited(Cell currentCell, Cell visited[], int visitedIndex, int direction)
{
    int currentXIndex = currentCell.x / cellSize;
    int currentYIndex = currentCell.y / cellSize;

    for (int i = 0; i < visitedIndex + 1; i++)
    {
        int visitedXIndex = visited[i].x / cellSize;
        int visitedYIndex = visited[i].y / cellSize;
        if (direction == 0)//up
        {
            if (visitedXIndex == currentXIndex && visitedYIndex == currentYIndex - 1)
            {
                return 1;
            }  
        }
        else if (direction == 1)//right
        {
            if (visitedXIndex == currentXIndex + 1 && visitedYIndex == currentYIndex)
            {
                return 1;
            }  
        }
        else if (direction == 2)//down
        {
            if (visitedXIndex == currentXIndex && visitedYIndex == currentYIndex + 1)
            {
                return 1;
            }  
        }
        else //left
        {
            if (visitedXIndex == currentXIndex - 1 && visitedYIndex == currentYIndex)
            {
                return 1;
            }  
        }           
    }
    return 0;
    
}

int nextCellInDirectionValid(Cell currentCell, Cell visited[], int visitedIndex, int direction)
{
    if (direction == 0) //above
    {
        if (currentCell.topWall == 0)
        {
            if (!neighbourVisited(currentCell, visited, visitedIndex, 0))
            {
                return 1;
            }   
        }
    }
    else if (direction == 1) //right
    {
        if (currentCell.rightWall == 0)
        {
            if (!neighbourVisited(currentCell, visited, visitedIndex, 1))
            {
                return 1;
            }   
        }
    }
    else if (direction == 2) //bellow
    {
        if (currentCell.bottomWall == 0)
        {
            if (!neighbourVisited(currentCell, visited, visitedIndex, 2))
            {
                return 1;
            }   
        }
    }
    else // left
    {
        if (currentCell.leftWall == 0)
        {
            if (!neighbourVisited(currentCell, visited, visitedIndex, 3))
            {
                return 1;
            }   
        }
    }
    return 0;
}

int deadEnd(Cell currentCell, Cell visited[], int visitedIndex)
{
    if (wallCount(currentCell) == 3)
    {
        return 1;
    }

    for (int direction = 0; direction < 4; direction++) //checks each direction, NESW, if a direction is valid and unvisited, then its not a dead end.
    {
        if (nextCellInDirectionValid(currentCell, visited, visitedIndex, direction))
        {
            return 0;
        }
        
    }

    return 1;
    
}

int pickRandomDirection(Cell currentCell, Cell visited[], int visitedIndex)
{
    int validDirectionFound = 0;
    int randomDirection = -1;
    while (!validDirectionFound)
    {
        randomDirection = rand() % 4;
        if (visitedIndex == 0) //when entering maze, we need to make sure we dont go left, as impossible to mark cell [0, -1] as visited
        {
            if (randomDirection != 3){
                if (nextCellInDirectionValid(currentCell, visited, visitedIndex, randomDirection))
                {
                    validDirectionFound = 1;
                }
            }   
        }
        else
        {
            if (nextCellInDirectionValid(currentCell, visited, visitedIndex, randomDirection))
            {
                validDirectionFound = 1;
            }
        }    
    }
    return randomDirection;
}

Cell incrementInDirection(Cell** grid, Cell currentCell, int direction)
{
    int xIndex = currentCell.x / cellSize;
    int yIndex = currentCell.y / cellSize;

    Cell nextCell;

    if (direction == 0)//going up
    {
        nextCell = grid[yIndex-1][xIndex];
    }
    else if (direction == 1)//go right
    {
        nextCell =  grid[yIndex][xIndex+1];
    }
    else if (direction == 2)//go down
    {
        nextCell = grid[yIndex + 1][xIndex];
    }
    else//go left
    {
        nextCell = grid[yIndex][xIndex - 1];
    }
    return nextCell;
}

void drawSolution(Cell stack[])
{
    if (colourScheme == 1)
    {
        setColour(red);
    }
    else
    {
        setColour(green);
    }
    for (int i = 0; i < top; i++)
    {
        int x1 = stack[i].x + cellSize / 2;
        int y1 = stack[i].y + cellSize / 2;
        
        int x2 = stack[i+1].x + cellSize / 2;
        int y2 = stack[i+1].y + cellSize / 2;

        drawLine(x1, y1, x2, y2);
    }
    
}

void animateSolve(Cell** grid, Cell stack[])
{
    if (colourScheme == 1)
    {
       setColour(black);
    }
    else
    {
        setColour(white);
    }
    drawMaze(grid);
    drawSolution(stack);
}

void resetMaze(Cell** grid)
{
    if (colourScheme == 1)
    {
        setColour(white);
        fillRect(0, 0, gridSize*cellSize, gridSize*cellSize);
        setColour(black);
    }
    else
    {
        setColour(black);
        fillRect(0, 0, gridSize*cellSize, gridSize*cellSize);
        setColour(white);
    }
    drawMaze(grid);
}

void solveMaze(Cell** grid, Cell stack[]) //algorithm used here is another version of depth first search, source: https://www.quora.com/How-do-I-solve-a-maze-with-depth-first-search?share=1
{
    top = -1;
    Cell visited[gridSize*gridSize];
    Cell currentCell;
    int visitedIndex = -1;
    Cell startCell = grid[0][0];
    int direction = 1; // up = 0, right = 1, down = 2, left = 3, think NESW

    push(stack, startCell); //start point must be a node

    currentCell = startCell;

    visitedIndex++;
    visited[visitedIndex] = currentCell;

    while (!endReached(grid, currentCell))
    {
        if (isNode(currentCell, direction))
        {
            while (deadEnd(currentCell, visited, visitedIndex))
            {
                currentCell = pop(grid, stack);
            }
            push(stack, currentCell);
            direction = pickRandomDirection(currentCell, visited, visitedIndex);
        }
        Cell nextCell = incrementInDirection(grid, currentCell, direction);   
        currentCell = nextCell;

        visitedIndex++;
        visited[visitedIndex] = currentCell;

        if (willViewAnimation)
        {
            animateSolve(grid, stack);
        }
    }    

    Cell endCell = grid[gridSize - 1][gridSize - 1]; //end point must be a node
    push(stack, endCell);

    if (willViewAnimation)
    {
        resetMaze(grid);
    }
    
}

void freeGrid(Cell** grid)
{
    for (int i=0; i<gridSize; i++)
    {
        free(grid[i]);
    }
    free(grid);
}

int getInput(int lowerBound, int upperBound, char inputPrompt[], char output[])
{
    char input[10];
    int returnValue = -1;
    while (returnValue < lowerBound || returnValue > upperBound)
    {
        fprintf(stderr, "%s: \n", inputPrompt);
        fgets(input,10,stdin);
        returnValue = atoi(input);
    }
    fprintf(stderr, "%s.\n", output);
    return returnValue;
}

void setMazeComplexity()
{
    int complexity = getInput(1, 5, "please enter a complexity/size (1-5)", "Ok");

    if (complexity == 1)
    {
        gridSize = 10;
        cellSize = 20;
    }
    else if (complexity == 2)
    {
        gridSize = 20;
        cellSize = 10;
    }
    else if (complexity == 3)
    {
        gridSize = 30;
        cellSize = 10;
    }
    else if (complexity == 4)
    {
        gridSize = 50;
        cellSize = 5;
    }
    else
    {
        gridSize = 60;
        cellSize = 5;
    }   
}


void setColourScheme()
{
    colourScheme = getInput(1, 2, "please choose a colour scheme. Enter 1 for light, 2 for dark", "Ok");
    if (colourScheme == 1)
    {
        setColour(white);
        fillRect(0, 0, 1000, 1000);
        setColour(black);
    }
    
    else
    {
        setColour(black);
        fillRect(0, 0, 1000, 1000);
        setColour(white);
    }
    
}

int willViewSolution()
{
    char inputPrompt[] = "do you wish to view the solution? enter 1 for yes and 0 for no, The solution will be indicated by the coloured line";
    char output[] = "Ok";
    return getInput(0, 1, inputPrompt, output);
}

void setPreferences()
{
    setColourScheme();
    setMazeComplexity();  
}

void getWillViewAnimation()
{
    char inputPrompt[] = "do you wish to view the program solving the maze as an animation? This is better for more complex mazes. enter 1 for yes or 0 for no";
    char output[] = "ok. Please open the java application to view the maze";
    willViewAnimation = getInput(0, 1, inputPrompt, output);
}

int main(void)
{
    srand(time(0)); 

    setPreferences();

    Cell **grid = mallocGrid();

    Cell stack[gridSize * gridSize];
    
    initializeCells(grid);

    createMaze(grid, stack);
    drawMaze(grid);

    if (willViewSolution())
    {   
        getWillViewAnimation();
        solveMaze(grid, stack);
        drawSolution(stack);
    } 
    else
    {
        fprintf(stderr, "please open the java app to view the maze.\n");
    }
    
    freeGrid(grid);
}