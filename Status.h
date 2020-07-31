#pragma once

#ifndef Project1_Status_h
#define Project1_Status_h

#include<stdio.h>
#include<SDL.h>

void init_status_lives(GameState* game);
void draw_status_lives(GameState* game);
void shutdown_status_lives(GameState* game);

void init_game_over(GameState* game);
void draw_game_over(GameState* game);
void shutdown_game_over(GameState* game);

void init_game_win(GameState* game);
void draw_game_win(GameState* game);
void shutdown_game_win(GameState* game);

#endif
