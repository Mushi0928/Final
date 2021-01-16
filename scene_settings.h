// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

#ifndef SCENE_SETTINGS_H
#define SCENE_SETTINGS_H
#include "game.h"

extern int volume;
extern int SE;
extern int difficulty;

typedef enum options{
    OPTION_VOLUME,
    OPTION_SE,
    OPTION_DIFFICULTY,
}Options;

typedef struct _option_rect{
    char Title[15];
    float x,y;
    float w,h;
    bool focused;
}Option_Rect;
// Return a Scene that has function pointers pointing to the
// functions that will react to the corresponding events.
Scene scene_settings_create(void);
#endif