#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"

extern int winner;
extern int startup_counter;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model, Sound splash, Sound fire);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model, Sound splash, Sound fire);

#endif //GAME_H
