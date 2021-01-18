// [HACKATHON 3-9]
// TODO: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
#include "scene_menu.h"
#include "utility.h"
#include "game.h"
#include "shared.h"
#include "string.h"
#include "allegro5/allegro_primitives.h"

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_BITMAP* img_arrow;
static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;

static Option_Rect option[3];
static int option_flag = OPTION_VOLUME;
int volume = 100;
int SE = 100;
int difficulty = 0;
char difficulty_string[4][10];

static void init(void){
    bgm = load_audio("Resource/S31-Night Prowler.ogg");
    play_bgm(bgm,&bgm_id, (float)volume/100.0);
    for(int i = 0;i<3;i++){
        option[i].w = 400;
        option[i].h = 64;
        option[i].focused = false;
        option[i].x = 400;
    }
    option[OPTION_VOLUME].y = 300;
    option[OPTION_SE].y = 500;
    option[OPTION_DIFFICULTY].y = 700;
    option[OPTION_VOLUME].focused = true;
    strcpy(option[OPTION_VOLUME].Title,"VOLUME :");
    strcpy(option[OPTION_SE].Title,"SE :");
    strcpy(option[OPTION_DIFFICULTY].Title,"DIFFICULTY :");
    strcpy(difficulty_string[0],"Easy");
    strcpy(difficulty_string[1],"Normal");
    strcpy(difficulty_string[2],"Hard");
    strcpy(difficulty_string[3],"Hell");
    img_background = load_bitmap_resized("Resource/wizard_tower.jpg",SCREEN_W,SCREEN_H);
    img_arrow = al_load_bitmap("Resource/arrow.png");
}
static void draw(void) {
    al_draw_bitmap(img_background,0,0,0);
    al_draw_text(font_pirulen_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "- SETTINGS -");
    al_draw_text(font_pirulen_24, al_map_rgb(255, 255, 255), SCREEN_W-20,20,ALLEGRO_ALIGN_RIGHT,"<<Backspace");
    
    al_draw_text(font_pirulen_32,al_map_rgb(0,0,0),option[0].x-20,option[0].y+10,ALLEGRO_ALIGN_RIGHT,option[0].Title);
    al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0),option[0].x+option[0].w/2,option[0].y+12,ALLEGRO_ALIGN_CENTER,"%d",volume);

    al_draw_text(font_pirulen_32,al_map_rgb(0,0,0),option[1].x-20,option[1].y+10,ALLEGRO_ALIGN_RIGHT,option[1].Title);
    al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0),option[1].x+option[1].w/2,option[1].y+12,ALLEGRO_ALIGN_CENTER,"%d",SE);

    al_draw_text(font_pirulen_32,al_map_rgb(0,0,0),option[2].x-20,option[2].y+10,ALLEGRO_ALIGN_RIGHT,option[2].Title);
    al_draw_textf(font_pirulen_32,al_map_rgb(0,0,0),option[2].x+option[2].w/2,option[2].y+12,ALLEGRO_ALIGN_CENTER,"%s",difficulty_string[difficulty]);

    al_draw_bitmap(img_arrow,option[option_flag].x,option[option_flag].y,ALLEGRO_FLIP_HORIZONTAL);
    al_draw_bitmap(img_arrow,option[option_flag].x+option[option_flag].w - 64,option[option_flag].y,0);
}
static void update(){

}
static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_BACKSPACE){
        game_change_scene(scene_menu_create());
    }
    if(keycode == ALLEGRO_KEY_DOWN || keycode == ALLEGRO_KEY_S){
        option_flag = (option_flag+1)%3;
    }
    if(keycode == ALLEGRO_KEY_UP || keycode == ALLEGRO_KEY_W){
        option_flag--;
        if(option_flag<0)option_flag+=3;
    }
    if(keycode == ALLEGRO_KEY_RIGHT || keycode == ALLEGRO_KEY_D){
        switch(option_flag){
            case OPTION_VOLUME:
                if(volume < 100)volume += 10;
                break;
            case OPTION_SE:
                if(SE < 100)SE += 10;
                break;
            case OPTION_DIFFICULTY:
                difficulty = (difficulty+1)%4;
                break;
        }
    }
    if(keycode == ALLEGRO_KEY_LEFT || keycode == ALLEGRO_KEY_A){
        switch(option_flag){
            case OPTION_VOLUME:
                if(volume > 0)volume -= 10;
                break;
            case OPTION_SE:
                if(SE > 0)SE -= 10;
                break;
            case OPTION_DIFFICULTY:
                difficulty--;
                if(difficulty<0)difficulty+=4;
                break;
        }
    }
}
static void destroy(){
    al_destroy_bitmap(img_background);
    al_destroy_bitmap(img_arrow);
    al_destroy_sample(bgm);
    stop_bgm(&bgm_id);
}

// The only function that is shared across files.
Scene scene_settings_create(void) {
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