#include<stdio.h>
#include<SDL.h>
#include<SDL_image.h>
#include<stdlib.h>
#include<time.h>
#include "Main.h"
#include"Status.h"

#define GRAVITY 0.2f

// steps to integrate SDL in your project: https://www.youtube.com/watch?v=tmGBhM8AEj8
// include and library option in Project Properties. and then linker

void initStars(GameState* game)
{
	game->stars[0].x = 480;
	game->stars[0].y = 100;

	for (int i = 1; i < NUM_STARS; i++)
	{
		// (rand() % (upper - lower + 1)) + lower
		game->stars[i].x = (rand() % 1000) + game->stars[i - 1].x;
		game->stars[i].y = rand() % 380;
		// fix positions so that no stars appear below or crash with a ledge
	}
}


void loadGame(GameState *game)
{
	SDL_Surface* surface = NULL;	// source pixels of the image in memory. used to create textures

	// load images and create rendering textures from them
	surface = IMG_Load("resources/stars.png");
	if (surface == NULL)
	{
		printf("Could not load star image.\n\n");
		SDL_Quit();
		exit(1);
	}

	game->star = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);
	// dont need surface once texture is created so memory can be freed 

	// walk_1 image
	surface = IMG_Load("resources/walk_1.png");
	if (surface == NULL)
	{
		printf("Could not load walk_1 image.\n\n");
		SDL_Quit();
		exit(1);
	}

	game->manFrames[0] = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	// walk_2 image
	surface = IMG_Load("resources/walk_2.png");
	if (surface == NULL)
	{
		printf("Could not load walk_2 image.\n\n");
		SDL_Quit();
		exit(1);
	}

	game->manFrames[1] = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	// brick wall image
	surface = IMG_Load("resources/wall.png");
	if (surface == NULL)
	{
		printf("Could not load wall image.\n\n");
		SDL_Quit();
		exit(1);
	}

	game->brick = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	// fire image
	surface = IMG_Load("resources/fire.png");
	if (surface == NULL)
	{
		printf("Could not load fire image.\n\n");
		SDL_Quit();
		exit(1);
	}

	game->fire = SDL_CreateTextureFromSurface(game->renderer, surface);
	SDL_FreeSurface(surface);

	// load fonts
	game->font = TTF_OpenFont("resources/Pixeboy.ttf", 48);
	if (!game->font)
	{
		printf("Could not load font.\n\n");
		SDL_Quit();
		exit(1);
	}

	srand(time(0));

	int tuneNo = rand() % 2;

	// load sounds
	if (tuneNo == 0)
	{
		game->bgMusic = Mix_LoadWAV("resources/bgmusic-wav.wav");
	}
	else if (tuneNo == 1)
	{
		game->bgMusic = Mix_LoadWAV("resources/bgmusic2-wav.wav");
	}

	if (game->bgMusic != NULL)
	{
		Mix_VolumeChunk(game->bgMusic, 100); // second parameter is volume of music (0 - 128)
	}
	

	game->jumpSound = Mix_LoadWAV("resources/jump.wav");
	game->landSound = Mix_LoadWAV("resources/land.wav");
	game->dieSound = Mix_LoadWAV("resources/die.wav");
	

	game->label = NULL;
	game->label2 = NULL;


	// init man's position
	game->man.x = 320 - 40;
	game->man.y = 240 - 40;
	game->man.dy = 0;
	game->man.dx = 0;
	game->man.onLedge = 0;
	game->man.animFrame = 0;
	game->man.facingLeft = 1;
	game->man.slowingDown = 0;
	game->man.lives = 3;
	game->statusState = STATUS_STATE_LIVES;
	game->man.isDead = 0;

	init_status_lives(game);

	game->scrollX = 0.0f;
	game->time = 0;
	game->deathCountdown = -1;

	// initialise stars
	initStars(game);
	
	// maximum jumping range is 512 px (2 ledges)
	// init ledges
	for (int i = 0; i < 100; i++)
	{
		game->ledges[i].w = 256;
		game->ledges[i].h = 64;
		game->ledges[i].x = i * 256;

		// (rand() % (upper - lower + 1)) + lower

		if (i >= 4)
		{
			//game->ledges[i].x = game->ledges[i - 1].x + 256 + ((i * 132 + i * i * 123) % 450); // make random number
			game->ledges[i].x = (rand() % 380) + game->ledges[i - 1].x + 256;
		}

		if (i < 4)
		{
			game->ledges[i].y = 400;
		}
		else
		{
			game->ledges[i].y = 400 - (rand() % 150);
		}
	}
	game->ledges[99].x = 350;
	game->ledges[99].y = 250;
}

void process(GameState* game)
{
	//add time
	game->time++;

	if (game->statusState == STATUS_STATE_LIVES)
	{
		if (game->time > 120)
		{
			shutdown_status_lives(game);
			game->statusState = STATUS_STATE_GAME;
			game->musicChannel = Mix_PlayChannel(-1, game->bgMusic, -1);
		}
	}
	else if (game->statusState == STATUS_STATE_GAMEOVER)
	{
		if (game->time > 190)
		{
			SDL_Quit();
			exit(0);
		}
	}
	else if (game->statusState == STATUS_STATE_GAME)
	{
		if (!game->man.isDead)
		{
			//man movement
			Man* man = &game->man;
			man->x += man->dx;
			man->y += man->dy;

			if (man->dx != 0 && man->onLedge && !man->slowingDown)
			{
				if (game->time % 8 == 0)
				{
					if (man->animFrame == 0)
					{
						man->animFrame = 1;
					}
					else
					{
						man->animFrame = 0;
					}
				}
			}

			/*if (man->x > 38320)
			{
				init_game_win(game);
				game->statusState = STATUS_STATE_WIN;
			}*/

			man->dy += GRAVITY;

			//Star movement
			/*for (int i = 0; i < NUM_STARS; i++)
			{
				game->stars[i].x = game->stars[i].baseX;
				game->stars[i].y = game->stars[i].baseY;

				if (game->stars[i].mode == 0)
				{
					game->stars[i].x = game->stars[i].baseX + sinf(game->stars[i].phase + game->time * 0.06f) * 75;
				}
				else
				{
					game->stars[i].y = game->stars[i].baseY + cosf(game->stars[i].phase + game->time * 0.06f) * 75;
				}
			}*/
		}

		if (game->man.isDead && game->deathCountdown < 0)
		{
			game->deathCountdown = 120;
		}
		if (game->deathCountdown >= 0)
		{
			game->deathCountdown--;
			if (game->deathCountdown < 0)
			{
				init_game_over(game);
				game->statusState = STATUS_STATE_GAMEOVER;

				game->man.lives--;

				if (game->man.lives >= 0)
				{
					init_status_lives(game);
					game->statusState = STATUS_STATE_LIVES;
					game->time = 0;

					//reset
					game->man.isDead = 0;
					game->man.x = 100;
					game->man.y = 240 - 40;
					game->man.dx = 0;
					game->man.dy = 0;
					game->man.onLedge = 0;
					initStars(game);
				}
				else
				{
					init_game_over(game);
					game->statusState = STATUS_STATE_GAMEOVER;
					game->time = 0;
				}
			}
		}
	}

	game->scrollX = -game->man.x + 320;
	if (game->scrollX > 0)
		game->scrollX = 0;
}


// useful utility function to see if 2 rectangles are colliding at all
int collide2d(float x1, float y1, float x2, float y2, float wt1, float ht1, float wt2, float ht2)
{
	return (!((x1 > (x2+wt2)) || (x2 > (x1+wt1)) || (y1 > (y2+ht2)) || (y2 > (y1+ht1))));
}

void collisionDetect(GameState* game)
{
	// iterate through all objects you are interested in colliding with

	for (int i = 0; i < NUM_STARS; i++)
	{
		if (collide2d(game->man.x, game->man.y, game->stars[i].x, game->stars[i].y, 48, 48, 28, 28))
		{
			if (!game->man.isDead)
			{
				game->man.isDead = 1;
				Mix_PlayChannel(-1, game->dieSound, 0);
				Mix_HaltChannel(game->musicChannel);
				break;
			}
		}
	}

	// check for falling
	if (game->man.y > 500)
	{
		if (!game->man.isDead)
		{
			game->man.isDead = 1;
			Mix_PlayChannel(-1, game->dieSound, 0);
			Mix_HaltChannel(game->musicChannel);
		};
	}

	// in this case the man cannot collide with the ledges so iterate through all ledges one by one
	// check for collision with any ledges (blocks)
	for (int i = 0; i < 100; i++)
	{
		float mw = 48, mh = 48;
		float mx = game->man.x, my = game->man.y;
		float bx = game->ledges[i].x, by = game->ledges[i].y, bw = game->ledges[i].w, bh = game->ledges[i].h;

		
		// left-right movement
		if (my + mh > by && my < by + bh)
		{
			// rubbing against right edge
			if (mx < bx + bw && mx + mw > bx + bw && game->man.dx < 0)
			{
				// correct x
				game->man.x = bx + bw;
				mx = bx + bw;
				game->man.dx = 0;
			}
			// rubbing against left edge
			else if (mx + mw > bx && mx < bx && game->man.dx > 0)
			{
				// correct x
				game->man.x = bx - mw;
				mx = bx - mw;
				game->man.dx = 0;
			}
		}
		

		if (mx + mw/2 > bx && mx + mw/2 < bx + bw)
		{
			// head bumping into ledge
			if (my < by + bh && my > by && game->man.dy < 0)
			{
				// correct y
				game->man.y = by + bh;
				my = by + bh;

				// bumped our head. stop any jump velocity
				game->man.dy = 0;
				game->man.onLedge = 1;
			}
		}

		if(mx + mw > bx && mx < bx + bw)
		{
			// landing on edge
			if (my + mh > by && my < by && game->man.dy > 0)
			{
				// correct y
				game->man.y = by - mh;

				//landed on ledge. stop any jump velocity
				game->man.dy = 0;
				if (!game->man.onLedge)
				{
					Mix_PlayChannel(-1, game->landSound, 0);
					game->man.onLedge = 1;
				}
			}
		}
	}
}

int processEvents(SDL_Window *window, GameState *game)
{
	SDL_Event event;
	int done = 0;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// when user closes window
			case SDL_WINDOWEVENT_CLOSE:
			{
				if (window)
				{
					SDL_DestroyWindow(window);
					window = NULL;
					done = 1;
				}
			}
			break;

			// when user presses key
			case SDL_KEYDOWN:
			{
				switch (event.key.keysym.sym)
				{
					// when user presses esc key
					case SDLK_ESCAPE:
					{
						done = 1;
					}
					break;

					case SDLK_UP:
					{
						// if man not already in the air
						if (game->man.onLedge)
						{
							game->man.dy = -9;
							game->man.onLedge = 0;
							Mix_PlayChannel(-1, game->jumpSound, 0);
						}
					}
					break;
				}
			}
			break;

			case SDL_QUIT:
			{
				// quit out of game
				done = 1;
			}
			break;
		}
	}

	// gets state of the keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);

	// checks if left arrow is pressed. gives much more fluid animation
	if (state[SDL_SCANCODE_LEFT])
	{
		game->man.slowingDown = 0;
		game->man.dx -= 0.5;
		if (game->man.dx < -6)
		{
			game->man.dx = -6;
		}
		game->man.facingLeft = 1;
	}
	else if (state[SDL_SCANCODE_RIGHT])
	{
		game->man.slowingDown = 0;
		game->man.dx += 0.5;
		if (game->man.dx > 6)
		{
			game->man.dx = 6;
		}
		game->man.facingLeft = 0;
	}
	else
	{
		game->man.animFrame = 0;
		game->man.dx *= 0.8f;
		game->man.slowingDown = 1;
		if (fabsf(game->man.dx) < 0.1f)
		{
			game->man.dx = 0;
		}
	}

	return done;
}

void doRender(SDL_Renderer* renderer, GameState *game)
{
	if (game->statusState == STATUS_STATE_LIVES)
	{
		draw_status_lives(game);
	}
	else if (game->statusState == STATUS_STATE_GAME)
	{
		// set drawing colour to blue (RGB)
		SDL_SetRenderDrawColor(renderer, 100, 149, 237, 255);

		// Clear screen (to blue)
		SDL_RenderClear(renderer);

		// set the drawing colour to white
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

		for (int i = 0; i < 100; i++)
		{
			SDL_Rect ledgeRect = { game->scrollX + game->ledges[i].x, game->ledges[i].y, game->ledges[i].w, game->ledges[i].h };
			SDL_RenderCopy(renderer, game->brick, NULL, &ledgeRect);
		}

		// draw rectangle at man's position
		SDL_Rect rect = { game->scrollX + game->man.x, game->man.y, 48, 48 };	// {x-coord of top left corner, y-coord of top left corner, width, height}
		SDL_RenderCopyEx(renderer, game->manFrames[game->man.animFrame], NULL, &rect, 0, NULL, game->man.facingLeft == 0);
		// last argument to decide whether to flip the image or not
		// always call arrays by reference to prevent creation of copy just to call a function

		if (game->man.isDead)
		{
			SDL_Rect rect = { game->scrollX + game->man.x + 5, game->man.y - 24, 38, 83 };
			SDL_RenderCopyEx(renderer, game->fire, NULL, &rect, 0, NULL, game->time % 20 < 10);
		}

		// draw the star image
		for (int i = 0; i < 100; i++)
		{
			SDL_Rect starRect = { game->scrollX + game->stars[i].x, game->stars[i].y, 50, 50 };
			SDL_RenderCopy(renderer, game->star, NULL, &starRect);
		}

		// the order in which you render is very important. previously rendered components will go behind the newly rendered ones
	}

	// we are done drawing. present to the screen what we just drew
	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{
	GameState gameState;
	SDL_Window* window = NULL;	// declare a window
	SDL_Renderer* renderer = NULL;	// declare a renderer

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);	// initialise sdl2
	
	// create an application window with the following settings
	window = SDL_CreateWindow(
		"Game Window",	// window title
		SDL_WINDOWPOS_UNDEFINED,	// initial x coordinate of window
		SDL_WINDOWPOS_UNDEFINED,	// initial y coordinate of window
		640,	// width
		480, // height
		0	//flags
	);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); 
	// use graphics to accelerate rendering. and match rendering with refresh rate of monitor to prevent tearing (used in place of sdl_delay)
	gameState.renderer = renderer;

	TTF_Init(); // initialise font system

	Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096);

	loadGame(&gameState);

	// enter program loop after window opens
	int done = 0;

	// event loop
	while (!done)
	{
		// check for events
		done = processEvents(window, &gameState);

		process(&gameState);

		collisionDetect(&gameState);

		//render screen
		doRender(renderer, &gameState);

		// don't burn up the CPU. 100 ms between each frame to prevent heat
		// SDL_Delay(50);
	}

	// shutdown game and unload all memory
	SDL_DestroyTexture(gameState.star);
	SDL_DestroyTexture(gameState.manFrames[0]);
	SDL_DestroyTexture(gameState.manFrames[1]);
	SDL_DestroyTexture(gameState.brick);
	if (gameState.label)
	{
		SDL_DestroyTexture(gameState.label);
	}
	TTF_CloseFont(gameState.font);

	Mix_FreeChunk(gameState.bgMusic);
	Mix_FreeChunk(gameState.landSound);
	Mix_FreeChunk(gameState.jumpSound);
	Mix_FreeChunk(gameState.dieSound);

	// close and destroy the window
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	TTF_Quit();

	// clean up
	SDL_Quit();

	return 0;
}