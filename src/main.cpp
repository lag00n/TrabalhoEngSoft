#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

#include "registry.hpp"

struct GameContext {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Uint64 last_frame_ticks;
	Registry registry;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) return SDL_APP_FAILURE;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	GameContext* ctx = new GameContext();
	ctx->last_frame_ticks = SDL_GetTicks();

	if (!SDL_CreateWindowAndRenderer("Marketplace de Órgãos", 800, 600, 0, &ctx->window, &ctx->renderer)) {
		return SDL_APP_FAILURE;
	}

	Entity player = ctx->registry.create_entity();
	ctx->registry.positions.insert(player, {400.0f, 300.0f});

	*appstate = ctx;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) return SDL_APP_SUCCESS;
	return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
	GameContext* ctx = static_cast<GameContext*>(appstate);

	SDL_SetRenderDrawColor(ctx->renderer, 34, 139, 34, 255);
	SDL_RenderClear(ctx->renderer);

	SDL_RenderPresent(ctx->renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	if (appstate) {
		GameContext* ctx = static_cast<GameContext*>(appstate);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		delete ctx;
	}
	SDL_Quit();
}
