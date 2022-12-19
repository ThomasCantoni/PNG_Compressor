//#include "lodepng.c"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "LinkedListElements.h"
#include "zlib.h"


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
  //char* raw_data;
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
void display_raw_image_data(char* image,size_t image_size)
{
    for (size_t i = 0; i < image_size;++i)
    {
        printf("0x%i \t",image[i]);
       if((i +1)% 16 == 0)
       {
            printf("|\n");
       }
    }
}
static unsigned int data_to_int(unsigned char *data, size_t data_length)
{
    if (data_length > 4) return 0;

    unsigned int value = 0;
    unsigned int multiplier = 1;
    for (int i = data_length - 1; i >= 0; i--)
    {
        unsigned char nibble = data[i];
        value += nibble * multiplier;
        multiplier *= 256;
    }
    return value;
}
char* get_png_raw_data(FILE* f,char* buffer,size_t* length)
{
    length = get_file_length(f);
    buffer = calloc( 1, length +1);

    if( !buffer ) fclose(f),fputs("memory alloc fails",stderr),exit(1);
    
    size_t i = fread(buffer , length, 1 , f);
    
    /* copy the file into the buffer */
    if(!buffer)
    {
      fclose(f);
      free(buffer);
      fputs("entire read fails",stderr);
      return NULL;
    }
    return buffer;
}
void print_stream(char* string,FILE* file,size_t len)
{
    printf("\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("0x%i at index %i \n",(int)string[i],ftell(file));
       
    }
    printf("stream ended \n");
}
struct chunk* read_chunk(FILE **file)
{
    
    printf("\n");
    //Allocate chunk struct memory
    chunk *ret = (chunk*)malloc(sizeof(chunk));
    if (!ret) return NULL;

    //Get data_length
    unsigned char *four_byte_data = (unsigned char *)malloc(4);
    if (!four_byte_data)
    {
        free(ret);
        return NULL;
    }
    fread(four_byte_data, 4, 1, *file);
    print_stream(four_byte_data,*file,4);
    
    ret->data_length = data_to_int(four_byte_data, 4);
    //printf("\n%c is %u\n",four_byte_data, ret->data_length);
    //Get chunk_type
    ret->type = (unsigned char *)malloc(4);
    if (!ret->type)
    {
        free(ret);
        free(four_byte_data);
        return NULL;
    }

    fread(four_byte_data, 4, 1, *file);
    printf("\n CHUNK TYPE");
    print_stream(four_byte_data,*file,4);
    memcpy(ret->type, four_byte_data, 4);

    //Get chunk_data
    unsigned char *chunk_data_from_file = (unsigned char *)malloc(ret->data_length);
    if (!chunk_data_from_file)
    {
        free(ret);
        free(four_byte_data);
        return NULL;
    }
    ret->data = (unsigned char *)malloc(ret->data_length);
    if (!ret->data)
    {
        free(ret);
        free(four_byte_data);
        free(chunk_data_from_file);
        return NULL;
    }
    fread(chunk_data_from_file, ret->data_length, 1, *file);
    printf("%u \n",ret->data_length);
    //print_stream(chunk_data_from_file,*file,4);
    memcpy(ret->data, chunk_data_from_file, ret->data_length);

    //Get and check CRC
    fread(four_byte_data, 4, 1, *file);

    unsigned int expected_crc = data_to_int(four_byte_data, 4);
    unsigned int exported_crc = crc32(crc32(0, ret->type, 4), ret->data, ret->data_length);
    if (expected_crc != exported_crc)
    {
        printf("Chunk checksum failed {%u} != {%u}\n", expected_crc, exported_crc);
        free(ret);
        free(four_byte_data);
        free(chunk_data_from_file);
        return NULL;
    }
    ret->crc = expected_crc;

    free(four_byte_data);
    free(chunk_data_from_file);
    return ret;
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
    

    size_t length ;
    char* raw_image_data =0;
    get_png_raw_data(file_to_open,raw_image_data,&length);
    new_image->size = length;
    printf("size of image is : %llu \n",new_image->size);
    //new_image->raw_data = raw_image_data;
    display_raw_image_data(raw_image_data,new_image->size);
    rewind(file_to_open);

    
    // while (1)
    // {
    //     chunk* chunk_in_question = read_chunk(file_to_open);
    //     if (!chunk_in_question)
    //     {
    //         break;
    //     }
    //     list_append((struct list_node **)&chunk_list, (struct list_node *)chunk_item_new(chunk));
    //     unsigned char iend_signature[] = {0x49, 0x45, 0x4e, 0x44};
    //     if (memcmp(iend_signature, chunk->chunk_type, 4) == 0)
    //     {
    //         break;
    //     }
    // }

    //new_image->IHDR->chunk =(chunk*) read_chunk(&file_to_open);
    printf("%s",new_image->IHDR->chunk->data);
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