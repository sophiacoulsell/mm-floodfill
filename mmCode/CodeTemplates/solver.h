#ifndef SOLVER_H
#define SOLVER_H



typedef enum {
    north = 0,
    east = 1,
    south = 2,
    west = 3
} Direction;


typedef enum Heading {NORTH, EAST, SOUTH, WEST} Heading;
typedef enum Action {LEFT, FORWARD, RIGHT, IDLE} Action;



Action solver();
Action leftWallFollower();
Action floodFill();

#include <stdio.h>

#include "API.h"
#define SIZE 16

typedef struct {
    int row;
    int column;
    int distanceVal;
} cell;

typedef struct {
    int x, y;
} point;


void enqueue(int x, int y);
point dequeue(void);
int isQueueEmpty(void);

void init_coords(cell (*p)[SIZE]);
void reinit_distances(cell (*p)[SIZE]);
void flood(void);


#endif