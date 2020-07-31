#pragma once	
#ifndef Project1_Main_h
#define Project1_Main_h

#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>

#define STATUS_STATE_LIVES 0
#define STATUS_STATE_GAME 1
#define STATUS_STATE_GAMEOVER 2

#define NUM_STARS 100

typedef struct Man
{
	float x, y;
	float dx, dy; // change in man's x and y position per frame
	short lives;
	char* name;
	int onLedge;
	int animFrame, facingLeft, slowingDown, isDead;
} Man;

typedef struct Star
{
	int x, y;
} Star;

typedef struct Ledge
{
	int x, y, w, h;
} Ledge;

typedef struct GameState
{
	float scrollX;

	// players
	Man man;

	// stars
	Star stars[NUM_STARS];

	// ledges
	Ledge ledges[100];

	// images
	// in 3d graphics hardware images are called textures. this is the hardware accelerated version of the surface
	SDL_Texture* star;
	SDL_Texture* manFrames[2];
	SDL_Texture* brick;
	SDL_Texture* label, *label2; // will contain rendered string on status screen
	SDL_Texture* fire;	// fire animation when man dies

	int labelW, labelH, label2W, label2H;

	// font
	TTF_Font* font;

	int time, deathCountdown;
	int statusState; // 0 = starting 2 secs of game. 1 = inside the game. 2 = game over

	// sounds
	int musicChannel;
	Mix_Chunk* bgMusic, * jumpSound, * landSound, * dieSound;

	SDL_Renderer* renderer;
} GameState;

#endif // !Project1_Main_h
