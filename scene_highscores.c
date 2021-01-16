#include "scene_highscores.h"
#include "scene_settings.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"
#include "game.h"

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;

Scene scene_highscores_create();

static void init(){
    bgm = load_audio("Resource/S31-Night Prowler.ogg");
    img_background = load_bitmap_resized("Resource/wizard_tower.jpg",SCREEN_W,SCREEN_H);
}
static void draw(){
    al_draw_bitmap(img_background,0,0,0);
    al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "- HIGHSCORES -");

    for(int i = 0;i<5;i++){
        al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0),600,200+100*i,ALLEGRO_ALIGN_LEFT,"%d. : %d",i+1,high_score[i]);
    }
    
}
static void on_key_down(int keycode){
    if (keycode == ALLEGRO_KEY_ENTER)
        game_change_scene(scene_menu_create());
}
static void destroy(){
    al_destroy_bitmap(img_background);
    al_destroy_sample(bgm);
    stop_bgm(&bgm_id);
}
Scene scene_highscores_create(){
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Start";
    scene.draw = &draw;
    scene.on_key_down = &on_key_down;
    scene.initialize = &init;
    scene.destroy = &destroy;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    game_log("Settings scene created");
    return scene;
}