#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "puzzle.h"
#include "PriorityQueue.h"
#include "interface.h"

// state node struct
typedef struct state_node StateNode;
struct state_node{
    state *State; // pointer to previous struct
    int g; // depth of current node, AKA moves made from initial to current state
    StateNode *Parent; // pointer to parent state
    StateNode *child[4]; // array of pointers to child-states
};

// simple linked list used to free memory
typedef struct list_{
    StateNode *item;
    struct list_ *next;
}List;

extern Mode MODE;

#define ABS(A, B) ((A>B) ? A-B : B-A) // absolute value of A-B

// manhantan distance heuristic
// sum of the manhanttan distances of each tile from its goal position 
static int h(state *k)
{
    int goal_value = 1; // variable used to represent/"create" goal state (tiles in ascending order)
    int manh = 0; // sum of manhanttan distances of misplaced tiles from their right coordinates in a goal state
    int misplaced_tiles = 0; // number of misplaced tiles
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (k->s[i][j] == 0){
                continue;
            }
            // calculate coordinates of k->s[i][j] in the goal state
            int x_goal = k->s[i][j]/N;
            int y_goal = k->s[i][j]%N;
            // current k->s[i][j] coordinates
            int x_value = i;
            int y_value = j;
            // add to the sum of manhanttan distances
            manh += ABS(x_goal, x_value) + ABS(y_goal, y_value);
            if (k->s[i][j] != goal_value){
                misplaced_tiles++;
            }
            goal_value++;
        }
    }
    if (misplaced_tiles == 0){
        return 0; // state k is the goal one
    }
    return manh;
}

// return true/false depending on if states match
bool is_state_same(state *one, state *two)
{
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (one->s[i][j] != two->s[i][j]){
                return false;
            }
        }
    }
    return true;
}

// frees allocated memory of a state
static void free_state(state *k)
{
    for (int i = 0; i < N; i++){
        free(k->s[i]);
    }
    free(k->s);
}

// returns a state pointer to a new created state
void initialize_state(state *k)
{
    k->s = malloc(N * sizeof(int *));
    assert(k->s);
    for (int i = 0; i < N; i++){
        k->s[i] = malloc(N * sizeof(int));
        assert(k->s[i]);
    }
} 

// returns a StateNode pointer to a new created StateNode
static StateNode *initialize_state_node(StateNode *parent, state *k, int cost)
{
    StateNode *new = malloc(sizeof(StateNode));
    assert(new);

    new->State = k; // copies to new state both the array and move
    new->g = cost;
    new->Parent = parent; // connect new state with the parent one

    for (int i = 0; i < N; i++){
        new->child[i] = NULL; // initialize child-states with NULL
    }
    return new;
}

// generates a new state, from current based on newMove
state *new_move(state *current, Movement newMove)
{
    // find zero's coordinates
    int iloc = -1, jloc = -1;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (current->s[i][j] == 0){
                // found zero's coordinates
                iloc = i;
                jloc = j;
                break;
            }
        }
        if (iloc != -1) break;
    }
    // allocate memory for new state
    state *new = malloc(sizeof(state));
    assert(new);
    initialize_state(new);
    // copy current state in new state
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            new->s[i][j] = current->s[i][j];
        }
    }
    // save previous move that lead to current state to avoid moving zero back and forth
    int previous_move = current->Move;

    if (MODE == MANUAL) previous_move = -1; // let user manually move back and forth

    // check if coordinates are valid to execute newMove
    if (newMove == UP && iloc-1 >= 0 && previous_move != DOWN){
        new->s[iloc][jloc] = new->s[iloc-1][jloc];
        new->s[iloc-1][jloc] = 0;
        new->Move = UP;
        return new;
    }
    else if (newMove == RIGHT && jloc+1 < N && previous_move != LEFT){
        new->s[iloc][jloc] = new->s[iloc][jloc+1];
        new->s[iloc][jloc+1] = 0;
        new->Move = RIGHT;
        return new;
    }
    else if (newMove == DOWN && iloc+1 < N && previous_move != UP){
        new->s[iloc][jloc] = new->s[iloc+1][jloc];
        new->s[iloc+1][jloc] = 0;
        new->Move = DOWN;
        return new;
    }
    else if (newMove == LEFT && jloc-1 >= 0 && previous_move != RIGHT){
        new->s[iloc][jloc] = new->s[iloc][jloc-1];
        new->s[iloc][jloc-1] = 0;
        new->Move = LEFT;
        return new;
    }
    free_state(new);
    free(new);
    return NULL;
}

// expands the state
static void expand_state(StateNode *parent, PriorityQueue pq)
{
    StateNode *child = NULL; // new child
    // try to create child-states for each possible move
    // if created, connect child with parent
    // and insert in priority queue
    state *test_child_state;
    // UP
    test_child_state = new_move(parent->State, UP);
    if (test_child_state != NULL){
        // child has been created
        child = initialize_state_node(parent, test_child_state, (parent->g)+1);
        parent->child[UP] = child;
        pq_insert(child, pq);
    }
    // RIGHT
    test_child_state = new_move(parent->State, RIGHT);
    if (test_child_state != NULL){
        // child has been created
        child = initialize_state_node(parent, test_child_state, (parent->g)+1);
        parent->child[RIGHT] = child;
        pq_insert(child, pq);
    }
    // DOWN
    test_child_state = new_move(parent->State, DOWN);
    if (test_child_state != NULL){
        // child has been created
        child = initialize_state_node(parent, test_child_state, (parent->g)+1);
        parent->child[DOWN] = child;
        pq_insert(child, pq);
    }
    // LEFT
    test_child_state = new_move(parent->State, LEFT);
    if (test_child_state != NULL){
        // child has been created
        child = initialize_state_node(parent, test_child_state, (parent->g)+1);
        parent->child[LEFT] = child;
        pq_insert(child, pq);
    }
}

// checks if given puzzle is solvable
bool is_puzzle_solvable(state *k)
{
    int *temp = malloc(9 * sizeof(int));
    assert(temp);
    int c = 0;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            temp[c++] = k->s[i][j];
        }
    }
    int inv_count = 0;
    for (int i = 0; i < 9 - 1; i++){
        for (int j = i+1; j < 9; j++){
            // Value 0 is used for empty space
            if (temp[j] && temp[i] &&  temp[i] > temp[j]){
                inv_count++;
            }
        }
    }
    free(temp);
    if (inv_count % 2 == 1) return false;
    else return true;
}

// destroys given puzzle
void destroy_puzzle(Pointer puzzle)
{
    StateNode *p = puzzle;
    free_state(p->State);
    free(p->State);
    free(p);
}

// compares 2 puzzles based on their g+h evaluation
int compare_puzzles(Pointer puzzle1, Pointer puzzle2)
{
    StateNode *p1 = puzzle1;
    StateNode *p2 = puzzle2;
    int f1 = p1->g + h(p1->State);
    int f2 = p2->g + h(p2->State);
    return (f1 - f2);
}

// solve given puzzle using A* algorithm; returns the optimal path
Movement *puzzle_solve(state *initial, state *goal, int *goal_moves)
{
    // used to calculate g()
    int cost = 0;

    // initialize the priority queue
    PriorityQueue pq;
    pq_initialize(&pq, compare_puzzles, destroy_puzzle);

    // list with solution
    StateNode *current = initialize_state_node(NULL, initial, 0);
    current->g = 0;

    // insert initial state in PQ
    pq_insert(current, pq);

    // create list used to free memory later
    List *list = malloc(sizeof(List));
    assert(list);
    List *head = list; // pointer to the first list node

    // current state removed
    // while there are no nodes to expand 
    while (!pq_is_empty(pq)){
        // pop state with highest priority and add it to the list
        current = pq_remove(pq);
        // put item in list
        list->item = current;
        list->next = malloc(sizeof(List));
        assert(list->next);
        list = list->next;
        // if current is goal state
        if (is_state_same(current->State, goal)){ 
            break;
        }
        cost++;
        // current state isn't the goal one
        // expand current state and insert child-states in PQ
        expand_state(current, pq);
    }
    pq_destroy(pq);
    // print solution
    if (cost == 0){
        // initial state is the goal one
        return NULL;
    } else {
        Movement *path = malloc((cost+1) * sizeof(Movement));
        assert(path);
        int i = 0;
        while (current->Parent != NULL){
            path[i++] = current->State->Move;
            current = current->Parent;
        }
        path[i] = initial->Move;
        // path is filled with correct moves, print it
        *goal_moves = i;
        // free memory of nodes in list
        List *tmp;
        while (head->next != list){
            tmp = head->next;
            free_state(tmp->item->State);
            free(head->item);
            free(head);
            head = tmp;
        }
        return path;
    }
}