//
// Created by daniate on 2022/4/2.
//

#ifndef GLUTTONOUSSNAKE_SNAKE_H
#define GLUTTONOUSSNAKE_SNAKE_H

#include <SDL.h>
#include "geometry.h"

#define TILE_SIZE (20)

// 蛇的最慢速度：每秒走几步
#define SNAKE_LOWEREST_SPEED (4)
#define SNAKE_DELAY_TIME (1000.0 / SNAKE_LOWEREST_SPEED)

typedef Position_t Food_t;

typedef enum {
    REST,
    UP,
    DOWN,
    LEFT,
    RIGHT,
} Direction;

typedef struct SnakeHead {
    Position_t pos;
} SnakeHead_t;

typedef struct SnakeBody {
    Position_t pos;
    struct SnakeBody *next;
} SnakeBody_t;

typedef struct Snake {
    SnakeHead_t *head;
    SnakeBody_t *body;
    Direction direction;
    /**
     * 尾巴的旧位置，当蛇移动时，会记录下来；
     * 假如蛇吃到了食物，就会增加一个SnakeBoy_t，
     * 并将新body的位置设置为这个位置
     */
    Position_t oldTailPos;
} Snake_t;

void snake_init(Snake_t **pSnake, int sceneWidth, int sceneHeight);
void snake_change_direction(Snake_t *snake, SDL_KeyCode keyCode);
void snake_move(Snake_t *snake);
bool snake_eat_food(Snake_t *snake, Food_t *food);
bool snake_did_eat_itself(Snake_t *snake);
void snake_free(Snake_t **pSnake);

#endif //GLUTTONOUSSNAKE_SNAKE_H
