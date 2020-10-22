#include <SDL.h> // Always needs to be included for an SDL app
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#include <exception>
#include <iostream>
#include <string>

//Game general information
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

//Speed
#define SPEED_X 10
#define SPEED_Y 10

int main(int, char *[])
{
	// --- INIT SDL ---
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		throw "No es pot inicialitzar SDL subsystems";

	// --- WINDOW ---
	SDL_Window *m_window{ SDL_CreateWindow("SDL...", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN) };
	if (m_window == nullptr)
		throw "No es pot inicialitzar SDL_Window";

	// --- RENDERER ---
	SDL_Renderer *m_renderer{ SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
	if (m_renderer == nullptr)
		throw "No es pot inicialitzar SDL_Renderer";

	//Initialize renderer color
	SDL_SetRenderDrawColor(m_renderer, 255, 255, 255, 255);

	//-->SDL_Image
	const Uint8 imgFlags{ IMG_INIT_PNG | IMG_INIT_JPG };
	if (!(IMG_Init(imgFlags) & imgFlags))
		throw "Error: SDL_image init";

	//-->SDL_TTF
	if (TTF_Init() == -1)
		throw "No es pot inicialitzar SDL_TTF";

	//-->SDL_Mix
	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
		throw "No es pot inicialitzar SDL_MIX";

	// --- SPRITES ---
	//Background
	SDL_Texture *bgTexture{ IMG_LoadTexture(m_renderer, "../../res/img/bg.jpg") };
	if (bgTexture == nullptr)
		throw "Error: bgTexture init";
	SDL_Rect bgRect{ 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	//Cursor
	SDL_Texture *cursorTexture{ IMG_LoadTexture(m_renderer, "../../res/img/kintoun.png") };
	SDL_Rect cursorRect{ 0,0,100,50 };
	SDL_Rect playerRect{ 0,0,100,50 };
	//-->Animated Sprite ---

	// --- TEXT ---
#pragma region // --- TEXT ---	
	TTF_Font *saiyanFont = TTF_OpenFont("../../res/ttf/saiyan.ttf", 72);
	SDL_Color saiyanColor = { 255,128,0 };
	SDL_Surface *gameSurface = TTF_RenderText_Solid(saiyanFont, "An SDL project is ready", saiyanColor);
	SDL_Texture *saiyanTexture = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(saiyanTexture, NULL, NULL, &texW, &texH);
	SDL_Rect saiyanRect = { 100,50,texW,texH };
	//Hover
	SDL_Color saiyanColorH = { 0,255,255 };
	gameSurface = TTF_RenderText_Solid(saiyanFont, "An SDL project is ready", saiyanColorH);
	SDL_Texture *saiyanTextureH = SDL_CreateTextureFromSurface(m_renderer, gameSurface);

	//Play
	int playW = 0;
	int playH = 0;
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Play", saiyanColor);
	SDL_Texture *playTexture = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	SDL_QueryTexture(playTexture, NULL, NULL, &playW, &playH);
	SDL_Rect playRect = { 100,150,playW,playH };
	//Hover play
	SDL_Color redC = { 255,0,0 };
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Play", redC);
	SDL_Texture *playTextureH = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	//Hover playPressed
	SDL_Color greenC = { 0,255,0 };
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Play", greenC);
	SDL_Texture *playTextureHpressed = SDL_CreateTextureFromSurface(m_renderer, gameSurface);

	////Options
	//int optsW = 0;
	//int optsH = 0;
	//gameSurface = TTF_RenderText_Solid(saiyanFont, "Options", saiyanColor);
	//SDL_Texture *optsTexture = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	//SDL_QueryTexture(optsTexture, NULL, NULL, &optsW, &optsH);
	//SDL_Rect optsRect = { 100,250,optsW,optsH };
	////Hover opts
	//gameSurface = TTF_RenderText_Solid(saiyanFont, "Options", saiyanColorH);
	//SDL_Texture *optsTextureH = SDL_CreateTextureFromSurface(m_renderer, gameSurface);

	//Exit
	int exitW = 0;
	int exitH = 0;
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Exit", saiyanColor);
	SDL_Texture *exitTexture = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	SDL_QueryTexture(exitTexture, NULL, NULL, &exitW, &exitH);
	SDL_Rect exitRect = { 100,350,exitW,exitH };
	//Hover exit
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Exit", saiyanColorH);
	SDL_Texture *exitTextureH = SDL_CreateTextureFromSurface(m_renderer, gameSurface);

	//Sound
	int soundW = 0;
	int soundH = 0;
	//on
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Sound", greenC);
	SDL_Texture *soundTexture = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
	SDL_QueryTexture(soundTexture, NULL, NULL, &soundW, &soundH);
	SDL_Rect soundRect = { 100,450,soundW,soundH };
	//off
	gameSurface = TTF_RenderText_Solid(saiyanFont, "Sound", redC);
	SDL_Texture *soundTextureH = SDL_CreateTextureFromSurface(m_renderer, gameSurface);
#pragma endregion
	// --- AUDIO ---
	Mix_Music *soundtrack{ Mix_LoadMUS("../../res/au/mainTheme.mp3") };
	if (!soundtrack)
		throw "Unable to load soundtrack";
	Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(soundtrack, -1);
	// --- GAME LOOP ---
#pragma region Booleans
	bool isRunning = true;
	bool playClick = false;
	bool soundOn = true;
	bool escBtn = true;
	bool mosuePressed = false;
#pragma endregion
	SDL_Event event;
	while (isRunning)
	{
		// HANDLE EVENTS
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isRunning = false;
				break;
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					isRunning = false;
				break;
			case SDL_MOUSEMOTION:
				cursorRect.x = event.motion.x + 20;
				cursorRect.y = event.motion.y + 10;
				break;
			case SDL_MOUSEBUTTONDOWN:
				mosuePressed = true;
				break;
			default:;
			}
		}

		// UPDATE
		playerRect.x += ((cursorRect.x - playerRect.w / 2) - playerRect.x) / SPEED_X;
		playerRect.y += ((cursorRect.y - playerRect.h / 2) - playerRect.y) / SPEED_Y;

#pragma region buttons
		isRunning = escBtn;
		if (mosuePressed) {
			if (cursorRect.x > 100 && cursorRect.x < 100 + playW && cursorRect.y>150 && cursorRect.y < 150 + playH) playClick = true;
			if (cursorRect.x > 100 && cursorRect.x < 100 + exitW && cursorRect.y>350 && cursorRect.y < 350 + exitH) isRunning = false;
			if (cursorRect.x > 100 && cursorRect.x < 100 + exitW && cursorRect.y>450 && cursorRect.y < 450 + exitH) soundOn = !soundOn;
			mosuePressed = false;
		}
		playerRect.x += ((cursorRect.x - playerRect.w / 2) - playerRect.x) / SPEED_X;
		playerRect.y += ((cursorRect.y - playerRect.h / 2) - playerRect.y) / SPEED_Y;

		//Button music
		if (soundOn == false) { Mix_PauseMusic(); }
		else { Mix_ResumeMusic(); }

#pragma region Hover
		SDL_Texture * playFinalTexture = nullptr;
		//PLAY
		if (cursorRect.x > 100 && cursorRect.x < 100 + playW && cursorRect.y>150 && cursorRect.y < 150 + playH)
		{
			if (!playClick)playFinalTexture = playTextureH;
			else playFinalTexture = playTextureHpressed; //playClick = false;
		}
		else //NORMAL
		{
			playFinalTexture = playTexture;
		}

		////OPTS
		//SDL_Texture * optsFinalTexture = nullptr;
		//if (cursorRect.x > 100 && cursorRect.x < 100 + optsW && cursorRect.y>250 && cursorRect.y < 250 + optsH)
		//{
		//	optsFinalTexture = optsTextureH;
		//}
		//else //NORMAL
		//{
		//	optsFinalTexture = optsTexture;
		//}
		//EXIT
		SDL_Texture * exitFinalTexture = nullptr;
		if (cursorRect.x > 100 && cursorRect.x < 100 + exitW && cursorRect.y>350 && cursorRect.y < 350 + exitH)
		{
			exitFinalTexture = exitTextureH;
		}
		else //NORMAL
		{
			exitFinalTexture = exitTexture;
		}
		//TEXT
		SDL_Texture * textFinalTexture = nullptr;
		//---HOVER
		if (cursorRect.x > 100 && cursorRect.x < 100 + texW && cursorRect.y>50 && cursorRect.y < 50 + texH)
		{
			textFinalTexture = saiyanTextureH;
		}
		else //NORMAL
		{
			textFinalTexture = saiyanTexture;
		}

		//SOUND TEXT
		SDL_Texture * soundFinalTexture = nullptr;
		if (soundOn == true)soundFinalTexture = soundTexture;
		if (soundOn == false)soundFinalTexture = soundTextureH;

#pragma endregion
#pragma endregion

		// DRAW
		SDL_RenderClear(m_renderer);
		//Background
		SDL_RenderCopy(m_renderer, bgTexture, nullptr, &bgRect);
#pragma region buttons render
		//TEXT
		SDL_RenderCopy(m_renderer, textFinalTexture, nullptr, &saiyanRect);
		//PLAY
		SDL_RenderCopy(m_renderer, playFinalTexture, nullptr, &playRect);
		////OPTS
		//SDL_RenderCopy(m_renderer, optsFinalTexture, nullptr, &optsRect);
		//EXIT
		SDL_RenderCopy(m_renderer, exitFinalTexture, nullptr, &exitRect);
		//SOUND
		SDL_RenderCopy(m_renderer, soundFinalTexture, nullptr, &soundRect);

#pragma endregion
		//CURSOR
		SDL_RenderCopy(m_renderer, cursorTexture, nullptr, &playerRect);

		SDL_RenderPresent(m_renderer);
	}

	// --- DESTROY ---
	Mix_CloseAudio();
	//Font
	TTF_CloseFont(saiyanFont);
	//textures
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(exitTextureH);
	//SDL_DestroyTexture(optsTextureH);
	SDL_DestroyTexture(playTextureH);
	SDL_DestroyTexture(saiyanTextureH);
	SDL_DestroyTexture(soundTextureH);
	SDL_DestroyTexture(soundTexture);
	SDL_DestroyTexture(exitTexture);
	//SDL_DestroyTexture(optsTexture);
	SDL_DestroyTexture(playTexture);
	SDL_DestroyTexture(saiyanTexture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);

	// --- QUIT ---
	Mix_Quit();
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();

	return 0;
}