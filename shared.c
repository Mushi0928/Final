// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
#include "scene_menu.h"
ALLEGRO_FONT* font_pirulen_180;
ALLEGRO_FONT* font_pirulen_120;
ALLEGRO_FONT* font_pirulen_60;
ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;

void shared_init(void) {
    font_pirulen_180 = load_font("Resource/pirulen.ttf", 180);
    font_pirulen_120 = load_font("Resource/pirulen.ttf", 120);
    font_pirulen_60 = load_font("Resource/pirulen.ttf", 60);
    font_pirulen_32 = load_font("Resource/pirulen.ttf", 32);
    font_pirulen_24 = load_font("Resource/pirulen.ttf", 24);
    game_change_scene(scene_menu_create());
}

void shared_destroy(void) {
    al_destroy_font(font_pirulen_32);
    al_destroy_font(font_pirulen_24);
}