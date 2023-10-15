#pragma once

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

class window
{
private:
    /* data */
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;
    ImGuiContext * imgui_context;
    SDL_Texture* m_texture;
    SDL_Surface* m_surface;
    SDL_Surface* m_win_surface;
    ImGuiIO* io;
    char* m_win_title;
    char* img_path;
    ImVec4 clear_color;

public:
    window();
    ~window();
    int init();
    void render();
    void update();
    void loadImg(char* img_path);
    void drawImg();
    void modifyImg();
    void grey();
    void setupGui();
    void original();
    void updateTexture();
    void greyScale();




};
