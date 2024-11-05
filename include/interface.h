#ifndef INTERFACE_H
#define INTERFACE_H

// window mode enum
typedef enum{
    WELCOME, AI, MANUAL, EXIT
}Mode;

// starts the game loop
void start_gameplay(state *, state *);

#endif