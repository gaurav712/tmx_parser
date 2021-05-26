#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <png.h>

#include "tmx_parser.h"

#define MAXWORDLEN  100

FILE *src;
char temp_word[MAXWORDLEN];
unsigned short tileset_count = 0;

/* Pixel array for the tileset image */
png_bytepp pixel_data;

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
    if((src = fopen(source_file, "r")) == NULL) {
        die_with_error("failed to load source file!");
    }
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
    strncpy(struct_with_offset->source_img_path, temp_word, strlen(temp_word));
}

static void load_tileset_image(const char *image_path) {

    int height, width;

    FILE *fp;
    png_structp png_ptr;
    png_infop info_ptr;

    /* load the tileset image file */
    fp = fopen(image_path, "r");

    /* Initialize libpng */
    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    
    /* Load the image */
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    pixel_data = png_get_rows(png_ptr, info_ptr);

    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);

    fclose(fp);
}

void load_tilesets(struct Tileset **tileset) {

    /* Set the tmp_ptr to the start of *tileset */

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

    load_tileset_image("/home/gaurav/Projects/SDL_Game/res/tilemaps/city_outline.png");
}

void destroy_tilemap(struct Tileset *tileset) {
    fclose(src);
    free(tileset);
}
