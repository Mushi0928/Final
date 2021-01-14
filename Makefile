ALLEGRO_LIBRARIES := allegro-5 allegro_image-5 allegro_font-5 allegro_ttf-5 allegro_dialog-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5
ALLEGRO_FLAGS := $(shell pkg-config --cflags --libs $(ALLEGRO_LIBRARIES)) -lallegro -lallegro_main

CC := gcc
OUT:= main
MAIN:= main.c game.c utility.c scene_menu.c scene_settings.c scene_start.c shared.c 

all: $(MAIN)
	$(CC) -o $(OUT) $(MAIN) $(ALLEGRO_FLAGS)