//
// Created by daniate on 2022/4/2.
//

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "snake.h"

void snake_init(Snake_t **pSnake, int sceneWidth, int sceneHeight) {
    Snake_t *snake = malloc(sizeof(Snake_t));
    memset(snake, '\0', sizeof(Snake_t));
    snake->direction = REST;

    snake->head = malloc(sizeof(SnakeHead_t));
    memset(snake->head, '\0', sizeof(SnakeHead_t));
    snake->head->pos.x = sceneWidth >> 1;
    snake->head->pos.y = sceneHeight >> 1;

    snake->body = malloc(sizeof(SnakeBody_t));
    memset(snake->body, '\0', sizeof(SnakeBody_t));
    snake->body->pos.x = snake->head->pos.x;
    snake->body->pos.y = snake->head->pos.y + TILE_SIZE;

    *pSnake = snake;
}

void snake_change_direction(Snake_t *snake, SDL_KeyCode keyCode) {
    switch (keyCode) {
        case SDLK_SPACE: {
            if (REST == snake->direction) {
                srand((unsigned int) time(NULL));
                int n = rand() % 3;
                if (0 == n) {
                    snake->direction = LEFT;
                } else if (1 == n) {
                    snake->direction = RIGHT;
                } else {
                    snake->direction = UP;
                }
            }
            break;
        }
        case SDLK_UP: {
            if (LEFT == snake->direction || RIGHT == snake->direction) {
                snake->direction = UP;
            }
            break;
        }
        case SDLK_DOWN: {
            if (LEFT == snake->direction || RIGHT == snake->direction) {
                snake->direction = DOWN;
            }
            break;
        }
        case SDLK_LEFT: {
            if (UP == snake->direction || DOWN == snake->direction) {
                snake->direction = LEFT;
            }
            break;
        }
        case SDLK_RIGHT: {
            if (UP == snake->direction || DOWN == snake->direction) {
                snake->direction = RIGHT;
            }
            break;
        }
        default:
            break;
    }
}

void snake_move(Snake_t *snake) {
    if (REST == snake->direction) {
        return;
    }
    SnakeBody_t *body = snake->body;
    Position_t prevPos = snake->head->pos;
    while (body) {
        Position_t pos = body->pos;
        body->pos = prevPos;
        prevPos = pos;
        body = body->next;
    }
    snake->oldTailPos = prevPos;

    switch (snake->direction) {
        case UP:
            snake->head->pos.y -= TILE_SIZE;
            break;
        case DOWN:
            snake->head->pos.y += TILE_SIZE;
            break;
        case LEFT:
            snake->head->pos.x -= TILE_SIZE;
            break;
        case RIGHT:
            snake->head->pos.x += TILE_SIZE;
            break;
        default:
            break;
    }
}

void play_eat_food_sound() {
    SDL_AudioSpec spec;
    Uint8 *buffer;
    Uint32 length;
    const char *file = "mixkit-retro-game-notification-212.wav";
    if (SDL_LoadWAV(file, &spec, &buffer, &length)) {
        SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
        if (0 == SDL_QueueAudio(deviceId, buffer, length)) {
            SDL_PauseAudioDevice(deviceId, 0);
        } else {
            fprintf(stderr, "Could not enqueue audio: %s\n", SDL_GetError());
        }
        /* Do stuff with the WAV data, and then... */
        SDL_FreeWAV(buffer);
    } else {
        fprintf(stderr, "Could not open %s: %s\n", file, SDL_GetError());
    }
}

bool snake_eat_food(Snake_t *snake, Food_t *food) {
    if (snake->head->pos.x == food->x && snake->head->pos.y == food->y) {
        SnakeBody_t *body = snake->body;
        while (body) {
            if (body->next) {
                body = body->next;
            } else {
                play_eat_food_sound();

                body->next = malloc(sizeof(SnakeBody_t));
                memset(body->next, '\0', sizeof(SnakeBody_t));
                body->next->pos = snake->oldTailPos;
                return true;
            }
        }
    }
    return false;
}

bool snake_did_eat_itself(Snake_t *snake) {
    SnakeBody_t *body = snake->body;
    while (body) {
        if (position_equal(snake->head->pos, body->pos)) {
            return true;
        }
        body = body->next;
    }
    return false;
}

void snake_free(Snake_t **pSnake) {
    Snake_t *snake = *pSnake;
    if (snake) {
        SnakeBody_t *body = snake->body;
        while (body) {
            SnakeBody_t *prev = body;
            body = body->next;
            free(prev);
        }
        if (snake->head) {
            free(snake->head);
        }
        free(snake);
    }
    *pSnake = NULL;
}