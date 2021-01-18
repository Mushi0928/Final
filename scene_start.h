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
typedef enum enemy_type{
    ENEMY_SKELETON,
}Enemy_Type;
typedef struct mage{
    //The cneter coordinate of mage
    float x,y;
    //The width and height of mage 
    float w,h;
    // The width and height of a single sprite.
    float sprite_w,sprite_h;
    //The velocity in x,y axes
    float vx,vy;
    int momentum_x;
    //The direction and pose the mage are in
    int direction;
    //The HP and ATK value of the mage
    int hp,max_hp,mp,max_mp,attack;
    //Spritesheet of the mage
    ALLEGRO_BITMAP* spritesheet;
    float last_hit_timestamp;
}Mage;
typedef struct {
    float x,y;
    float w,h;
    float sprite_w,sprite_h;
    float sprite_x,sprite_y;
    float vx,vy;
    int momentum_x;
    float speed;
    int direction;
    int hp,max_hp,attack;
    ALLEGRO_BITMAP* sprite;
    int enemy_type;
    float last_hit_timestamp;
    float last_anim_timestamp;
    bool hidden;
    int animationFrame;
}Enemy;

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
    //the coordinate of the left upper corner of the collider
    float x,y;
    //the width and height of the collider
    float w,h;
    bool hidden;
}BlockCollider;
typedef struct {
    bool hidden;
    float x,y;
    float radius;
    float dR,ddR;
}Ultimate;
// Return a Scene that has function pointers pointing to the
// functions that will react to the corresponding events.
Scene scene_start_create(void);
#endif