#pragma once

#include<SDL2/SDL.h>

#define MAX_PATH_LEN    1024

struct Tileset {
    unsigned short firstgid;
    unsigned short tile_width;
    unsigned short tile_height;
    unsigned short tilecount;
    unsigned short columns;

    /* Source to the tileset image */
    SDL_Texture *src_image_texture;

    /* To the next node */
    struct Tileset *next;
};

struct Layer {
    unsigned short width;
    unsigned short height;

    /* CSV data for the layer */
    unsigned short **layer_data;

    /* To the next node */
    struct Layer *next;
};

void load_tilemap(const char *source_file);
void get_tilemapspecs(unsigned short *width, unsigned short *height, unsigned short *tile_width, unsigned short *tile_height);
void load_tilesets(struct Tileset **tileset, SDL_Renderer *renderer);
void load_layers(struct Layer **layers);
void destroy_tilemap(struct Tileset *, struct Layer *layers);
