
#include "lodepng.c"
#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <stdio.h>
#include <math.h>


int show(const char* filename) {

  unsigned error; //this is needed for lode png 
  unsigned char* image; //this pointer points to the image bytes in memory
  unsigned w, h, x, y; //width, height, x and y coordinates for image
  int done;
  
  SDL_Window* sdl_window;
  SDL_Renderer* sdl_renderer;
  SDL_Texture* sdl_texture;
  SDL_Event sdl_event;
  size_t screenw, screenh, pitch;
  size_t jump = 1;  // the jump variable is a number i need to clamp down the image properly
  unsigned int* sdl_pixels;


  printf("showing %s\n", filename);

  // im passing image, w and h as reference so that the lodepng function will populate them
  error = lodepng_decode32_file(&image,&w,&h,filename); 
  // if error happens to be any value above 0 then it's an error
  if(error) {
    printf("decoder error %u: %s\n", error, lodepng_error_text(error));
    return 0;
  }

  // clamp the image if it is above 1024x or 1024y
  if(w / 1024 >= jump) jump = w / 1024 + 1; 
  if(h / 1024 >= jump) jump = h / 1024 + 1;




  screenw = w / jump;
  screenh = h / jump;
  
  printf("Image width: %u  and height: %u\n",w,h);
  printf("Window width: %llu  and height: %llu\n",screenw,screenh);




  sdl_pixels = (unsigned int*)malloc(screenw * screenh*sizeof(unsigned int));

  if(!sdl_pixels) {
    printf("Failed to allocate pixels\n");
    return 0;
  }

  for(y = 0; y + jump - 1 < h; y +=jump)
  for(x = 0; x + jump - 1 < w; x +=jump) {
    // get RGBA components 
    unsigned int r = image[4*y * w + 4 * x + 0]; // red 
    unsigned int g = image[4*y * w + 4 * x + 1]; // green
    unsigned int b = image[4*y * w + 4 * x + 2]; // blue 
    unsigned int a = image[4*y * w + 4 * x + 3]; // alpha

    // make translucency visible by placing checkerboard pattern behind image 
    //int checker_color = 191 + 64 * (((x / 16) % 2) == ((y / 16) % 2));
    int checker_color = 0;
    r = (a * r + (255 - a) * checker_color) / 255;
    g = (a * g + (255 - a) * checker_color) / 255;
    b = (a * b + (255 - a) * checker_color) / 255;

    // give the color value to the pixel of the screenbuffer 
    unsigned int pixel_color = (256*256)* r + 256 * g + b; 
    // RGBA is 4 bytes so i need to place these bytes in the correct bit,
    // so i multiply the values by 2^16 and 2^8 respectively 
    // (i am pushing these values to the left by 16 bits and then 8 bits respectively)

    sdl_pixels[(y * screenw + x)/jump] = pixel_color;
    
  }

  //SDL INIT

  //create window
  SDL_CreateWindowAndRenderer(screenw, screenh, SDL_WINDOW_OPENGL, &sdl_window, &sdl_renderer);
  if(!sdl_window) //if for some reason the window doesnt exist, print error;
  {
    printf("Error, no SDL screen\n");
    exit(-1);
  }
 
  SDL_SetWindowTitle(sdl_window, filename);
  
  //
  sdl_texture = SDL_CreateTexture(sdl_renderer, SDL_PIXELFORMAT_ARGB8888,
                                  SDL_TEXTUREACCESS_STATIC, screenw, screenh);

  pitch = screenw * sizeof(Uint32); // the pitch is the length of the scanline
  SDL_UpdateTexture(sdl_texture, NULL, sdl_pixels, pitch);
  
  SDL_RenderClear(sdl_renderer);
  SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
  SDL_RenderPresent(sdl_renderer);

  // pause until you press escape and meanwhile redraw screen
  done = 0;
  while(done == 0) {
    while(SDL_PollEvent(&sdl_event)) {
      if(sdl_event.type == SDL_QUIT) done = 2;
      else if(SDL_GetKeyboardState(NULL)[SDLK_ESCAPE]) done = 2;
      else if(sdl_event.type == SDL_KEYDOWN) done = 1; // press any other key for next image 
    }
    SDL_Delay(5); // pause 5 ms so it consumes less processing power 
  }

  SDL_Quit();
  free(sdl_pixels);
  return done == 2 ? 1 : 0;
}

int main(int argc, char* argv[]) {
  //puts("ASS");
  int i;
  if(argc <= 1) printf("Please enter PNG file name(s) to display\n");;

  for(i = 1; i < argc; i++) {
    if(show(argv[i])) return 0;
  }
 
  return 0;
}
