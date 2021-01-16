#include "scene_start.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "game.h"
#include "utility.h"
#include "scene_menu.h"
#include "shared.h"
#include <math.h>
#include <stdlib.h>

static ALLEGRO_BITMAP* img_background;
static ALLEGRO_BITMAP* img_bullet_left;
static ALLEGRO_BITMAP* img_bullet_right;
static ALLEGRO_BITMAP* img_grass_ground;
static ALLEGRO_BITMAP* img_health_bar;
static ALLEGRO_BITMAP* img_health_bar_border;
static ALLEGRO_BITMAP* img_skeleton;

static const int BLOCK_WIDTH = 128;

static void init(void);
static void update(void);
static void update_skeleton(Enemy* skeleton);
static void draw_movable_object(MovableObject obj);
static void draw(void);
static void draw_block(BlockCollider block);
static void draw_enemy(Enemy enemy);
static void init_mage();
static void init_block();
static void init_skeleton(Enemy* skeleton);
static void destroy(void);
static bool rect_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2);

#define MAX_ENEMY 20
#define MAX_BULLET 20
#define MAX_BLOCK 10
static Mage mage;
static MovableObject bullets[MAX_BULLET];
static Enemy enemies[MAX_ENEMY];
static int Score;
static bool ended = false;

static double spawn_delay = 3.0f;
static double last_spawn_timestamp;
static int spawn_area_flag = 0;
static float spawn_area[4][2];
static const float MAX_COOLDOWN = 0.1f;
static double last_shoot_timestamp;
static const float JUMP_COOLDOWN = 0.4f;
static double last_jump_timestamp;
static const float JUMP_SPEED = -5.0f;
static int jump_count = 2;
static const float SKELETON_JUMP_SPEED = -5;

static const float G = 0.2f;

static BlockCollider blocks[MAX_BLOCK];

static ALLEGRO_SAMPLE* bgm;
static ALLEGRO_SAMPLE_ID bgm_id;
static ALLEGRO_SAMPLE* dying;
static ALLEGRO_SAMPLE_ID dying_id;

static bool draw_gizmos;

static void init(void) {
    spawn_area[0][0] = 64;
    spawn_area[0][1] = 178;
    spawn_area[1][0] = SCREEN_W - 64;
    spawn_area[1][1] = 178;
    spawn_area[2][0] = 64;
    spawn_area[2][1] = 690;
    spawn_area[3][0] = SCREEN_W - 64;
    spawn_area[3][1] = 690;

    Score = 0;
    ended = false;

    int i;
    //load images
    img_background = load_bitmap_resized("Resource/start-bg.jpg", SCREEN_W, SCREEN_H);
    img_bullet_left = al_load_bitmap("Resource/bullet_left.png");
    img_bullet_right = al_load_bitmap("Resource/bullet_right.png");
    img_grass_ground = al_load_bitmap("Resource/grassGround.png");
    img_health_bar = al_load_bitmap("Resource/health_bar.png");
    img_health_bar_border = al_load_bitmap("Resource/health_bar_border.png");
    img_skeleton = al_load_bitmap("Resource/skeleton.png");
    //init mage
    init_mage();
    //init bullets
    for (i = 0; i < MAX_BULLET ;i++) {
        bullets[i].img = img_bullet_right;
        bullets[i].w = al_get_bitmap_width(img_bullet_right);
        bullets[i].h = al_get_bitmap_height(img_bullet_right);
        bullets[i].vx = 10;
        bullets[i].vy = 0;
        bullets[i].hidden = true;
    }
    //init blocks
    for(i = 0;i<MAX_BLOCK;i++){
        blocks[i].hidden = true;
    }
    init_block();
    //init enemies
    for(i = 0;i<MAX_ENEMY;i++){
        enemies[i].hidden = true;
    }
    //init_skeleton(&enemies[0]);
    // Can be moved to shared_init to decrease loading time.
    bgm = load_audio("Resource/misty_dungeon.wav");
    dying = load_audio("Resource/dying_sound.wav");
    game_log("Start scene initialized");
    play_bgm(bgm,&bgm_id, 1);
}

static void update(void) {
    if(ended)return;
    float temp_vx = 0;
    if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
        temp_vx -= 1;
    if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
        temp_vx += 1;
    if (key_state[ALLEGRO_KEY_Q])
        mage.momentum_x  = 5;
    mage.vx = temp_vx + mage.momentum_x;
    mage.y += mage.vy * 4;
    mage.x += mage.vx * 4;
    if(mage.momentum_x > 0)mage.momentum_x -= 1;
    else if(mage.momentum_x < 0)mage.momentum_x += 1;
    else mage.momentum_x = 0;
    //gravity
    mage.vy += G;

    int i;
    for (i = 0; i < MAX_BULLET ;i++) {
        if (bullets[i].hidden)
            continue;
        bullets[i].x += bullets[i].vx;
        bullets[i].y += bullets[i].vy;
        for(int j = 0;j<MAX_ENEMY;j++){
            if(!enemies[j].hidden){
                if(rect_collision(bullets[i].x - bullets[i].w/2,bullets[i].y - bullets[i].h/2,bullets[i].w,bullets[i].h,
                                  enemies[j].x - enemies[j].w/2,enemies[j].y,enemies[j].w,enemies[j].h)){
                    enemies[j].hp -= mage.attack;
                    bullets[i].hidden = true;
                }
            }
        }
        
        if (bullets[i].x + bullets[i].w < 0 || bullets[i].x - bullets[i].w > SCREEN_W)
            bullets[i].hidden = true;
    }
    if(mage.vx > 0 || mage.direction == ATTACK_RIGHT)mage.direction = RIGHT;
    if(mage.vx < 0 || mage.direction == ATTACK_LEFT)mage.direction = LEFT;
    if(key_state[ALLEGRO_KEY_SPACE]){
        if(mage.direction == RIGHT)mage.direction = ATTACK_RIGHT;
        if(mage.direction == LEFT)mage.direction = ATTACK_LEFT;
    }
    //Shooting
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
    //Spawning skeleton
    if(now - last_spawn_timestamp >= spawn_delay){
        for(i = 0;i<MAX_ENEMY;i++){
            if(enemies[i].hidden){
                init_skeleton(&enemies[i]);
                last_spawn_timestamp = now;
                break;
            }
        }
    }
    //enemy movement
    for(int i = 0;i<MAX_ENEMY;i++){
        if(!enemies[i].hidden)
            switch (enemies[i].enemy_type){
                case ENEMY_SKELETON:
                    update_skeleton(&enemies[i]);
                    break;
            }
    }
    //make sure the mage is inside the screen
    if (mage.x - mage.w / 2 < 0)
        mage.x = mage.w / 2;
    else if (mage.x + mage.w / 2 > SCREEN_W)
        mage.x = SCREEN_W - mage.w / 2;
    if (mage.y + mage.h / 2 > SCREEN_H){
        mage.y = SCREEN_H - mage.h / 2;
        mage.vy = 0;
        jump_count = 2;
    }
    //ground collision
    for(int i = 0;i<MAX_BLOCK;i++){
        if(!blocks[i].hidden){
            if(mage.vy > 0 && (mage.y + mage.h/2 - mage.vy * 8) <= blocks[i].y){
                bool left_corner = pnt_in_rect(mage.x-mage.w/2,mage.y + mage.h/2,
                                            blocks[i].x,blocks[i].y,blocks[i].w,blocks[i].h);
                bool right_corner = pnt_in_rect(mage.x+mage.w/2,mage.y + mage.h/2,
                                            blocks[i].x,blocks[i].y,blocks[i].w,blocks[i].h);
                if(left_corner || right_corner){
                    mage.y = blocks[i].y - mage.h/2;
                    jump_count = 2;
                    mage.vy = 0;
                }                               
            }
            
            
        }
    }
    //checking if the mage is alive
    if(mage.hp <= 0){
        ended = true;
        stop_bgm(&bgm_id);
        al_play_sample(dying,1,0,1,ALLEGRO_PLAYMODE_ONCE,&dying_id);
    }
}
static void update_skeleton(Enemy* skeleton){
    if((*skeleton).hp <= 0){
        (*skeleton).hidden = true;
        Score += 50;
        return;
    }
    (*skeleton).vx = ((*skeleton).x > mage.x)?-1:1;
    if((*skeleton).x > mage.x){
        (*skeleton).vx = -1;
        (*skeleton).direction = LEFT;
    }else if((*skeleton).x < mage.x){
        (*skeleton).vx = 1;
        (*skeleton).direction = RIGHT;
    }else{
        (*skeleton).vx = 0;
    }
    (*skeleton).x += (*skeleton).vx * (*skeleton).speed;
    (*skeleton).y += (*skeleton).vy * 4;
    (*skeleton).vy += G;
    for(int i = 0;i<MAX_BLOCK;i++){
        if(!blocks[i].hidden){
            if((*skeleton).vy > 0 && ((*skeleton).y + (*skeleton).h/2 - (*skeleton).vy * 8) <= blocks[i].y){
                bool left_corner = pnt_in_rect((*skeleton).x-(*skeleton).w/2,(*skeleton).y + (*skeleton).h/2,
                                                blocks[i].x,blocks[i].y,blocks[i].w,blocks[i].h);
                bool right_corner = pnt_in_rect((*skeleton).x+(*skeleton).w/2,(*skeleton).y + (*skeleton).h/2,
                                            blocks[i].x,blocks[i].y,blocks[i].w,blocks[i].h);
                if(left_corner || right_corner){
                    (*skeleton).y = blocks[i].y - (*skeleton).h/2;
                    (*skeleton).vy = 0;
                }                               
            }
            if((*skeleton).vy <= 1 && (*skeleton).y > mage.y){
                if((*skeleton).direction == LEFT){
                    if(pnt_in_rect((*skeleton).x - (*skeleton).w/2,(*skeleton).y + (*skeleton).h/2,
                                   blocks[i].x + blocks[i].w - 10,blocks[i].y + blocks[i].h - 10,10,10)){
                        (*skeleton).vy = SKELETON_JUMP_SPEED;
                    }
                }else{
                    if(pnt_in_rect((*skeleton).x + (*skeleton).w/2,(*skeleton).y + (*skeleton).h/2,
                                   blocks[i].x , blocks[i].y + blocks[i].h - 10,10,10)){
                        (*skeleton).vy = SKELETON_JUMP_SPEED;
                    }
                }
            }
        }
    }
    if(rect_collision((*skeleton).x - (*skeleton).w/2,(*skeleton).y - (*skeleton).h/2,(*skeleton).w,(*skeleton).h,
                      mage.x - mage.w/2,mage.y - mage.h/2,mage.w,mage.h)){
        mage.hp -= (*skeleton).attack;
        if((*skeleton).x >= mage.x){
            mage.momentum_x = -8;
        }else{
            mage.momentum_x = 8;
        }
    }   
}
static void draw(void) {
    int i;
    al_draw_bitmap(img_background, 0, 0, 0);
    for(int i = 0;i<MAX_BLOCK;i++){
        if(!blocks[i].hidden)
            draw_block(blocks[i]);
            if(draw_gizmos)
            al_draw_rectangle(round(blocks[i].x), round(blocks[i].y),
                round(blocks[i].x+blocks[i].w), round(blocks[i].y+blocks[i].h), al_map_rgb(255, 0, 0), 0);
    }
    al_draw_bitmap_region(mage.spritesheet, mage.sprite_w * (mage.direction%4),mage.sprite_h*(mage.direction/4),
                          mage.sprite_w,mage.sprite_h,
                          mage.x - mage.sprite_w/2,mage.y - (mage.sprite_h - mage.h/2),0);
    for (i = 0; i < MAX_BULLET ;i++)
        draw_movable_object(bullets[i]); 
    for(int i = 0 ; i < MAX_ENEMY;i++){
        if(!enemies[i].hidden){
            draw_enemy(enemies[i]);
        }
    }
    al_draw_bitmap(img_health_bar_border,20,20,0);
    al_draw_bitmap_region(img_health_bar,3,0,((float)mage.hp/mage.max_hp)*158,22,23,20,0);
    if(!ended)
        al_draw_textf(font_pirulen_32,al_map_rgb(0, 0, 0),SCREEN_W/2,0,ALLEGRO_ALIGN_CENTER,"Score : %d",Score);
    else{
        al_draw_text(font_pirulen_180,al_map_rgb(0, 0, 0),SCREEN_W/2,200,ALLEGRO_ALIGN_CENTER,"YOU DIED");
        al_draw_textf(font_pirulen_60,al_map_rgb(0, 0, 0),SCREEN_W/2,SCREEN_H/3,ALLEGRO_ALIGN_CENTER,"Score : %d",Score);
        al_draw_text(font_pirulen_32,al_map_rgb(0, 0, 0),SCREEN_W,SCREEN_H-160,ALLEGRO_ALIGN_RIGHT,"ENTER>>");
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
static void draw_block(BlockCollider block){
    for(int i = 0;i<block.w/BLOCK_WIDTH;i++){
        al_draw_bitmap(img_grass_ground,block.x+i*BLOCK_WIDTH,block.y,0);
    }
}
static void draw_enemy(Enemy enemy){
    int draw_argu;
    switch(enemy.direction){
        case LEFT:
            draw_argu = ALLEGRO_FLIP_HORIZONTAL;
            break;
        case RIGHT:
            draw_argu = 0;
            break;
    }
    al_draw_bitmap_region(enemy.sprite,0,0,300,300,
                          enemy.x - enemy.sprite_w/2,enemy.y - enemy.sprite_h + enemy.h/2,draw_argu);
    al_draw_bitmap(img_health_bar_border,enemy.x - 83,enemy.y + enemy.h/2 + 5,0);
    al_draw_bitmap_region(img_health_bar,3,0,((float)enemy.hp/enemy.max_hp)*158,22,enemy.x - 83,enemy.y + enemy.h/2 + 5,0);
    if(draw_gizmos)
        al_draw_rectangle(round(enemy.x - enemy.w/2),round(enemy.y - enemy.h/2),
                          round(enemy.x + enemy.w/2),round(enemy.y + enemy.h/2),
                          al_map_rgb(255, 0, 0), 0);
}

static void destroy(void) {
    al_destroy_bitmap(img_background);
    al_destroy_sample(bgm);
    al_destroy_sample(dying);
    al_destroy_bitmap(img_bullet_left);
    al_destroy_bitmap(img_bullet_right);
    al_destroy_bitmap(mage.spritesheet);
    al_destroy_bitmap(img_grass_ground);
    al_destroy_bitmap(img_health_bar);
    al_destroy_bitmap(img_health_bar_border);
    al_destroy_bitmap(img_skeleton);
    game_log("Start scene destroyed");
}

static void on_key_down(int keycode) {
    if (keycode == ALLEGRO_KEY_TAB)
        draw_gizmos = !draw_gizmos;
    if (keycode == ALLEGRO_KEY_BACKSPACE)
        game_change_scene(scene_menu_create());
    if (ended && keycode == ALLEGRO_KEY_ENTER)
        game_change_scene(scene_menu_create());
}
static void init_mage(){
    mage.vx = 0;
    mage.vy = 0;
    mage.momentum_x = 0;
    mage.w = 60;
    mage.h = 90;
    mage.x = SCREEN_W/2;
    mage.y = SCREEN_H - mage.h/2 - BLOCK_WIDTH;
    mage.sprite_w = 256;
    mage.sprite_h = 256;
    mage.hp = 100;
    mage.max_hp = 100;
    mage.attack = 10;
    mage.spritesheet = al_load_bitmap("Resource/mage_big.png");
}
static void init_block(){
    int ground[8][4] = {
                        {0,0,13,1},
                        {4,3,5,1},
                        {9,4,4,1},
                        {0,4,4,1},
                        {2,1,1,1},
                        {3,2,1,1},
                        {10,1,1,1},
                        {9,2,1,1},
                    };
    for(int i = 0;i<8;i++){
        blocks[i].x = ground[i][0]*BLOCK_WIDTH;
        blocks[i].y = SCREEN_H - (ground[i][1]+1)*BLOCK_WIDTH;
        blocks[i].w = ground[i][2]*BLOCK_WIDTH;
        blocks[i].h = ground[i][3]*BLOCK_WIDTH;
        blocks[i].hidden = false;
    }
}
static void init_skeleton(Enemy* skeleton){
    (*skeleton).x = spawn_area[spawn_area_flag][0];
    (*skeleton).y = spawn_area[spawn_area_flag][1];
    (*skeleton).w = 40;
    (*skeleton).h = 100;
    (*skeleton).sprite_w = 300;
    (*skeleton).sprite_h = 300;
    (*skeleton).vx = 0;
    (*skeleton).vy = 0;
    (*skeleton).speed = 2 + ((float)(rand()%10))/10;
    (*skeleton).direction = LEFT;
    (*skeleton).max_hp = 100;
    (*skeleton).hp = (*skeleton).max_hp;
    (*skeleton).attack = 5;
    (*skeleton).sprite = img_skeleton;
    (*skeleton).enemy_type = ENEMY_SKELETON;
    (*skeleton).last_hit_timestamp = 0.0f;
    (*skeleton).hidden = false;
    spawn_area_flag = (spawn_area_flag+1)%4;
}

static bool rect_collision(float x1, float y1, float w1, float h1, float x2, float y2, float w2, float h2){
    if(pnt_in_rect(x1   ,y1   ,x2,y2,w2,h2))return true;
    if(pnt_in_rect(x1+w1,y1   ,x2,y2,w2,h2))return true;
    if(pnt_in_rect(x1   ,y1+h1,x2,y2,w2,h2))return true;
    if(pnt_in_rect(x1+w1,y1+h2,x2,y2,w2,h2))return true;
    return false;
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