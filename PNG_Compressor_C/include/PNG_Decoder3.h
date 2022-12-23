
#define SDL_MAIN_HANDLED

#include <stdio.h>
#include <string.h>

#include <math.h>
#include "LinkedList.h"
#include "zlib.h"


#define PNG_SIGNATURE "\x89PNG\r\n\x1a\n"
typedef struct chunk
{
    unsigned int data_length;
    unsigned char* type;
    unsigned char* data;
    unsigned int crc;
} chunk;
typedef struct IHDR_Info
{
   unsigned int width;
    unsigned int height;
    unsigned int bit_depth;
    unsigned int color_type;
    unsigned int compression_method;
    unsigned int filter_method;
    unsigned int interlace_method;
} IHDR_Info;

typedef struct png_image
{
  char* filename;
  size_t size;
  int joe;
  chunk* IHDR;
  unsigned char* pixels;
  IHDR_Info* IHDR_Info;
  list_double_node** node_list;
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
char* get_png_raw_data(FILE* f,char** buffer,size_t* length)
{
    *length = get_file_length(f);
    //printf("%llu AAAAA \n",*length);
    *buffer = (char*)calloc( 1, *length +1);

    if( !buffer ) fclose(f),fputs("memory alloc fails",stderr),exit(1);
    
    size_t i = fread(*buffer , *length, 1 , f);
    //printf("cazzo %s",*buffer);
    /* copy the file into the buffer */
    if(!buffer)
    {
      fclose(f);
      free(*buffer);
      fputs("entire read fails",stderr);
      return NULL;
    }
    //display_raw_image_data(*buffer,*length);
    rewind(f);
    return *buffer;
}
void print_stream(unsigned char* string,FILE* file,size_t len)
{
    printf("PRINTING A STREAM\n");
    for (size_t i = 0; i < 4; i++)
    {
        printf("0x%i at index %lo \n",(int)string[i],ftell(file));
       
    }
    printf("stream ended \n");
}
void print_chunk_info(chunk* c)
{
     printf("Chunk name:");
    for (size_t i = 0; i < 4; i++)
    {
        /* code */
     printf("%c", c->type[i]);
    }
    printf("\n");
    
    printf("Chunk data size: %u\n", c->data_length);
    printf("Chunk CRC: %u\n", c->crc);
   

}
static int paeth_predictor(int a, int b, int c)
{
    int p = a + b - c;
    int pa = abs(p - a);
    int pb = abs(p - b);
    int pc = abs(p - c);
    if (pa <= pb && pa <= pc)
        return a;
    else if (pb <= pc)
        return b;
    else
        return c;
}

static int recon_a(unsigned char *pixels, unsigned int r, unsigned int c, int stride, int bytes_per_pixels)
{
    if (c >= bytes_per_pixels)
        return pixels[r * stride + c - bytes_per_pixels];
    else
        return 0;
}

static int recon_b(unsigned char *pixels, unsigned int r, unsigned int c, int stride, int bytes_per_pixels)
{
    if (r > 0)
        return pixels[(r - 1) * stride + c];
    else
        return 0;
}

static int recon_c(unsigned char *pixels, unsigned int r, unsigned int c, int stride, int bytes_per_pixels)
{
    if (c >= bytes_per_pixels && r > 0)
        return pixels[(r - 1) * stride + c - bytes_per_pixels];
    else
        return 0;
}

unsigned char *parse_idat(chunk *idat, IHDR_Info *ihdr_infos)
{
    int bytes_per_pixels = 4;
   
    printf("%i ",ihdr_infos->width);
    int stride = ihdr_infos->width * bytes_per_pixels;

    //Decompress pixels data
    unsigned long uncompressed_size = ihdr_infos->height * (1 + stride);
    unsigned long uncompressed_max_size = compressBound(idat->data_length);
    
    unsigned char *uncompressed_idat = (unsigned char *)malloc(uncompressed_size);
    if (!uncompressed_idat)
    {
        return NULL;
    }
    int result = uncompress(uncompressed_idat, &uncompressed_size, idat->data, uncompressed_max_size);
    if (result != Z_OK)
    {
        printf("Unable to uncompress IDAT: error %d\n", result);
        free(uncompressed_idat);
        return NULL;
    }

    //Assign pixels memory
    unsigned char *pixels = (unsigned char * )malloc(ihdr_infos->height * stride);
    if (!pixels)
    {
        return NULL;
    }
    int count = 0;

    //De-filtering of uncompressed pixels data
    int i = 0;
    int filter_type = 0;
    int filt_x = 0;
    int recon_x = 0;
    for (unsigned int r = 0; r < ihdr_infos->height; r++)
    {
        filter_type = uncompressed_idat[i++];
        for (unsigned int c = 0; c < stride; c++)
        {
            filt_x = uncompressed_idat[i++];
            switch (filter_type)
            {
                case 0:
                    recon_x = filt_x;
                    break;
                case 1:
                    recon_x = filt_x + recon_a(pixels, r, c, stride, bytes_per_pixels);
                    break;
                case 2:
                    recon_x = filt_x + recon_b(pixels, r, c, stride, bytes_per_pixels);
                    break;
                case 3:
                    recon_x = filt_x + (recon_a(pixels, r, c, stride, bytes_per_pixels) + recon_b(pixels, r, c, stride, bytes_per_pixels)) / 2;
                    break;
                case 4:
                    recon_x = filt_x + paeth_predictor(recon_a(pixels, r, c, stride, bytes_per_pixels), recon_b(pixels, r, c, stride, bytes_per_pixels), recon_c(pixels, r, c, stride, bytes_per_pixels));
                    break;
                default:
                    recon_x = filt_x;
                    break;
            }
            pixels[count++] = recon_x;
        }
    }
    puts("Pixels parsing complete");
    return pixels;
}
void print_image(png_image* img)
{
    puts("##################### PRINTING WHOLE PNG IMAGE LIST ######################");

       //print_image(new_image);
    png_image* image =img;

    if(!image)
    {
        puts("Image is null!");
        return;
    }
    if(!image->node_list)
    {

        puts("LIST IS NULL");
    }
    list_double_node** head = (list_double_node**) &image->node_list;
    list_double_node* node = *head;
    if(!node)
    {
        puts("ERROR: Node doesn't exist!");
        return;
    }
    //printf("\n pointer %p \n",(list_double_node*)image->node_list);
    //printf("\n pointer %p \n",node);
    

    int count =  get_count((list_double_node**)&image->node_list);
    //puts("MADONNA");
    while(node)
    {
       print_chunk_info((chunk*)node->value);
       node = node->next;
    }
     //puts("boh");
    
       puts("##################### IMAGE LIST ENDED ######################");
}
struct chunk* read_chunk(FILE **file)
{
    
    //printf("reading a chunk\n");
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
    ret->data_length = data_to_int(four_byte_data, 4);
   
    //Get chunk_type
   
    ret->type = (unsigned char *)malloc(4);
    if (!ret->type)
    {
        free(ret);
        free(four_byte_data);
        return NULL;
    }

    fread(four_byte_data, 4, 1, *file);
    //printf("\n CHUNK TYPE:\n");
    //print_stream(four_byte_data,*file,4);
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
IHDR_Info* get_IHDR_from_chunk(chunk* chunk)
{
    unsigned char *four_byte_data = (unsigned char *)malloc(4);
    if (!four_byte_data)
    {
        return NULL;
    }
    IHDR_Info* info = (IHDR_Info *)malloc(sizeof(IHDR_Info));
    if (!info)
    {
        return NULL;
    }

    int count = 0;

    //Parsing data
    for (int i = 0; i < 4; i++)
    {
        four_byte_data[i] = chunk->data[count++];
    }
    info->width = data_to_int(four_byte_data, 4);
    for (int i = 0; i < 4; i++)
    {
        four_byte_data[i] = chunk->data[count++];
    }
    info->height = data_to_int(four_byte_data, 4);

    info->bit_depth = chunk->data[count++];

    info->color_type = chunk->data[count++];

    info->compression_method = chunk->data[count++];

    info->filter_method = chunk->data[count++];

    info->interlace_method = chunk->data[count++];

    //Sanity checks
    if (info->compression_method != 0)
    {
        printf("Invalid compression method [%u]", info->compression_method);
        free(info);
        free(four_byte_data);
        return NULL;
    }
    if (info->filter_method != 0)
    {
        printf("Invalid filter method [%u]", info->filter_method);
        free(info);
        free(four_byte_data);
        return NULL;
    }
    if (info->color_type != 6)
    {
        printf("Only true color alpha is supported [%u]", info->color_type);
        free(info);
        free(four_byte_data);
        return NULL;
    }
    if (info->bit_depth != 8)
    {
        printf("Only bit depth of 8 is supported [%u]", info->bit_depth);
        free(info);
        free(four_byte_data);
        return NULL;
    }
    if (info->interlace_method != 0)
    {
        printf("Adam7 interlace not supported [%u]", info->interlace_method);
        free(info);
        free(four_byte_data);
        return NULL;
    }
    if (info->width == 0 || info->height == 0)
    {
        printf("Invalid image size [%u, %u]", info->width, info->height);
        free(info);
        free(four_byte_data);
        return NULL;
    }

    free(four_byte_data);
    return info;
}
png_image* decode_image( char* filename)
{
    png_image* new_image = (png_image*) calloc(1,sizeof(png_image));
    //new_image->raw_pixels ="a0000";
    FILE* file_to_open;
    errno_t err =  fopen_s(&file_to_open,filename,"rb");
    
   // new_image->a = 100;
    if(!file_to_open)
    {
        puts("ERROR: file not found!");
        exit(-1);
    }
    

    size_t length ;
    char* raw_image_data;
    get_png_raw_data(file_to_open,&raw_image_data,&length);
    new_image->size = length;
    printf("size of image is : %llu \n",new_image->size);
    printf("%s",raw_image_data);
    
     new_image->filename=filename;
    //new_image->test = 10;
    //new_image->raw_data = raw_image_data;
    //display_raw_image_data(raw_image_data,new_image->size);
    
    //check the signature
    char sign [8];  
    //char* signature_in_file = calloc(1,8);
    fread(sign,sizeof(char),8,file_to_open);
    printf("%s\n",sign);
    if(strncmp(sign,PNG_SIGNATURE,8)!=0)
    {
       puts("ERROR: PNG signature not present");
       exit(-1);
    }
    
   
       char iend_signature[] = {0x49, 0x45, 0x4e, 0x44};
       char ihdr_signature[] = {0x49, 0x48, 0x44, 0x52};
       char idat_signature[] = {0x49, 0x44, 0x41, 0x54};


    while (1)
    {

       list_double_node* new_node = (list_double_node*) calloc(1,sizeof(list_double_node));
       //list_double_node* n = new
       chunk* chunk_in_question = read_chunk(&file_to_open);
        
       if (!chunk_in_question)
       {
          puts("no chunk");

           break;
       }
       puts("!!!!!!!! PRINTING CHUNK INFO !!!!!!!!!!!!");
       print_chunk_info(chunk_in_question);
       new_node->value = chunk_in_question;
       
       new_node->next = NULL;
       puts("\n   AAAAA\n");
       doublelist_append((list_double_node**) &new_image->node_list,new_node);
       

       if(memcmp(chunk_in_question->type,ihdr_signature,4)==0)
       {

          

          new_image->IHDR = chunk_in_question;
          new_image->IHDR_Info = get_IHDR_from_chunk(chunk_in_question);
       }
       if(memcmp(chunk_in_question->type,idat_signature,4)==0)
       {
         // new_image->pixels = (unsigned char*)calloc(1,chunk_in_question->data_length);
          
          new_image->pixels = parse_idat(chunk_in_question,new_image->IHDR_Info);
         

       }
       
       //list_append((struct list_node **)&chunk_list, (struct list_node *)chunk_item_new(chunk));
       if (memcmp(chunk_in_question->type,iend_signature,4) == 0)
       {
          puts("File has ended");
           break;
       }
      
    }

    
    return new_image;
}



// int main(int argc, char *argv[])
// {
//     //puts("ASS");
//   int i;
//   if(argc <= 1)
//   {
//         printf("Please enter PNG file name(s) to display\n");
//         return 0;
//   } 

//   i=1;
//     png_image* image = decode_image(argv[i]);
//     //puts("\naaaaaaaaaaaaaaaaa");
//     print_image(image);
//     puts("Printing pixels: \n");
//     int l = strlen((char*)image->pixels);
//     for (size_t i = 0; i < 111 ;++i) //NUMBER 111 IS HARDCODED
//     {
//         printf("0x%i \t",image->pixels[i]);
//        if((i +1)% 16 == 0)
//        {
//             printf("|\n");
//        }
//     }
    
//     puts("\n\nImage succesfully parsed");
    
//     return 0;
// }


