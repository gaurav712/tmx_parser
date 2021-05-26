#include <stdio.h>

#include "tmx_parser.h"

int main(void) {

    unsigned short map_height = 0;
    unsigned short map_width = 0;
    unsigned short tile_width = 0;
    unsigned short tile_height = 0;

    struct Tileset *tileset = NULL;

    load_tilemap("/home/gaurav/Projects/SDL_Game/res/tilemaps/TileMap.tmx");

    get_tilemapspecs(&map_width, &map_height, &tile_width, &tile_height);

    printf("map: %ux%u, tile: %ux%u\n", map_width, map_height, tile_width, tile_height);

    load_tilesets(&tileset);

    printf("%hd %hd %hd %hd %hd %s\n",
        tileset->firstgid, tileset->tile_width, tileset->tile_height, tileset->tilecount, tileset->columns,
        tileset->source_img_path);

    destroy_tilemap(tileset);
    return 0;
}
