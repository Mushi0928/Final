// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_START_H
#define SCENE_START_H
#include "game.h"

typedef enum mage_dir{
    RIGHT,
    LEFT,
    JUMP_RIGHT,
    JUMP_LEFT,
    ATTACK_RIGHT,
    ATTACK_LEFT,
    DIED,
}Mage_Direction;

typedef struct mage{
    //The cneter coordinate of mage
    float x,y;
    //The width and height of mage 
    float w,h;
    // The width and height of a single sprite.
    float sprite_w,sprite_h;
    //The velocity in x,y axes
    float vx,vy;
    //The direction and pose the mage are in
    int direction;
    //The HP and ATK value of the mage
    int hp,attack;
    //Spritesheet of the mage
    ALLEGRO_BITMAP* spritesheet;
}Mage;

typedef struct {
    // The center coordinate of the image.
    float x, y;
    // The width and height of the object.
    float w, h;
    // The velocity in x, y axes.
    float vx, vy;
    // Should we draw this object on the screen.
    bool hidden;
    // The pointer to the object’s image.
    ALLEGRO_BITMAP* img;
} MovableObject;

typedef struct {
    float x,y;
    float w,h;
}Block;
// Return a Scene that has function pointers pointing to the
// functions that will react to the corresponding events.
Scene scene_start_create(void);
#endif