#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "raylib.h"
#include "puzzle.h"
#include "PriorityQueue.h"
#include "interface.h"

// window size
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450

// puzzle tile size
#define IMAGE_SIZE 133

// current window mode
Mode MODE;

//////////////////////////////////////////////////////////////////
//                                                              //
//                       DRAW FUNCTIONS                         //
//                                                              //
//////////////////////////////////////////////////////////////////

// used to draw buttons' outlines when mouse is on them
static bool is_mouse_on_button(void)
{
    // find mouse coordinates
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    // check if mouse was on a button of a screen
    if (MODE == WELCOME){
        if (mouse_x >= 75 && mouse_x <= 375 && mouse_y >= 130 && mouse_y <= 210){
            DrawRectangleLines(74, 129, 300, 80, BLACK);
            DrawRectangleLines(73, 128, 300, 80, BLACK);
            DrawRectangleLines(72, 127, 300, 80, BLACK);
            return true;
        }
        else if (mouse_x >= 425 && mouse_x <= 725 && mouse_y >= 130 && mouse_y <= 210){
            DrawRectangleLines(424, 129, 300, 80, BLACK);
            DrawRectangleLines(423, 128, 300, 80, BLACK);
            DrawRectangleLines(422, 127, 300, 80, BLACK);
            return true;
        }
        else if (mouse_x >= 250 && mouse_x <= 550 && mouse_y >= 250 && mouse_y <= 330){
            DrawRectangleLines(249, 249, 300, 80, BLACK);
            DrawRectangleLines(248, 248, 300, 80, BLACK);
            DrawRectangleLines(247, 247, 300, 80, BLACK);
            return true;
        }
    }else{
        if (mouse_x >= 10 && mouse_x <= 46 && mouse_y >= 10 && mouse_y <= 30){
            DrawRectangleLines(9, 9, 36, 20, BLACK);
            DrawRectangleLines(8, 8, 36, 20, BLACK);
            return true;
        }
    }
    return false;
}

// draw the WELCOME mode window
static void draw_welcome(int *framesCounter)
{
    if (*framesCounter > 300) *framesCounter = 0;
    else (*framesCounter)++;
    DrawText("The 8-Puzzle Game!", 180, 30, 45, BLACK);
    DrawText(TextSubtext("SELECT PREFERED OPTION", 0, (*framesCounter)/10), 250, 90, 20, GRAY);
    DrawRectangle(75, 130, 300, 80, RAYWHITE);
    DrawText("Solve the Puzzle", 95, 155, 30, BLACK);
    DrawRectangleLines(75, 130, 300, 80, BLACK);
    DrawRectangle(425, 130, 300, 80, RAYWHITE);
    DrawText("A.I. Solver", 495, 155, 30, BLACK);
    DrawRectangleLines(425, 130, 300, 80, BLACK);
    DrawRectangle(250, 250, 300, 80, RAYWHITE);
    DrawText("EXIT", 360, 275, 35, BLACK);
    DrawRectangleLines(250, 250, 300, 80, BLACK);
}

static char *int_to_str(int);

// draw given puzzle with moves and appropriate messages based on if solved
static void draw_puzzle(state *puzzle, Texture2D *textures, int moves, state *goal)
{
    int key; // key used to determine current element of the puzzle
    Texture2D Key;
    int x, y = 20;

    // draw the goal puzzle state
    DrawText("Goal Puzzle:", 100, 125, 25, BLACK);
    DrawTexture(textures[9], 115, 160, RAYWHITE);

    for (int i = 0; i < N; i++){
        if (i > 0) y += IMAGE_SIZE;
        x = 360;
        for (int j = 0; j < N; j++){
            key = puzzle->s[i][j];
            if (key != NULLkey) Key = textures[key];
            else Key = textures[0];
            DrawTexture(Key, x, y, RAYWHITE);
            x += IMAGE_SIZE;
        }
    }
    if (moves != -1){
        DrawText("MOVES: ", 80, 55, 40, BLACK);
        char str[N];
        strcpy(str, int_to_str(moves));
        DrawText(str, 255, 55, 40, BLACK);
        if (is_state_same(puzzle, goal)){
            DrawText("SOLVED!", 110, 300, 30, BLACK);
            DrawText("PRESS ENTER TO EXIT", 20, 350, 28, DARKGRAY);
        }else{
            if (MODE == MANUAL){
                DrawText("Instructions:", 16, 315, 25, BLACK);
                DrawText("- Press the arrow keys to move", 14, 355, 20, BLACK);
                DrawText("  a tile in the blank space", 14, 380, 20, BLACK);
            }
            else if (MODE == AI){
                DrawText("Instructions:", 16, 315, 25, BLACK);
                DrawText("- Press ENTER to generate", 14, 355, 20, BLACK);
                DrawText("  the next optimal move", 14, 380, 20, BLACK);
            }
        }
    }
}

// draw a puzzle's outline/board and the back <- arrow key
static void draw_puzzle_board(void)
{
    // print puzzle's board
    DrawRectangleLines(359, 19, 401, 401, BLACK);
    DrawLine(360, 20+IMAGE_SIZE, 360+400, 20+IMAGE_SIZE, BLACK);
    DrawLine(360, 20+266, 360+400, 20+266, BLACK);
    DrawLine(360+IMAGE_SIZE, 20, 360+IMAGE_SIZE, 20+400, BLACK);
    DrawLine(360+266, 20, 360+266, 20+400, BLACK);
    // go back button
    DrawRectangle(10, 10, 36, 20, RAYWHITE);
    DrawRectangleLines(10, 10, 36, 20, BLACK);
    DrawText("<-", 17, 9, 25, BLACK);
}

// draws the initial/puzzle-scan window
static void draw_input_window(state *initial, Texture2D *textures)
{
    DrawText("Insert the starting Puzzle:", 20, 70, 25, BLACK);
    DrawText("Instructions:", 16, 315, 25, BLACK);
    DrawText("- Press SPACE for the blank tile", 14, 355, 20, BLACK);
    DrawText("- Press 1-8 for the number-tiles", 14, 380, 20, BLACK);
    draw_puzzle(initial, textures, -1, NULL);
    draw_puzzle_board();
}

//////////////////////////////////////////////////////////////////
//                                                              //
//                      OTHER FUNCTIONS                         //
//                                                              //
//////////////////////////////////////////////////////////////////

// string_reverses given string
static void string_reverse(char *s)
{
   char *begin, *end, temp;
   int length = strlen(s);
   begin = s;
   end = s;
   for (int c = 0; c < length-1; c++){
       end++;
   }
   for (int c = 0; c < length/2; c++){        
       temp = *end;
       *end = *begin;
       *begin = temp;
       begin++;
       end--;
   }
}
 
// Implementation of i->a (int to string)
static char *int_to_str(int num)
{
    int i = 0;
    char *str = malloc(sizeof(char)*3);
    assert(str);
    // Handle 0 explicitly, otherwise empty string is printed for 0
    if (num == 0){
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }
    // Process individual digits
    while (num != 0){
        int rem = num % 10;
        str[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        num /= 10;
    }
    str[i] = '\0'; // Append string terminator
    string_reverse(str);
    return str;
}

// check if mouse clicked on the specific (rectangle) coordinates
static bool check_mouse_pressed(int x1, int x2, int y1, int y2)
{
    // find mouse coordinates
    int mouse_x = GetMouseX();
    int mouse_y = GetMouseY();
    // check if mouse clicked when there
    if (mouse_x >= x1 && mouse_x <= x2 && mouse_y >= y1 && mouse_y <= y2){
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            return true;
        }
    }
    return false;
}

// check if mouse pressed a screen button; change screen mode
static Mode mouse_clicked_button(void)
{
    if (MODE == WELCOME){
        // check if mouse clicked a welcome-screen button
        if (check_mouse_pressed(75, 375, 130, 210)) return MANUAL;
        if (check_mouse_pressed(425, 725, 130, 210)) return AI;
        if (check_mouse_pressed(250, 550, 250, 330)) return EXIT;
    }else{
        // check if mouse clicked the back (<-) arrow button
        if (check_mouse_pressed(10, 46, 10, 30)) return WELCOME;
    }
    return MODE;
}

// checks if input move is legal
static bool is_move_legal(state *puzzle, Movement move)
{
    int iloc = -1, jloc = -1;
    // find zero's coordinates
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (!puzzle->s[i][j]){
                iloc = i;
                jloc = j;
                break;
            }
        }
        if (iloc != -1) break;
    }
    if (move == DOWN){
        if (iloc != 2) return true;
    }
    else if (move == UP){
        if (iloc) return true;
    }
    else if (move == LEFT){
        if (jloc) return true;
    }
    else if (move == RIGHT){
        if (jloc != 2) return true;
    }
    return false;
}

// checks if an arrow key has been pressed and if the move is legal, make the new move
// used for manual puzzle solve
int check_key_pressed(state **puzzle)
{
    Movement move = -1;
    if (IsKeyReleased(KEY_UP)){
        move = DOWN;
    }
    else if (IsKeyReleased(KEY_DOWN)){
        move = UP;
    }
    else if (IsKeyReleased(KEY_RIGHT)){
        move = LEFT;
    }
    else if (IsKeyReleased(KEY_LEFT)){
        move = RIGHT;
    }

    if ((int)move != -1){
        if (is_move_legal(*puzzle, move)){
            *puzzle = new_move(*puzzle, move); 
            return 1;
        }else{
            return -1; // doing this to play audio "error"
        }
    }
    return 0;
}

// manual user play
state *solve_puzzle_manual(state *puzzle, int *moves)
{
    (*moves) += check_key_pressed(&puzzle);
    return puzzle;
}

// scan the puzzle through keyboard keys
static state *puzzle_scan(state *puzzle, bool *inserted)
{
    // check users input
    int Key;
    if (IsKeyReleased(KEY_SPACE)) Key = 0;
    else if (IsKeyReleased(KEY_ONE)) Key = 1;
    else if (IsKeyReleased(KEY_TWO)) Key = 2;
    else if (IsKeyReleased(KEY_THREE)) Key = 3;
    else if (IsKeyReleased(KEY_FOUR)) Key = 4;
    else if (IsKeyReleased(KEY_FIVE)) Key = 5;
    else if (IsKeyReleased(KEY_SIX)) Key = 6;
    else if (IsKeyReleased(KEY_SEVEN)) Key = 7;
    else if (IsKeyReleased(KEY_EIGHT)) Key = 8;
    else return puzzle;

    // check if given number already exists in the puzzle
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (puzzle->s[i][j] == Key) return puzzle;
        }
    }

    // insert Key in the puzzle
    for (int i = 0; i < N; i++){
        for (int j = 0; j < N; j++){
            if (puzzle->s[i][j] == NULLkey){
                puzzle->s[i][j] = Key;
                Key = NULLkey;
                break;
            }
        }
        if (Key == NULLkey) break;
    }
    *inserted = true;
    return puzzle;
}

// starts the game loop
void start_gameplay(state *initial, state *goal)
{
    // initiallize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "8-Puzzle Game");
    SetWindowState(FLAG_VSYNC_HINT);

    // initialize audio
    InitAudioDevice();
    // load sounds
    Sound tile_move = LoadSound("./assets/tile_move.wav");
    Sound click = LoadSound("./assets/click.wav");
    Sound error = LoadSound("./assets/error.wav");
    Sound touch = LoadSound("./assets/touch.wav");

    // load images
    // make an array with the textures
    Texture2D *textures = malloc(10 * sizeof(Texture2D));
    Image image = LoadImage("./assets/0.png");   
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);  
    textures[0] = LoadTextureFromImage(image); 
    UnloadImage(image);
    image = LoadImage("./assets/1.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[1] = LoadTextureFromImage(image); 
    UnloadImage(image);
    image = LoadImage("./assets/2.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[2] = LoadTextureFromImage(image); 
    UnloadImage(image);
    image = LoadImage("./assets/3.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[3] = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("./assets/4.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[4] = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("./assets/5.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[5] = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("./assets/6.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[6] = LoadTextureFromImage(image); 
    UnloadImage(image);
    image = LoadImage("./assets/7.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[7] = LoadTextureFromImage(image); 
    UnloadImage(image);
    image = LoadImage("./assets/8.png");     
    ImageResize(&image, IMAGE_SIZE, IMAGE_SIZE);
    textures[8] = LoadTextureFromImage(image);
    UnloadImage(image);
    image = LoadImage("./assets/goal-puzzle-state.png");     
    ImageResize(&image, IMAGE_SIZE/2+50, IMAGE_SIZE/2+50);
    textures[9] = LoadTextureFromImage(image);
    UnloadImage(image);

    // puzzle scan initializations
    int inserted_elements = 0;
    bool inserted = false;
    bool unsolvable = false;

    // WELCOME window initializations
    int framesCounter = 0;

    // AI window initializations
    int moves = -1;
    int goal_moves, path_counter;
    Movement *path = NULL;
    state *puzzle = initial;

    // MANUAL window initalizations
    state *puzzle_manual = initial;
    int moves_manual = 0;
    int final_user_moves = moves_manual;

    // starting mode is WELCOME
    MODE = WELCOME;
    // old_mode is used to generate sound when MODE is changed
    Mode old_mode;

    // is mouse on button check to play sound when on-button
    bool old_current = false;
    bool new_current = false;

    // start main game loop
    while(!WindowShouldClose()){

        // base case to exit, in case of unsolvable puzzle
        if (unsolvable && IsKeyReleased(KEY_ENTER)) MODE = EXIT;

        // check if should exit
        if (MODE == EXIT) break;

        // check if mouse pressed a button; MODE changed
        old_mode = MODE;
        MODE = mouse_clicked_button();
        if (old_mode != MODE) PlaySound(click);

        if (inserted_elements >= 9 && !unsolvable){
            // final user moves = -1 => puzzle has been solved
            if (MODE == MANUAL && final_user_moves == -1){
                if (IsKeyReleased(KEY_ENTER)) MODE = EXIT;
            }
            if (MODE == MANUAL && final_user_moves != -1){
                int old_m = moves_manual;
                puzzle_manual = solve_puzzle_manual(puzzle_manual, &moves_manual);
                if (old_m < moves_manual) PlaySound(tile_move); // move made
                if (old_m > moves_manual){ // illegal move
                    moves_manual++;
                    PlaySound(error);
                }

                // check if puzzle is solved
                if (is_state_same(puzzle_manual, goal)) final_user_moves = -1;
            }

            if (MODE == AI && moves == -1){
                moves = 0;
                // solve the puzzle and save the path in an int-array
                path = puzzle_solve(initial, goal, &goal_moves);
                path_counter = goal_moves;
            }
            else if (MODE == AI && moves != -1){
                // base case; initial puzzle is the goal one
                if (IsKeyReleased(KEY_ENTER)){
                    if (path == NULL) break; // initial puzzle is the goal one
                    // generate next AI move
                    PlaySound(tile_move);
                    puzzle = new_move(puzzle, path[--path_counter]);
                    moves++;
                    if (path_counter < 0) MODE = EXIT;
                }
            }
        }
//------------------------------------------------------------------------------------//
        // drawing
        BeginDrawing();
            // basic drawings for all window-cases
            ClearBackground(LIGHTGRAY);
            DrawText("(c) Giorgos Sofronas", SCREEN_WIDTH-115, SCREEN_HEIGHT-15, 10, BLACK);

            // check which window should be displayed
            if (MODE == WELCOME){
                draw_welcome(&framesCounter);
            }

            // still inserting initial puzzle
            if (inserted_elements < 9){
                if (MODE == MANUAL || MODE == AI){
                    initial = puzzle_scan(initial, &inserted);
                    if (inserted){
                        inserted_elements++;
                        inserted = false;
                        // check if puzzle is solvable
                        if (inserted_elements == 9){
                            if (!is_puzzle_solvable(initial)){
                                unsolvable = true;
                                inserted_elements++;
                            }
                        }
                    }
                    draw_input_window(initial, textures);
                }
            }else if (inserted_elements >= 9 && !unsolvable){
                // draw current puzzle state
                if (MODE == AI){
                    draw_puzzle_board();
                    draw_puzzle(puzzle, textures, moves, goal);
                }
                else if (MODE == MANUAL){
                    draw_puzzle_board();
                    draw_puzzle(puzzle_manual, textures, moves_manual, goal);
                }
            }else{
                if (unsolvable){
                    DrawText("Given Puzzle is Unsolvable...", SCREEN_WIDTH/2-205, SCREEN_HEIGHT/2-50, 30, BLACK);
                    DrawText("PRESS ENTER TO EXIT", SCREEN_WIDTH/2-154, SCREEN_HEIGHT/2-10, 28, DARKGRAY);
                }
            }

            // check if mouse is on a button; draw its outlines
            old_current = new_current;
            new_current = is_mouse_on_button();
            if (old_current == false && new_current == true) PlaySound(touch);

        EndDrawing();
    }
    // free used memory
    if (MODE == AI && path != NULL) free(path);
    // unload textures
    for (int i = 0; i < 9; i++){
        UnloadTexture(textures[i]);
    }
    free(textures);
    // unload sounds
    UnloadSound(tile_move);
    UnloadSound(click);
    UnloadSound(touch);
    UnloadSound(error);
    CloseAudioDevice();
    // close the window
    CloseWindow();
}