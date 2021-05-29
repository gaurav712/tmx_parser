#include <stdio.h>

#include "tmx_parser.h"

#define WINDOW_WIDTH    800
#define WINDOW_HEIGHT   600

int main(int argc, char *argv[]) {

    unsigned short map_height = 0;
    unsigned short map_width = 0;
    unsigned short tile_width = 0;
    unsigned short tile_height = 0;

    struct Tileset *tileset = NULL;

    SDL_Renderer *renderer;
    SDL_Window *window;

    load_tilemap("/home/gaurav/Projects/SDL_Game/res/tilemaps/TileMap.tmx");

    get_tilemapspecs(&map_width, &map_height, &tile_width, &tile_height);

    printf("map: %ux%u, tile: %ux%u\n", map_width, map_height, tile_width, tile_height);

    load_tilesets(&tileset);

    for(short i = 0; i < tileset_count; i++) {
        printf("%hd %hd %hd %hd %hd %s\n",
            (tileset + i)->firstgid,
            (tileset + i)->tile_width,
            (tileset + i)->tile_height,
            (tileset + i)->tilecount,
            (tileset + i)->columns,
            (tileset + i)->source_img_path);
    }

    if(SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "error initializing");
        exit(1);
    }

    /* Test texture rendering */
    window = SDL_CreateWindow("Texture Test",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if(window == NULL) {
        fprintf(stderr, "window create failed\n");
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if(renderer == NULL) {
        fprintf(stderr, "renderer create failed\n");
        exit(1);
    }

    load_tileset_images(renderer, tileset);

    SDL_Event event;
    int close = 0;

    while(!close) {
        while( SDL_PollEvent( &event ) != 0 ) {
            //User requests quit
            if( event.type == SDL_QUIT )
            {
                close = 1;
            }
        }
    }

    destroy_tilemap(tileset);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
