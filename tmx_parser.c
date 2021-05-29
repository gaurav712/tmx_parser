#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "tmx_parser.h"

#define MAXWORDLEN  100

FILE *src;
char temp_word[MAXWORDLEN];
char rel_path[MAX_PATH_LEN];    /* Relative path to the Tilemap file */

/* Number of tilesets in the tilemap */
unsigned short tileset_count = 0;

/* Textures for the tileset images */
SDL_Texture **tileset_textures;

static inline void die_with_error(const char *error) {
    fprintf(stderr, error);
    exit(-1);
}

/* Pick out the attribute value from the string */
static void get_attr_val(const char *attr_string, char *value) {

    short index = 0, i = 0;

    /* Find the starting index for quotes */
    for(; index < strlen(attr_string); index++) {
        if(attr_string[index] == '"') {
            index++;
            break;
        }
    }

    /* Now copy until the last - 1(to ignore that last ") */
    for(; index < (strlen(attr_string) - 1); i++) {
        value[i] = attr_string[index];
        index++;
    }

    /* Fill in the trailing null */
    value[i] = '\0';
}

/* Keep reading from the file until you reach the terminal_key */
static void read_until(const char *terminal_key) {
    while((fscanf(src, "%s", temp_word)) == 1) {
        if(!(strncmp(temp_word, terminal_key, strlen(terminal_key)))) {
            break;
        }
    }
}

/* Load the source file with tilemap */
void load_tilemap(const char *source_file) {

    short index;

    if((src = fopen(source_file, "r")) == NULL) {
        die_with_error("failed to load source file!");
    }

    /* Fill in the relative path(excluding the filename) */
    for(index = (strlen(source_file) - 1); source_file[index] != '/' && source_file[index] != '\\'; index--);

    strncpy(rel_path, source_file, index + 1);
}

/* Get the tilemap specs */
void get_tilemapspecs(
    unsigned short *map_width,
    unsigned short *map_height,
    unsigned short *tile_width,
    unsigned short *tile_height
    ) {

    /* Step into the <map> tag and find the dimension for the map and tiles*/
    read_until("width=");

    get_attr_val(temp_word, temp_word);
    *map_width = atoi(temp_word);

    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    *map_height = atoi(temp_word);

    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    *tile_width = atoi(temp_word);

    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    *tile_height = atoi(temp_word);
}

static void fill_in_tileset_info(struct Tileset *tileset) {

    /* To make the following assignments cleaner */
    struct Tileset *struct_with_offset = (struct Tileset *)(tileset + (tileset_count - 1));

    /* Get the firstgid */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    struct_with_offset->firstgid = atoi(temp_word);

    /* tile_width */
    read_until("tilewidth");
    get_attr_val(temp_word, temp_word);
    struct_with_offset->tile_width = atoi(temp_word);

    /* tile_height */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    struct_with_offset->tile_height = atoi(temp_word);

    /* tilecount */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    struct_with_offset->tilecount = atoi(temp_word);

    /* columns */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    struct_with_offset->columns = atoi(temp_word);

    /* Image source path */
    read_until("source=");
    get_attr_val(temp_word, temp_word);

    /* Make the path relative to the tilemap */
    strcpy(struct_with_offset->source_img_path, rel_path);
    strcat(struct_with_offset->source_img_path, temp_word);
}

void load_tilesets(struct Tileset **tileset) {

    while (1) {
        /* Read the next word */
        fscanf(src, "%s", temp_word);

        /* If it is an initial <layer> tag, exit the loop */
        if(!(strncmp(temp_word, "<layer", 6))) {
            break;
        }

        /* If it is an initial <tileset> tag, add it to the Tileset linked list */
        if(!(strncmp(temp_word, "<tileset", 8))) {

            tileset_count++;

            /* Allocate for the new entry */
            *tileset = (struct Tileset *)realloc(*tileset, tileset_count * sizeof(struct Tileset));

            /* Fill it in */
            fill_in_tileset_info(*tileset);
        }
    }
}

/* Load images' textures */
void load_tileset_images(SDL_Renderer *renderer, struct Tileset *tileset) {

    SDL_Surface *temp_surface;

    /* Allocated for the texture pointers */
    tileset_textures = (SDL_Texture **) malloc(tileset_count * sizeof(SDL_Texture *));

    for(short count = 0; count < tileset_count; count++) {
        temp_surface = IMG_Load((tileset + count)->source_img_path);
        tileset_textures[count] = SDL_CreateTextureFromSurface(renderer, temp_surface);
        SDL_FreeSurface(temp_surface);
    }
}

void destroy_tilemap(struct Tileset *tileset) {
    fclose(src);
    free(tileset);

    /* Destroy textures */
    for(short count = 0; count < tileset_count; count++) {
        SDL_DestroyTexture(tileset_textures[count]);
    }
}
