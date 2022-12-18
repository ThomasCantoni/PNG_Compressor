#include "lodepng.c"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "LinkedListElements.h"

#define PNG_SIGNATURE "\x89PNG\r\n\x1a\n"
typedef struct chunk
{
    unsigned int data_length;
    unsigned char* type;
    unsigned char* data;
    unsigned int crc;
} chunk;
typedef struct chunk_node
{
    chunk* chunk;
    list_double_node* node;
} chunk_node;
typedef struct png_image
{
  char* filename;
  size_t size;
  char* raw_data;
  unsigned int width;
  unsigned int height;
  chunk_node* IHDR;
  chunk_node* node_list;
} png_image;
size_t get_file_length( FILE* file_to_open )
{
    fseek(file_to_open,0,SEEK_END);
    size_t len = ftell(file_to_open);
    rewind(file_to_open);
    return len;
}
void display_raw_image_data(png_image* image)
{
    for (size_t i = 0; i < image->size;++i)
    {
        printf("0x%i \t",image->raw_data[i]);
       if((i +1)% 16 == 0)
       {
            printf("|\n");
       }
    }
}
png_image* decode_image(const char* filename)
{
    png_image* new_image = (png_image*) malloc(sizeof(png_image));
    FILE* file_to_open = fopen(filename,"rb");
    
    if(!file_to_open)
    {
        puts("ERROR: file not found!");
        exit(-1);
    }
    
    new_image->size = get_file_length(file_to_open);

    printf("size of image is : %llu \n",new_image->size);
    char* raw_image_data =  calloc( 1, new_image->size +1);

    if( !raw_image_data ) fclose(file_to_open),fputs("memory alloc fails",stderr),exit(1);
    
    size_t i = fread(raw_image_data , new_image->size, 1 , file_to_open);
    
    /* copy the file into the buffer */
    if(!raw_image_data)
    {
      fclose(file_to_open);
      free(raw_image_data);
      fputs("entire read fails",stderr);
      exit(1);
    }
    new_image->raw_data = raw_image_data;
    display_raw_image_data(new_image);
    
    

    
    puts("AO");
        chunk_node* ihdr = (chunk_node*) malloc(sizeof(ihdr));
        //ihdr->chunk=  csearch_chunk(file_to_open,PNG_SIGNATURE);
        //new_image->IHDR->node->next
    return NULL;
}
 //void get_image_chunks(png_image* target_png,FILE* )
chunk* search_chunk(char* raw_image_data,char* string_to_find)
{
    return NULL;
}

chunk* read_chunk(png_image* image)
{
  // chunk* chunk;
  // fread(chunk->data_length, sizeof(unsigned int), 1, f);
  // fread(chunk->type, sizeof(char), 4, f);
  // //chunk.data = malloc(chunk.length);
  // fread(chunk->data, sizeof(unsigned char), chunk->data_length, f);
  // fread(&chunk.crc, sizeof(unsigned int), 1, f);
  // //unsigned long actual_crc = crc32(crc32(0, ret->chunk_type, 4), Bytef ,chunk->data_length);
  // if (chunk.crc != actual_crc)
  // {
  //     fprintf(stderr, "Error: chunk checksum failed\n");
  //     exit(1);
  // }
  // return chunk;
}

int main(int argc, char *argv[])
{
    //puts("ASS");
  int i;
  if(argc <= 1)
  {
        printf("Please enter PNG file name(s) to display\n");
        return 0;
  } 

  i=1;
    png_image* image = decode_image(argv[i]);
    // Open the PNG file
   // FILE *f = fopen("spiderman.png", "rb");
   // if (f == NULL)
   // {
   //     fprintf(stderr, "Error: unable to open file\n");
   //     return 1;
   // }
//
   // // Read the PNG signature
   // char signature[8];
   // fread(signature, sizeof(char), 8, f);
   // if (strncmp(signature,PNG_SIGNATURE, 8) != 0)
   // {
   //     fprintf(stderr, "Error: invalid PNG signature\n");
   //     return 1;
   // }
//
    // Read the chunks in the file
   // while (1)
   // {
   //     chunk chunk = read_chunk();
   //     // Process the chunk data as needed
   //     // ...
   //     if (strncmp(chunk.type, "IEND", 4) == 0)
   //     {
   //         // End of image data
   //         break;
   //     }
   //     free(chunk.data);
   // }

    //fclose(f);
    return 0;
}

/*
unsigned char *parse_png(const char *string, int *width, int *height, int *channels)
{
    unsigned char *pixels = NULL;

    //Open .png
    FILE *png;
    png = fopen(string, "rb");
    if (!png)
    {
        printf("Error opening file");
        return NULL;
    }
    size_t file_size = get_file_length(&png);

    //Check if first 8 byte are the PNG signature
    unsigned char *signature_data = (unsigned char *)malloc(8);
    if (!signature_data)
    {
        printf("Could not load memory");
        goto end;
    }
    fread(signature_data, 8, 1, png);
    unsigned char png_signature[] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
    if (memcmp(signature_data, png_signature, 8) != 0)
    {
        printf("Invalid PNG signature");
        goto end;
    }
    free(signature_data);

    //Reading chunks
    chunk_list_t *chunk_list = NULL;
    while (1)
    {
        chunk_t *chunk = read_chunk(&png);
        if (!chunk)
        {
            break;
        }
        list_append((struct list_node **)&chunk_list, (struct list_node *)chunk_item_new(chunk));
        unsigned char iend_signature[] = {0x49, 0x45, 0x4e, 0x44};
        if (memcmp(iend_signature, chunk->chunk_type, 4) == 0)
        {
            break;
        }
    }

    //Parse IHDR chunk
    ihdr_info_t *ihdr_infos = parse_ihdr(((chunk_list_t *)list_pop((struct list_node **)&chunk_list))->chunk);
    if (!ihdr_infos)
    {
        goto end;
    }

    //Look for IDAT chunk
    chunk_list_t *node = (chunk_list_t *)list_pop((struct list_node **)&chunk_list);
    chunk_t *idat = NULL;
    unsigned char idat_signature[] = {0x49, 0x44, 0x41, 0x54};
    while (node)
    {
        chunk_t *chunk = node->chunk;
        if (memcmp(idat_signature, chunk->chunk_type, 4) == 0)
        {
            idat = chunk;
            break;
        }
        node = (chunk_list_t *)list_pop((struct list_node **)&chunk_list);
    }
    if (!idat)
    {
        printf("Could not find pixels data");
        goto end;
    }

    //Parse IDAT chunk
    pixels = parse_idat(idat, ihdr_infos);
    if (!pixels)
    {
        goto end;
    }

    *width = ihdr_infos->width;
    *height = ihdr_infos->height;
    *channels = 4;
    free(chunk_list);
end:
    fclose(png);
    return pixels;
}
*/