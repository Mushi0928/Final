#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include "scene_menu.h"
#include <math.h>

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_BITMAP* img_bullet_left;
static ALLEGRO_BITMAP* img_bullet_right;



static void init(void);
static void update(void);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void destroy(void);

#define MAX_ENEMY 20
#define MAX_BULLET 20
static Mage mage;
static MovableObject bullets[MAX_BULLET];

static const float MAX_COOLDOWN = 0.1f;
static double last_shoot_timestamp;
static const float JUMP_COOLDOWN = 0.5f;
static double last_jump_timestamp;
static const float JUMP_SPEED = -5.0f;
static int jump_count = 2;

static const float G = -0.2f;

static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static bool draw_gizmos;

static void init(void) {
    int i;
    img_background = load_bitmap_resized("Resource/start-bg.jpg", SCREEN_W, SCREEN_H);

    mage.vx = 0;
    mage.vy = 0;
    mage.w = 60;
    mage.h = 90;
    mage.x = 200;
    mage.y = SCREEN_H - mage.h/2;
    mage.sprite_w = 256;
    mage.sprite_h = 256;
    mage.hp = 100;
    mage.attack = 5;
    mage.spritesheet = al_load_bitmap("Resource/mage_big.png");

    img_bullet_left = al_load_bitmap("Resource/bullet_left.png");
    img_bullet_right = al_load_bitmap("Resource/bullet_right.png");

    for (i = 0; i < MAX_BULLET ;i++) {
        bullets[i].img = img_bullet_right;
        bullets[i].w = al_get_bitmap_width(img_bullet_right);
        bullets[i].h = al_get_bitmap_height(img_bullet_right);
        bullets[i].vx = 10;
        bullets[i].vy = 0;
        bullets[i].hidden = true;
    }
    // Can be moved to shared_init to decrease loading time.
    bgm = load_audio("Resource/mythica.ogg");
    game_log("Start scene initialized");
    bgm_id = play_bgm(bgm, 1);
}

static void update(void) {
    mage.vx = 0;
    /*
    if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
        mage.vy += 1;
    */
    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
        mage.vx -= 1;
    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
        mage.vx += 1;
     
    int i;
    for (i = 0; i < MAX_BULLET ;i++) {
        if (bullets[i].hidden)
            continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        if (bullets[i].x + bullets[i].w < 0 || bullets[i].x - bullets[i].w > SCREEN_W)
            bullets[i].hidden = true;
    }

    if(mage.vx > 0 || mage.direction == ATTACK_RIGHT)mage.direction = RIGHT;
    if(mage.vx < 0 || mage.direction == ATTACK_LEFT)mage.direction = LEFT;
    if(key_state[ALLEGRO_KEY_SPACE]){
        if(mage.direction == RIGHT)mage.direction = ATTACK_RIGHT;
        if(mage.direction == LEFT)mage.direction = ATTACK_LEFT;
    }

    double now = al_get_time();
    if (key_state[ALLEGRO_KEY_SPACE] && now - last_shoot_timestamp >= MAX_COOLDOWN) {
        for (int i = 0;i < MAX_BULLET;i++) {
            if (bullets[i].hidden) {
                last_shoot_timestamp = now;
                bullets[i].hidden = false;
                if(mage.direction == ATTACK_RIGHT){
                    bullets[i].x = mage.x + 35;
                    bullets[i].y = mage.y + 25;
                    bullets[i].vx = 10;
                    bullets[i].img = img_bullet_right;
                }else if(mage.direction == ATTACK_LEFT){
                    bullets[i].x = mage.x - 35;
                    bullets[i].y = mage.y + 25;
                    bullets[i].vx = -10;
                    bullets[i].img = img_bullet_left;
                }
                break;
            }
        }
    }

    if ((key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W]) && now - last_jump_timestamp >= JUMP_COOLDOWN && jump_count > 0){
        last_jump_timestamp = now;
        mage.vy = JUMP_SPEED;
        jump_count--;
    }
    
    mage.y += mage.vy * 4;
    mage.x += mage.vx * 4;
    mage.vy -= G;

    if (mage.x - mage.w / 2 < 0)
        mage.x = mage.w / 2;
    else if (mage.x + mage.w / 2 > SCREEN_W)
        mage.x = SCREEN_W - mage.w / 2;
    if (mage.y - mage.h / 2 < 0)
        mage.y = mage.h / 2;
    else if (mage.y + mage.h / 2 > SCREEN_H){
        mage.y = SCREEN_H - mage.h / 2;
        jump_count = 2;
    }
}

static void draw_movable_object(MovableObject obj) {
    if (obj.hidden)
        return;
    al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
    if (draw_gizmos) {
        al_draw_rectangle(round(obj.x - obj.w / 2), round(obj.y - obj.h / 2),
            round(obj.x + obj.w / 2) + 1, round(obj.y + obj.h / 2) + 1, al_map_rgb(255, 0, 0), 0);
    }
}

static void draw(void) {
    int i;
    al_draw_bitmap(img_background, 0, 0, 0);
    al_draw_bitmap_region(mage.spritesheet, mage.sprite_w * (mage.direction%4),mage.sprite_h*(mage.direction/4),
                          mage.sprite_w,mage.sprite_h,
                          mage.x - mage.sprite_w/2,mage.y - (mage.sprite_h - mage.h/2),0);
    for (i = 0; i < MAX_BULLET ;i++)
        draw_movable_object(bullets[i]);
}

static void destroy(void) {
    al_destroy_bitmap(img_background);
    al_destroy_sample(bgm);
    al_destroy_bitmap(img_bullet_left);
    al_destroy_bitmap(img_bullet_right);
    al_destroy_bitmap(mage.spritesheet);
    stop_bgm(bgm_id);
    game_log("Start scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_TAB)
        draw_gizmos = !draw_gizmos;
    if (keycode == ALLEGRO_KEY_BACKSPACE)
        game_change_scene(scene_menu_create());
}

Scene scene_start_create(void) {
    Scene scene;
    memset(&scene, 0, sizeof(Scene));
    scene.name = "Start";
    scene.initialize = &init;
    scene.update = &update;
    scene.draw = &draw;
    scene.destroy = &destroy;
    scene.on_key_down = &on_key_down;
    // TODO: Register more event callback functions such as keyboard, mouse, ...
    game_log("Start scene created");
    return scene;
}