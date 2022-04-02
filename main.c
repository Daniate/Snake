//
// Created by daniate on 2022/4/2.
//

#include <stdio.h>
#include <stdlib.h>

#include "snake.h"

#define SCENE_WIDTH (800)
#define SCENE_HEIGHT (600)

#define FPS (30)
#define DELAY_TIME (1000.0 / FPS)

typedef struct GameLogic {
    bool running;
    bool failed;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    Snake_t *snake;
    Food_t *food;
    Uint32 sceneRenderedLastStamp;
    Uint32 snakeMovedLastStamp;
} GameLogic_t;

GameLogic_t *game = NULL;

void changePositionOfFood() {
    int x, y;

    bool overlap;
    do {
        srand((unsigned int) time(NULL));
        x = rand() % (SCENE_WIDTH / TILE_SIZE) * TILE_SIZE;

        srand((unsigned int) time(NULL));
        y = rand() % (SCENE_HEIGHT / TILE_SIZE) * TILE_SIZE;

        overlap = (x == game->snake->head->pos.x && y == game->snake->head->pos.y);
        if (overlap) {
            continue;
        }
        SnakeBody_t *body = game->snake->body;
        while (body) {
            overlap = (x == body->pos.x && y == body->pos.y);
            if (overlap) {
                break;
            }
            body = body->next;
        }
    } while (overlap);

    game->food->x = x;
    game->food->y = y;
}

void initGameLogic() {
    game = malloc(sizeof(GameLogic_t));
    memset(game, '\0', sizeof(GameLogic_t));

    snake_init(&game->snake, SCENE_WIDTH, SCENE_HEIGHT);

    game->food = malloc(sizeof(Food_t));
    memset(game->food, '\0', sizeof(Food_t));

    game->running = true;

    changePositionOfFood();
}

void initSDL() {
    SDL_Init(SDL_INIT_EVERYTHING);

    game->window = SDL_CreateWindow("贪吃蛇", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCENE_WIDTH, SCENE_HEIGHT,
                                    SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!game->renderer) {
        game->renderer = SDL_CreateRenderer(game->window, -1, 0);
    }
    SDL_RenderSetLogicalSize(game->renderer, SCENE_WIDTH, SCENE_HEIGHT);
}

void moveSnake() {
    snake_move(game->snake);
    game->snakeMovedLastStamp = SDL_GetTicks();
}

bool detectCollision() {
    bool status = false;

    // 判断蛇头是否出界，如果出界，则游戏失败
    if (game->snake->head->pos.x < 0 || game->snake->head->pos.x >= SCENE_WIDTH || game->snake->head->pos.y < 0 ||
        game->snake->head->pos.y >= SCENE_HEIGHT) {
        fprintf(stderr, "蛇头出界\n");
        status = true;
    }

    // 判断蛇头是否与蛇身其他部分重叠，如果有重叠，则游戏失败
    if (snake_did_eat_itself(game->snake)) {
        fprintf(stderr, "蛇头撞到蛇身\n");
        status = true;
    }

    return status;
}

void eatFoodIfNeeded() {
    if (snake_eat_food(game->snake, game->food)) {
        changePositionOfFood();
    }
}

void updateGameLogic() {
    if (SDL_QUIT == game->event.type) {
        game->running = false;
    } else if (SDL_KEYDOWN == game->event.type) {
        SDL_KeyCode keyCode = game->event.key.keysym.sym;
        if (SDLK_ESCAPE == keyCode || SDLK_q == keyCode) {
            game->running = false;
        } else if (SDLK_SPACE == keyCode) {
            if (game->failed) {
                game->failed = false;
                game->snakeMovedLastStamp = 0;
                snake_free(&game->snake);
                snake_init(&game->snake, SCENE_WIDTH, SCENE_HEIGHT);
                changePositionOfFood();
            } else {
                snake_change_direction(game->snake, keyCode);
            }
        } else {
            if (!game->failed) {
                snake_change_direction(game->snake, keyCode);
            }
        }
    }

    if (game->running && !game->failed) {
        if (SDL_GetTicks() - game->snakeMovedLastStamp >= SNAKE_DELAY_TIME) {
            moveSnake();
            game->failed = detectCollision();
            if (!game->failed) {
                eatFoodIfNeeded();
            }
        }
    }
}

void renderScene() {
    if (!game->running) {
        return;
    }

    Uint32 frameTime = SDL_GetTicks() - game->sceneRenderedLastStamp;
    if (frameTime < DELAY_TIME) {
        SDL_Delay((int) (DELAY_TIME - frameTime));
    }

    // 清空背景
    SDL_SetRenderDrawColor(game->renderer, 0x00, 0x00, 0x00, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(game->renderer);

    SDL_Rect rect;
    rect.x = game->food->x;
    rect.y = game->food->y;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    // 绘制食物
    SDL_SetRenderDrawColor(game->renderer, 0x6A, 0x5A, 0xCD, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(game->renderer, &rect);

    // 先绘制蛇身
    SDL_SetRenderDrawColor(game->renderer, 0x99, 0x99, 0x99, SDL_ALPHA_OPAQUE);
    SnakeBody_t *body = game->snake->body;
    while (NULL != body) {
        rect.x = body->pos.x;
        rect.y = body->pos.y;

        SDL_RenderFillRect(game->renderer, &rect);

        body = body->next;
    }

    // 后绘制蛇头，防止在蛇头碰撞到蛇身时，蛇身覆盖到蛇头
    SDL_SetRenderDrawColor(game->renderer, 0x00, 0xDD, 0x00, SDL_ALPHA_OPAQUE);
    rect.x = game->snake->head->pos.x;
    rect.y = game->snake->head->pos.y;
    SDL_RenderFillRect(game->renderer, &rect);

    SDL_RenderPresent(game->renderer);

    game->sceneRenderedLastStamp = SDL_GetTicks();
}

void quit() {
    free(game->food);
    game->food = NULL;

    snake_free(&game->snake);

    SDL_DestroyRenderer(game->renderer);
    game->renderer = NULL;

    SDL_DestroyWindow(game->window);
    game->window = NULL;

    free(game);

    SDL_Quit();
}

int main(int argc, char **argv) {
    initGameLogic();
    initSDL();
    while (game->running) {
        SDL_PollEvent(&game->event);
        updateGameLogic();
        renderScene();
    }
    quit();
    return EXIT_SUCCESS;
}
