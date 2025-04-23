#include "solver.h"
#include "API.h"

#define SIZE 16

Action solver() {
    return floodFill();
}

// This is an example of a simple left wall following algorithm.
Action leftWallFollower() {
    if(API_wallFront()) {
        if(API_wallLeft()){
            return RIGHT;
        }
        return LEFT;
    }
    return FORWARD;
}



cell grid [SIZE][SIZE];
point queue[SIZE * SIZE];

int vWalls [SIZE][SIZE+1] = {0};
int hWalls [SIZE+1][SIZE] = {0};

int mouseX = 0;
int mouseY = SIZE -1;
int currentDist = -1;
int currentDir = 0;



int front = 0, back = 0;

void enqueue(int x, int y) {
    queue[back++] = (point){x, y};
}

point dequeue() {
    return queue[front++];
}

int isQueueEmpty() {
    return front == back;
}


void init_coords(cell(*p)[SIZE] ){
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cell *coordinate = &p[i][j]; 
            coordinate->distanceVal = -1;

            //wall boundaries
            if (j == 0){
                vWalls[i][j] = 1; 
                API_setWall(i, j, 'W');
            }
            if (j == SIZE -1){
                vWalls[i][j + 1] = 1;
                API_setWall(i, j, 'E');
            }
            if (i == 0){
                hWalls[i][j] = 1;
                API_setWall(i, j, 'N');
            }
            if (i == SIZE -1){
                hWalls[i + 1][j] = 1;
                API_setWall(i, j, 'S');
            }
        }
    }

    //set goal
    int val1 = (SIZE/2);
    int val2 = val1 -1;

    p[val2][val2].distanceVal = 0;
    p[val2][val1].distanceVal = 0;
    p[val1][val2].distanceVal = 0;
    p[val1][val1].distanceVal = 0;

    


    // enqueue goal cells

    front = 0;
    back = 0;

    enqueue(val2, val2);
    enqueue(val2, val1);
    enqueue(val1, val2);
    enqueue(val1, val1);

}


void reinit_distances(cell(*p)[SIZE]) {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            cell *coordinate = &p[i][j];
            coordinate->distanceVal = -1; 
        }
    }

    // Set goal
    int val1 = (SIZE / 2);
    int val2 = val1 - 1;

    p[val2][val2].distanceVal = 0;
    p[val2][val1].distanceVal = 0;
    p[val1][val2].distanceVal = 0;
    p[val1][val1].distanceVal = 0;


    // Enqueue goal cells
    front = 0;
    back = 0;
    enqueue(val2, val2);
    enqueue(val2, val1);
    enqueue(val1, val2);
    enqueue(val1, val1);
}

void flood(){
    init_coords(grid);
    while (!isQueueEmpty()){
        point p = dequeue();
        int x = p.x, y = p.y;
        int dist = grid[x][y].distanceVal + 1;
        char text [10];
        sprintf(text, "%d", dist);

        if (x > 0 && hWalls[x][y] == 0 && grid[x - 1][y].distanceVal == -1) {
            grid[x-1][y].distanceVal = dist;
            API_setText(x-1, y, text);
            enqueue(x-1, y);
        }
        if (x < SIZE - 1 && hWalls[x + 1][y] == 0 && grid[x + 1][y].distanceVal == -1) {
            grid[x+1][y].distanceVal = dist;
            API_setText(x+1, y, text);
            enqueue(x+1, y);
        }
        if (y > 0 && vWalls[x][y] == 0 && grid[x][y - 1].distanceVal == -1) {
            grid[x][y-1].distanceVal = dist;
            API_setText(x, y-1, text);
            enqueue(x, y-1);
        }
        if (y < SIZE - 1 && vWalls[x][y + 1] == 0 && grid[x][y + 1].distanceVal == -1) {
            grid[x][y+1].distanceVal = dist;
            API_setText(x, y+1, text);
            enqueue(x, y+1);
        }
    }

}

Direction getTrueDirection(Direction facing, char move) {
    switch (move) {
        case 'L':  // Turn Left
            return (Direction)((facing + 3) % 4);
        case 'R':  // Turn Right
            return (Direction)((facing + 1) % 4);
        case 'F':  // Move Forward
            return facing;
        case 'B':  // Move Backward
            return (Direction)((facing + 2) % 4);
        default:
            return facing;  // Invalid input, return current direction
    }
}

void updatePosition(Direction facing) {
     // Move Straight
    switch (facing) {
        case north: mouseY--; break;
        case east:  mouseX++; break;
        case south: mouseY++; break;
        case west:  mouseX--; break;
    }
}


void updateWalls(int x, int y, int dir) {
    switch (dir) {
        case 0: // North
            if (x > 0) {
                hWalls[y][x] = 1;
                API_setWall(x, y, 'n');  
            }
            break;

        case 1: // East
            if (y < SIZE - 1) {
                vWalls[y][x + 1] = 1;
                API_setWall(x, y, 'e');  
            }
            break;

        case 2: // South
            if (x < SIZE - 1) {
                hWalls[y + 1][x] = 1;
                API_setWall(x, y, 's');  
            }
            break;

        case 3: // West
            if (y > 0) {
                vWalls[y][x] = 1;
                API_setWall(x, y, 'w');  
            }
            break;
    }
}


void printWalls() {
    debug_log("\n--- Vertical Walls (vWalls) ---");
    for (int i = 0; i < SIZE; i++) {
        char row[SIZE * 4];  // Buffer for printing
        int index = 0;
        for (int j = 0; j < SIZE + 1; j++) {
            index += sprintf(&row[index], "%d ", vWalls[i][j]);
        }
        debug_log("%s", row);
    }

    debug_log("\n--- Horizontal Walls (hWalls) ---");
    for (int i = 0; i < SIZE + 1; i++) {
        char row[SIZE * 4];  // Buffer for printing
        int index = 0;
        for (int j = 0; j < SIZE; j++) {
            index += sprintf(&row[index], "%d ", hWalls[i][j]);
        }
        debug_log("%s", row);
    }
}









// Put your implementation of floodfill here!
Action floodFill() {
    flood();

    debug_log("\n--- DEBUG INFO ---");
    debug_log("Current Position: (%d, %d)", mouseX, mouseY);
    debug_log("Current Direction: %d", currentDir);
    debug_log("Dist value: %d", grid[mouseY][mouseX].distanceVal);

    // Check available directions
    int leftOpen = !API_wallLeft();
    int forwardOpen = !API_wallFront();
    int rightOpen = !API_wallRight();

    debug_log("Walls Detected - Left: %d, Forward: %d, Right: %d", !leftOpen, !forwardOpen, !rightOpen);

    if (!leftOpen){
        debug_log("Left wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'L'));
        printWalls();
    }
    if (!forwardOpen){
        debug_log("Front wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'F'));
        printWalls();
    }
    if (!rightOpen){
        debug_log("Right wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'R'));
        printWalls();
    }

    // 1. Check Left First
    if (!API_wallLeft()) {
        debug_log("Turning LEFT");
        int newDir = getTrueDirection(currentDir, 'L');
        currentDir = newDir;
        updatePosition(currentDir);
        flood();
        return LEFT;
    } else {
        debug_log("Left wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'L'));
        printWalls();
    }

    // 2. Check Forward
    if (forwardOpen) {
        debug_log("Moving FORWARD");
        updatePosition(currentDir);
        flood();
        return FORWARD;
    } else {
        debug_log("Front wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'F'));
        printWalls();
    }

    // 3. Check Right
    if (rightOpen) {
        debug_log("Turning RIGHT");
        int newDir = getTrueDirection(currentDir, 'R');
        currentDir = newDir;
        updatePosition(currentDir);
        flood();
        return RIGHT;
    } else {
        debug_log("Right wall detected");
        updateWalls(mouseX, mouseY, getTrueDirection(currentDir, 'R'));
        printWalls();
    }

    // If stuck, return IDLE instead of forcing a movement
    debug_log("No moves available! Staying IDLE.");
    API_turnLeft();
    currentDir = getTrueDirection(currentDir, 'L');
    currentDir = getTrueDirection(currentDir, 'L');
    updatePosition(currentDir);
    return LEFT;
}


// if (mouseX > 0){
//         west = grid[mouseY][mouseX-1].distanceVal;
//         min = west;
//     }
//     if (mouseX < SIZE-1){
//         east = grid[mouseY][mouseX+1].distanceVal;
//         if (east < min)
//             min = east;
//     }
//     if (mouseY > 0){
//         north = grid[mouseY-1][mouseX].distanceVal;
//         if (north < min)
//             min = north;
//     }
//     if (mouseY < SIZE-1){
//         south = grid[mouseY+1][mouseX].distanceVal;
//         if (south < min)
//             min = south;
//     }

//     if 