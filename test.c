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
    struct Layer *layers = NULL;

    SDL_Renderer *renderer;
    SDL_Window *window;

    struct Tileset *temp_ptr;

    load_tilemap("/home/gaurav/Projects/SDL_Game/res/tilemaps/TileMap.tmx");

    get_tilemapspecs(&map_width, &map_height, &tile_width, &tile_height);

    printf("map: %ux%u, tile: %ux%u\n", map_width, map_height, tile_width, tile_height);

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

    load_tilesets(&tileset, renderer);

    for(temp_ptr = tileset; temp_ptr; temp_ptr = temp_ptr->next) {
        printf("%hd %hd %hd %hd %hd\n",
            temp_ptr->firstgid,
            temp_ptr->tile_width,
            temp_ptr->tile_height,
            temp_ptr->tilecount,
            temp_ptr->columns);
    }

    load_layers(&layers);

    /* Print layer data */
    for(unsigned short row = 0; row < layers->height; row++) {
        for(unsigned short col = 0; col < layers->width; col++) {
            printf("%hd ", (layers->layer_data)[row][col]);
        }
        printf("\n");
    }

    SDL_Event event;
    int close = 0;

    /* Test the tectures */
    SDL_Rect dest_rect = {0, 0, 32, 32};
    SDL_Rect src_rect = {0, 0, 32, 32};

    while(!close) {
        while( SDL_PollEvent( &event ) != 0 ) {
            //User requests quit
            if( event.type == SDL_QUIT )
            {
                close = 1;
            }
        }
        for(short row = 0; row < WINDOW_HEIGHT; row+=32) {
            dest_rect.y = row;
            for(short col = 0; col < WINDOW_WIDTH; col+=32) {
                dest_rect.x = col;
                SDL_RenderCopy(renderer, tileset->src_image_texture, &src_rect, &dest_rect);
            }
        }

        SDL_RenderPresent(renderer);
    }

    destroy_tilemap(tileset, layers);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
