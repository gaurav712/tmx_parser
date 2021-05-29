#pragma once

#include<SDL2/SDL.h>

#define MAX_PATH_LEN    1024

struct Tileset {
    unsigned short firstgid;
    unsigned short tile_width;
    unsigned short tile_height;
    unsigned short tilecount;
    unsigned short columns;

    char source_img_path[MAX_PATH_LEN];
};

extern unsigned short tileset_count;
extern SDL_Texture **tileset_textures;

void load_tilemap(const char *source_file);
void get_tilemapspecs(unsigned short *width, unsigned short *height, unsigned short *tile_width, unsigned short *tile_height);
void load_tilesets(struct Tileset **tileset);
void load_tileset_images(SDL_Renderer *renderer, struct Tileset *tileset);
void destroy_tilemap(struct Tileset *);
