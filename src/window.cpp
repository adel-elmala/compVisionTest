#include "window.h"

#include <SDL2/SDL_image.h>

window::window() :m_win_title{ "ComVisonTest" },m_surface{NULL},m_texture{NULL},io{NULL}
{
}

window::~window()
{
	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	//ImGui::DestroyContext();
	if(m_texture)
		SDL_DestroyTexture(m_texture);
	SDL_DestroyRenderer(m_renderer);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

int window::init()
{
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

	// Create window with SDL_Renderer graphics context
	SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	m_window = SDL_CreateWindow("CompVisionTest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);
	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (m_renderer == nullptr)
	{
		SDL_Log("Error creating SDL_Renderer!");
		return 0;
	}
	//SDL_RendererInfo info;
	//SDL_GetRendererInfo(renderer, &info);
	//SDL_Log("Current SDL_Renderer: %s", info.name);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	io = &(ImGui::GetIO()); (void)io;
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io->ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForSDLRenderer(m_window, m_renderer);
	ImGui_ImplSDLRenderer2_Init(m_renderer);

	// read the image 
	loadImg("C:/Users/a.refaat/projects/comvisiontest/lena.png");

}

void window::render()
{
	// Our state
	bool show_demo_window = true;
	bool show_another_window = true;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	// Main loop
	bool done = false;
	while (!done)
	{
		// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			ImGui_ImplSDL2_ProcessEvent(&event);
			if (event.type == SDL_QUIT)
				done = true;
			if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(m_window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer2_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();
		//ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());


		//// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
		//if (show_demo_window)
		//	ImGui::ShowDemoWindow(&show_demo_window);

		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			static int counter = 0;

			ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

			ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

			if (ImGui::Button("modify image"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				modifyImg();
				counter++;
			}
			ImGui::SameLine();
			ImGui::Text("counter = %d", counter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io->Framerate, io->Framerate);
			ImGui::End();
		}
		// Rendering
		ImGui::Render();
		SDL_RenderSetScale(m_renderer, io->DisplayFramebufferScale.x, io->DisplayFramebufferScale.y);
		SDL_SetRenderDrawColor(m_renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(m_renderer);
		drawImg();
		ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(m_renderer);
	}
}



void window::loadImg(char* img_path)
{
	m_surface = IMG_Load(img_path);
}
/* This is meant to show how to edit a surface's pixels on the CPU, but
   normally you should use SDL_FillRect() to wipe a surface's contents. */
void window::drawImg()
{
	int w, h; // texture width & height

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	//m_img = IMG_LoadTexture(m_renderer, img_path);
	SDL_QueryTexture(m_texture, NULL, NULL, &w, &h); // get the width and height of the texture

	// put the location where we want the texture to be drawn into a rectangle
	// I'm also scaling the texture 2x simply by setting the width and height
	SDL_Rect texr = {0, 0, w ,  h };
	SDL_RenderCopy(m_renderer, m_texture, NULL, &texr);

}
Uint32 getpixel(SDL_Surface* surface, int x, int y)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		return *p;
		break;

	case 2:
		return *(Uint16*)p;
		break;

	case 3:
		if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
			return p[0] << 16 | p[1] << 8 | p[2];
		else
			return p[0] | p[1] << 8 | p[2] << 16;
		break;

	case 4:
		return *(Uint32*)p;
		break;

	default:
		return 0;       /* shouldn't happen, but avoids warnings */
	}
}
void setpixel(SDL_Surface* surface, int x, int y,Uint32 value)
{
	int bpp = surface->format->BytesPerPixel;
	/* Here p is the address to the pixel we want to retrieve */
	Uint8* p = (Uint8*)surface->pixels + y * surface->pitch + x * bpp;

	switch (bpp)
	{
	case 1:
		*p = value;
		break;

	case 2:
		*(Uint16*)p = (Uint16) value;
		break;

	case 3:
		/*if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			*(Uint16*)p = (value & 0x00ff0000) >> 8 | value & 0x0000ff00;
			*((Uint8*)p + 2) = value & 0x00 ;

		}*/
		/*else*/
		*(Uint8*)p = value & 0x000000ff;
		*((Uint8*)p + 1) = (value & 0x0000ff00)>>8;
		*((Uint8*)p + 2) = (value & 0x00ff0000) >> 16;
		break;

	case 4:
		*(Uint32*)p = (Uint32) value;
		break;

	default:
		break;       /* shouldn't happen, but avoids warnings */
	}
}

void window::modifyImg()
{
	SDL_LockSurface(m_surface);
	SDL_Color rgb;
	for (int x = 0;x <= m_surface->w;++x)
	{
		for (int y = 0; y <= m_surface->h; y++)
		{
			Uint32 pix = getpixel(m_surface, x, y);
			SDL_GetRGB(pix, m_surface->format, &rgb.r, &rgb.g, &rgb.b);
			rgb = { 0,rgb.g,0,255 };
			pix  = SDL_MapRGB(m_surface->format, rgb.r, rgb.g, rgb.b);
			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);

}