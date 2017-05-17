/*******************************************************************************************
*
* im trying to make a game fml
*
********************************************************************************************/

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//#define PLATFORM_WEB
#define DEBUG

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Some Defines
//----------------------------------------------------------------------------------

#define PLAYER_MAX_LIFE         5
#define FOOD_INTERVAL           1
#define FOOD_AMOUNT             50

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum GameScreen { LOGO, TITLE, GAMEPLAY, ENDING } GameScreen;

typedef struct Player {
    Vector2 position;
    Vector2 size;
    int life;
} Player;

typedef struct Food {
    Vector2 position;
    float   radius;
    bool shouldBeDrawn;
    int value;
} Food;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int  screenWidth = 800;
static int  screenHeight = 450;

static int  framesCounter;
static bool gameOver;
static bool pause;

static Food foodArray[50];
//static bool timeOut;

static Player player;

static clock_t startTime;



// TODO: Represent food as an array of circles or an array of Vector2s.
//       We can then iterate over the array in GameUpdate(), update the shouldBeDrawn property each Timer() iteration,
//       and in DrawGame draw each food if it's shouldBeDrawn property is true.

//------------------------------------------------------------------------------------
// Module Functions Declaration (local)
//------------------------------------------------------------------------------------
static void InitGame(void);         // Initialize game
static void UpdateGame(void);       // Update game (one frame)
static void DrawGame(void);         // Draw game (one frame)
static void UnloadGame(void);       // Unload game
static void UpdateDrawFrame(void);  // Update and Draw (one frame)


// Additional module functions
static void UpdateFood(void);
static void DrawFood(void);
static bool Timer(int seconds);

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "what should i name this piece of s##t game");
    
    InitGame();
    
#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        UpdateGame();
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        DrawGame();
        //----------------------------------------------------------------------------------
        
    }
#endif

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadGame();         // Unload loaded data (textures, sounds, models etc)
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void InitGame(void)
{
    // Initialize player
    player.position = (Vector2){ screenWidth/2, screenHeight - screenHeight/8 };
    player.size = (Vector2){ screenWidth/10, screenHeight/22.5 };
    player.life = PLAYER_MAX_LIFE;
    
    // Initialize food
    for (int i = 0; i < FOOD_AMOUNT; i++)
        foodArray[i] = (Food){ (Vector2){ rand() % 800, 394 }, rand() % 15, false, rand() % 10 };
    
    // Seed rand()
    srand (time(NULL));

}

// Update game (one frame)
void UpdateGame(void)
{
    if (!gameOver)
    {
        if (IsKeyPressed('P')) pause = !pause;
        
        if (!pause)
        {
            // Player Movement
            if (IsKeyDown(KEY_LEFT))                                  player.position.x -= 5;
            if ((player.position.x - player.size.x/2) <= 0)           player.position.x =  player.size.x/2;
            if (IsKeyDown(KEY_RIGHT))                                 player.position.x += 5;
            if ((player.position.x + player.size.x/2) >= screenWidth) player.position.x =  screenWidth - player.size.x/2;
            
            // Update food
            UpdateFood();
            
            if (player.life <= 0) gameOver = true;
        }
    }
    else
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            InitGame();
            gameOver = false;
        }
    }
}

// Draw game (one frame)
void DrawGame(void)
{
    BeginDrawing();
        ClearBackground(RAYWHITE);
        
        if (!gameOver)
        {
        #if defined(DEBUG)                                                                                                                     
            DrawText(FormatText("screen: %dx%d\nx: %f\ny: %f", screenWidth, screenHeight, player.position.x, player.position.y),  50, 70, 20, BLACK);
        #endif
            // Draw ground
            DrawRectangle(0, screenHeight - screenHeight/8 + (screenHeight - screenHeight/8)/36, screenWidth, screenHeight/9, GREEN);
            
            // Draw player bar
            DrawRectangle(player.position.x - player.size.x/2, player.position.y - player.size.y/2, player.size.x, player.size.y, BLACK);
            
            // Draw player lives
            for (int i = 0; i < player.life; i++) DrawRectangle(10 + 40*i, 10, 35, 10, LIGHTGRAY);
            
            
            // Draw food
            if (Timer(FOOD_INTERVAL))
            {
                static int foodIndex = -1;
                if (foodIndex >= FOOD_AMOUNT) foodIndex = -1;
                foodArray[foodIndex++].shouldBeDrawn = true;
                
                for (int i = 0; i <= foodIndex; i++)
                    if (foodArray[i].shouldBeDrawn == true) DrawCircle(foodArray[i].position.x, foodArray[i].position.y, foodArray[i].radius, RED);
            }
            
            // Handle pausing
            if (pause) DrawText("GAME PAUSED", screenWidth / 2 - MeasureText("GAME PAUSED", 40)/2, screenHeight/2 - 40, 40, GRAY);
        }
        else DrawText("PRESS [ENTER] TO PLAY AGAIN", screenWidth/2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20)/2, screenHeight/2 - 50, 20, GRAY);
        
    EndDrawing();
}

// Unload game variables
void UnloadGame(void)
{
    // TODO: Free game assets
}

// Update and draw (one frame)
void UpdateDrawFrame(void)
{
    UpdateGame();
    DrawGame();
}    

//--------------------------------------------------------------------------------------
// Additional module functions
//--------------------------------------------------------------------------------------
void UpdateFood(void)
{
    static int foodIndex = -1;
    if (foodIndex > FOOD_AMOUNT) foodIndex = -1;
    foodArray[foodIndex++].shouldBeDrawn = true;
}

/// Version with static variable
bool Timer(int seconds)
{
    static clock_t startTime = (clock_t) -1;
    
    if (startTime == -1) 
    {
        startTime = clock();
        return false;
    }
    
    else if (((clock() - startTime) / CLOCKS_PER_SEC) > seconds)
    {
        startTime = clock();
        return true;
    }
    
    return false;
}

/// Version with global variable
// static void Timer(void (*func)(void), int seconds)
// {               
        // if (startTime == -1) 
        // {
            // startTime = clock();
        // }
        // else if (((startTime - clock()) / CLOCKS_PER_SEC) > seconds)
        // {
            // startTime = clock();
            // (*func)();
        // }
// }