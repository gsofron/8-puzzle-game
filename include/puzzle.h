/* Interface file puzzle.h */

#ifndef PUZZLE_H
#define PUZZLE_H

#include <stdbool.h>

#define N 3 // board size is 3X3

#define NULLkey -1 // NULLkey

// pointer to the priority queue struct
typedef struct priority_queue *PriorityQueue;

// definitions of possible actions
typedef enum{
    UP, RIGHT, DOWN, LEFT
}Movement;

// state struct
typedef struct state_struct{
    int **s; // current state / board NxN
    Movement Move; // move that resulted in this state
}state;

// returns true/false depending on if states are equal
bool is_state_same(state *, state *);

// allocates memory for a state
void initialize_state(state *);

// creates a new child-state based on move gievn, if possible
state *new_move(state *, Movement);

// checks if puzzle is solvable
bool is_puzzle_solvable(state *);

// solve given puzzle using A* algorithm, returns the path array
Movement *puzzle_solve(state *, state *, int *);

#endif