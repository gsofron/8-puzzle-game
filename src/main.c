#include "puzzle.h"
#include "interface.h"

int main(void)
{
    // initialize the initial and the goal state 
    state goal; 
    goal.Move = -1;
    initialize_state(&goal);
    state initial;
    initialize_state(&initial);
    initial.Move = -1;
    // fill initial with NULLkey and goal state appropriately
    int c = 0;
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            initial.s[i][j] = NULLkey;
            goal.s[i][j] = c++;
        }
    }
    // start the game loop
    start_gameplay(&initial, &goal);
    return 0;
}