#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {  // SDL on Windows requires this exact signature (argc/argv)
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(800, 600, 0, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // SDL_RenderPresent(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawPoint(renderer, 400, 300);

    SDL_RenderPresent(renderer);

    SDL_Delay(10000);



    return 0;
}
