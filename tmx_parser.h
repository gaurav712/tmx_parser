#pragma once

#define MAX_PATH_LEN    1024

struct Tileset {
    unsigned short firstgid;
    unsigned short tilecount;
    unsigned short columns;
    unsigned short tile_width;
    unsigned short tile_height;

    char source_img_path[MAX_PATH_LEN];
};

void load_tilemap(const char *source_file);
void get_tilemapspecs(unsigned short *width, unsigned short *height, unsigned short *tile_width, unsigned short *tile_height);
void load_tilesets(struct Tileset **tileset);
void destroy_tilemap(struct Tileset *);