#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

#include "tmx_parser.h"

#define MAXWORDLEN  100

FILE *src;
char temp_word[MAXWORDLEN];
char rel_path[MAX_PATH_LEN];    /* Relative path to the Tilemap file */

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

/* Load images' textures */
static inline void load_tileset_texture(SDL_Renderer *renderer, SDL_Texture **texture, const char *source_img_path) {

    SDL_Surface *temp_surface;

    temp_surface = IMG_Load(source_img_path);
    *texture = SDL_CreateTextureFromSurface(renderer, temp_surface);
    SDL_FreeSurface(temp_surface);
}

static void fill_in_tileset_info(struct Tileset *tileset, SDL_Renderer *renderer) {

    char temp_str[MAX_PATH_LEN];

    /* Get the firstgid */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    tileset->firstgid = atoi(temp_word);

    /* tile_width */
    read_until("tilewidth");
    get_attr_val(temp_word, temp_word);
    tileset->tile_width = atoi(temp_word);

    /* tile_height */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    tileset->tile_height = atoi(temp_word);

    /* tilecount */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    tileset->tilecount = atoi(temp_word);

    /* columns */
    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    tileset->columns = atoi(temp_word);

    /* Load the texture */
    read_until("source=");
    get_attr_val(temp_word, temp_word);

    /* Make the path relative to the tilemap */
    strcpy(temp_str, rel_path);
    strcat(temp_str, temp_word);

    load_tileset_texture(renderer, &(tileset->src_image_texture), temp_str);
}

void load_tilesets(struct Tileset **tileset, SDL_Renderer *renderer) {

    struct Tileset *temp_ptr;
    Uint8 initialized = 0;

    while (1) {
        /* Read the next word */
        fscanf(src, "%s", temp_word);

        /* If it is an initial <layer> tag, exit the loop */
        if(!(strncmp(temp_word, "<layer", 6))) {
            break;
        }

        /* If it is an initial <tileset> tag, add it to the Tileset linked list */
        if(!(strncmp(temp_word, "<tileset", 8))) {

            /* Allocate for the new entry */
            if(!initialized) {
                temp_ptr = (struct Tileset *)malloc(sizeof(struct Tileset));
                *tileset = temp_ptr;
                initialized = 1;
            } else {
                temp_ptr->next = (struct Tileset *)malloc(sizeof(struct Tileset));
                temp_ptr = temp_ptr->next;
            }

            /* Fill it in */
            fill_in_tileset_info(temp_ptr, renderer);

            temp_ptr->next = NULL;
        }
    }

    /* Push back the "<layer" tag to the stream for the parsing of layers */
    for(short i = 5; i >= 0; i--) {
        ungetc(temp_word[i], src);
    }
}

static void fill_in_layer_data(struct Layer *layers) {

    /* Get the width and height of the layer */
    read_until("width=");
    get_attr_val(temp_word, temp_word);
    layers->width = atoi(temp_word);

    fscanf(src, "%s", temp_word);
    get_attr_val(temp_word, temp_word);
    layers->height = atoi(temp_word);

    /* Allocate for the rows */
    layers->layer_data = (unsigned short **) malloc(sizeof(unsigned short *) * layers->height);
    /* Now for each columns */
    for(unsigned short count = 0; count < layers->height; count++) {
        (layers->layer_data)[count] = (unsigned short *) malloc(sizeof(unsigned short) * layers->width);
    }
    
    read_until("encoding=\"csv\">");
    
    /* Read the numbers and fill them in */
    for(unsigned short row = 0; row < layers->height; row++) {
        for(unsigned short col = 0; col < layers->width; col++) {
            fscanf(src, "%hd,", &(layers->layer_data)[row][col]);
        }
    }
}

/* Load Layer data */
void load_layers(struct Layer **layers) {

    struct Layer *temp_ptr;
    Uint8 initialized = 0;

    while (1) {
        /* Read the next word */
        fscanf(src, "%s", temp_word);

        /* If its the end of tilemap, exit the loop */
        if(!(strncmp(temp_word, "</map>", 6))) {
            break;
        }

        /* If it is an initial <layer> tag, add it to the layers linked list */
        if(!(strncmp(temp_word, "<layer", 6))) {

            /* Allocate for the new entry */
            if(!initialized) {
                temp_ptr = (struct Layer *)malloc(sizeof(struct Layer));
                *layers = temp_ptr;
                initialized = 1;
            } else {
                temp_ptr->next = (struct Layer *)malloc(sizeof(struct Layer));
                temp_ptr = temp_ptr->next;
            }

            /* Fill it in */
            fill_in_layer_data(temp_ptr);

            temp_ptr->next = NULL;
        }
    }
}

void destroy_tilemap(struct Tileset *tileset, struct Layer *layers) {

    struct Tileset *tileset_tmp_ptr;
    struct Layer *layer_tmp_ptr;

    fclose(src);

    while(tileset) {
        SDL_DestroyTexture(tileset->src_image_texture);
        tileset_tmp_ptr = tileset->next;
        free(tileset);
        tileset = tileset_tmp_ptr;
    }

    while(layers) {
        for(unsigned short count = 0; count < layers->height; count++) {
            free((layers->layer_data)[count]);
        }
        free(layers->layer_data);
        layer_tmp_ptr = layers->next;
        free(tileset);
        layers = layer_tmp_ptr;
    }
}
