#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>

#include "registry.hpp"
#include "systems.hpp"

struct GameContext {
	SDL_Window* window;
	SDL_Renderer* renderer;
	Uint64 last_frame_ticks;
	Registry registry;

	// Texturas armazenadas aqui para limpar no fechamento do jogo
	SDL_Texture* soldier_tex;
};

SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
	if (!SDL_Init(SDL_INIT_VIDEO)) return SDL_APP_FAILURE;

	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");

	GameContext* ctx = new GameContext();
	ctx->last_frame_ticks = SDL_GetTicks();

	if (!SDL_CreateWindowAndRenderer("Marketplace de Órgãos", 800, 600, 0, &ctx->window, &ctx->renderer)) {
		return SDL_APP_FAILURE;
	}

	ctx->soldier_tex = IMG_LoadTexture(ctx->renderer, "assets/main/Soldier.png");
	if (!ctx->soldier_tex) {
		std::cerr << "ERRO: " << SDL_GetError() << std::endl;
		return SDL_APP_FAILURE;
	}
	SDL_SetTextureScaleMode(ctx->soldier_tex, SDL_SCALEMODE_NEAREST);

	Entity player = ctx->registry.create_entity();
	ctx->registry.positions.insert(player, {400.0f, 300.0f});
	ctx->registry.sprites.insert(player, {false, 100.0f, 100.0f, 3.0f, ctx->soldier_tex});

	SDL_Texture* map_tex = IMG_LoadTexture(ctx->renderer, "assets/grass.png");
	SDL_SetTextureScaleMode(map_tex, SDL_SCALEMODE_NEAREST);
	Entity map_entity = ctx->registry.create_entity();

	std::vector<int> background_tilemap = {
		0, 0, 0, 0, 253, 254, 0, 0, 0, 0,
		0, 1, 0, 0, 2, 2, 0, 0, 0, 0,
		0, 0, 0, 2, 2, 2, 2, 0, 1, 0,
		0, 0, 0, 2, 2, 2, 2, 0, 0, 0,
		0, 0, 0, 0, 2, 2, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 1, 0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 180
	};

	// {texture, layout, tile_size, tileset_columns, map_columns, map_rows, scale}
	ctx->registry.positions.insert(map_entity, {20.0f, 20.0f});
	ctx->registry.tilemaps.insert(map_entity, {map_tex, background_tilemap, 16, 25, 10, 8, 4.0f});

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

	Systems::RenderTilemap(ctx->registry, ctx->renderer);
	Systems::Render(ctx->registry, ctx->renderer);

	SDL_RenderPresent(ctx->renderer);
	return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	if (appstate) {
		GameContext* ctx = static_cast<GameContext*>(appstate);
		if (ctx->soldier_tex) SDL_DestroyTexture(ctx->soldier_tex);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		delete ctx;
	}
	SDL_Quit();
}
