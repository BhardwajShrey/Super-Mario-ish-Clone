#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include "Main.h"


void init_status_lives(GameState* game)
{
	char str[128] = "";

	sprintf(str, "x %d", (int)game->man.lives);

	SDL_Color white = { 255, 255, 255, 255 };
	SDL_Surface* tmp = TTF_RenderText_Blended(game->font, str, white);
	game->labelW = tmp->w;
	game->labelH = tmp->h;
	game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
	SDL_FreeSurface(tmp);

	SDL_Color lightBlue = { 128, 192, 255, 255 };
	tmp = TTF_RenderText_Blended(game->font, "Get to the end alive!", lightBlue);
	game->label2W = tmp->w;
	game->label2H = tmp->h;
	game->label2 = SDL_CreateTextureFromSurface(game->renderer, tmp);
	SDL_FreeSurface(tmp);
}

void draw_status_lives(GameState* game)
{
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

	// clear screen (to black)
	SDL_RenderClear(game->renderer);

	SDL_Rect rect = { 320-70, 240-24, 48, 48 };	// {x-coord of top left corner, y-coord of top left corner, width, height}
	SDL_RenderCopyEx(game->renderer, game->manFrames[0], NULL, &rect, 0, NULL, game->man.facingLeft == 0);

	SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

	SDL_Rect textRect = { 320-20, 240 - game->labelH/2, game->labelW, game->labelH };
	SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);

	SDL_Rect textRect2 = { 320 - game->label2W / 2, 100 - game->label2H / 2, game->label2W, game->label2H };
	SDL_RenderCopy(game->renderer, game->label2, NULL, &textRect2);
}

void shutdown_status_lives(GameState* game)
{
	SDL_DestroyTexture(game->label);
	game->label = NULL;

	SDL_DestroyTexture(game->label2);
	game->label2 = NULL;
}

void init_game_over(GameState* game)
{
	//Create label textures for status screen
	SDL_Color white = { 255, 255, 255, 255 };

	SDL_Surface* tmp = TTF_RenderText_Blended(game->font, "GAME OVER!", white);
	game->labelW = tmp->w;
	game->labelH = tmp->h;
	game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
	SDL_FreeSurface(tmp);
}

void draw_game_over(GameState* game)
{
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

	//Clear the screen
	SDL_RenderClear(game->renderer);

	//set the drawing color to white
	SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);

	SDL_Rect textRect = { 320 - game->labelW / 2, 240 - game->labelH / 2, game->labelW, game->labelH };
	SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);
}

void shutdown_game_over(GameState* game)
{
	SDL_DestroyTexture(game->label);
	game->label = NULL;
}

void init_game_win(GameState* game)
{
	//Create label textures for status screen
	SDL_Color white = { 255, 255, 255, 255 };

	SDL_Surface* tmp = TTF_RenderText_Blended(game->font, "You live..", white);
	game->labelW = tmp->w;
	game->labelH = tmp->h;
	game->label = SDL_CreateTextureFromSurface(game->renderer, tmp);
	SDL_FreeSurface(tmp);
}

void draw_game_win(GameState* game)
{
	SDL_Renderer* renderer = game->renderer;
	SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);

	//Clear the screen
	SDL_RenderClear(game->renderer);

	//set the drawing color to white
	SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);

	SDL_Rect textRect = { 320 - game->labelW / 2, 240 - game->labelH / 2, game->labelW, game->labelH };
	SDL_RenderCopy(game->renderer, game->label, NULL, &textRect);
}

void shutdown_game_win(GameState* game)
{
	SDL_DestroyTexture(game->label);
	game->label = NULL;
}