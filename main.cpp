#define SDL_MAIN_USE_CALLBACKS 1 // Ativa o novo sistema de loop do SDL3
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h> // Necessário para substituir o main()
#include <SDL3_image/SDL_image.h>
#include <iostream>

const float FRAME_WIDTH = 100.0f;
const float FRAME_HEIGHT = 100.0f;
const float DRAW_SCALE = 3.0f;

// O estado do jogo
struct GameContext {
	bool facing_left;
	int current_frame;
	int anim_row;
	int frames_in_row;

	float player_x, player_y;
	float anim_timer;

	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* player_texture;

	Uint64 last_frame_ticks;
};

// 1. Inicialização (Chamado uma vez ao abrir o jogo)
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {

	// SDL3 retorna booleano (true = sucesso)
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		std::cerr << "Erro SDL_Init: " << SDL_GetError() << std::endl;
		return SDL_APP_FAILURE;
	}

	GameContext* ctx = new GameContext();
	ctx->player_x = 400.0f;
	ctx->player_y = 300.0f;
	ctx->current_frame = 0;
	ctx->anim_row = 0;
	ctx->anim_timer = 0.0f;
	ctx->frames_in_row = 6;
	ctx->facing_left = false;

	// @TODO@: pesquisar pq isso quebra
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	// Cria janela e renderizador em uma única chamada simplificada
	if (!SDL_CreateWindowAndRenderer("Marketplace de Órgãos", 800, 600, 0, &ctx->window, &ctx->renderer)) {
		std::cerr << "Erro Janela/Render: " << SDL_GetError() << std::endl;
		return SDL_APP_FAILURE;
	}

	ctx->player_texture = IMG_LoadTexture(ctx->renderer, "assets/main/Soldier.png");

	if(!ctx->player_texture) {
		std::cerr << "[ERRO] Falha ao carregar textura: " << SDL_GetError() << std::endl;
		return SDL_APP_FAILURE;
	}

	ctx->last_frame_ticks = SDL_GetTicks();
	*appstate = ctx;
	return SDL_APP_CONTINUE; // Continua rodando
}

// 2. Eventos (Chamado para cada input, clique ou comando)
SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
	if (event->type == SDL_EVENT_QUIT) { // Em SDL3, SDL_QUIT virou SDL_EVENT_QUIT
		return SDL_APP_SUCCESS; // Encerra o jogo com sucesso
	}
	return SDL_APP_CONTINUE;
}

// 3. Game Loop (Chamado a cada frame sincronizado com o monitor)
SDL_AppResult SDL_AppIterate(void* appstate) {
	GameContext* ctx = static_cast<GameContext*>(appstate);

	Uint64 current_ticks = SDL_GetTicks();
	float delta_time = (current_ticks - ctx->last_frame_ticks) / 1000.0f;
	ctx->last_frame_ticks = current_ticks;

	// SDL3 retorna array de booleanos, não de Uint8
	int numkeys;
	const bool* state = SDL_GetKeyboardState(&numkeys);

	bool is_moving = false;
	float speed = 250.0f * delta_time;

	if (state[SDL_SCANCODE_RIGHT]) {
		ctx->player_x += speed;
		ctx->facing_left = false;
		is_moving = true;
	}

	if (state[SDL_SCANCODE_LEFT]) {
		ctx->player_x -= speed;
		ctx->facing_left = true;
		is_moving = true;
	}

	if (state[SDL_SCANCODE_DOWN]) {
		ctx->player_y += speed;
		is_moving = true;
	}

	if (state[SDL_SCANCODE_UP]) {
		ctx->player_y -= speed;
		is_moving = true;
	}

	if (state[SDL_SCANCODE_ESCAPE]) return SDL_APP_SUCCESS;

	if (is_moving) {
		if (ctx->anim_row != 1) {
			ctx->anim_row = 1;
			ctx->frames_in_row = 8;
			ctx->current_frame = 0;
			ctx->anim_timer = 0.0f;
		}
	} else {
		if (ctx->anim_row != 0) {
			ctx->anim_row = 0;
			ctx->frames_in_row = 6;
			ctx->current_frame = 0;
			ctx->anim_timer = 0.0f;
		}
	}

	ctx->anim_timer += delta_time;
	if(ctx->anim_timer >= 0.1f) {
		ctx->current_frame++;

		if(ctx->current_frame >= ctx->frames_in_row)
			ctx->current_frame = 0;

		ctx->anim_timer -= 0.1f;
	}

	// Limpa a tela (Fundo verde escurinho)
	SDL_SetRenderDrawColor(ctx->renderer, 34, 139, 34, 255);
	SDL_RenderClear(ctx->renderer);

	// Desenha o jogador (SDL3 utiliza FRect (float) por padrão)
	SDL_FRect src_rect;
	src_rect.x = ctx->current_frame * FRAME_WIDTH;
	src_rect.y = ctx->anim_row * FRAME_HEIGHT;
	src_rect.w = FRAME_WIDTH;
	src_rect.h = FRAME_HEIGHT;

	SDL_FRect dst_rect;
	dst_rect.x = ctx->player_x;
	dst_rect.y = ctx->player_y;
	dst_rect.w = FRAME_WIDTH * DRAW_SCALE;
	dst_rect.h = FRAME_HEIGHT * DRAW_SCALE;

	SDL_FlipMode flip = ctx->facing_left ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
	SDL_RenderTextureRotated(ctx->renderer, ctx->player_texture, &src_rect, &dst_rect, 0.0, nullptr, flip);

	// Atualiza a tela
	SDL_RenderPresent(ctx->renderer);

	return SDL_APP_CONTINUE;
}

// 4. Encerramento (Chamado quando retorna SDL_APP_SUCCESS ou FAILURE)
void SDL_AppQuit(void* appstate, SDL_AppResult result) {
	if (appstate) {
		GameContext* ctx = static_cast<GameContext*>(appstate);
		// Destruição de recursos (Opcional no SDL3 pois ele limpa automaticamente, mas recomendado)
		if(ctx->player_texture) SDL_DestroyTexture(ctx->player_texture);
		SDL_DestroyRenderer(ctx->renderer);
		SDL_DestroyWindow(ctx->window);
		delete ctx;
	}
	SDL_Quit();
}
