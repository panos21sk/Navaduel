#ifndef SETTINGS_H
#define SETTINGS_H

#define WIDTH 1280
#define HEIGHT 600

typedef enum
{
    MAIN, //entry screen
    OPTIONS, //options
    GAME, //main game screen
    GAME_OVER, //game over screen - when a ship gets destroyed
    ABOUT // credits and basic gameplay
} screen;

#endif //SETTINGS_H
