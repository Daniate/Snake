//
// Created by daniate on 2022/4/2.
//

#ifndef GLUTTONOUSSNAKE_GEOMETRY_H
#define GLUTTONOUSSNAKE_GEOMETRY_H

#include <stdbool.h>

typedef struct Position {
    int x;
    int y;
} Position_t;

bool position_equal(Position_t pos1, Position_t pos2);

#endif //GLUTTONOUSSNAKE_GEOMETRY_H
