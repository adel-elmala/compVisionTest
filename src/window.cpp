#include "window.h"

#include <SDL2/SDL_image.h>
#include <algorithm>    // std::min
#include <vector>
#include <set>

window::window() :m_win_title{ "ComVisonTest" }, m_surface{ NULL }, m_texture{ NULL }, io{ NULL }, img_path{ "C:/Users/a.refaat/projects/comvisiontest/bin2.png" }
{
}

window::~window()
{
	// Cleanup
	ImGui_ImplSDLRenderer2_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	if (m_texture)
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
	loadImg(img_path);

}

void window::render()
{
	// Our state
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Main loop
	bool done = false;
	while (!done)
	{
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


		// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
		{
			static float f = 0.0f;
			ImGui::Begin("Controls!");                          // Create a window called "Hello, world!" and append into it.

			if (ImGui::Button("reset"))
			{
				loadImg(img_path);

			}
			if (ImGui::Button("mod image"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			{
				modifyImg();

			}
			if (ImGui::Button("grey scale"))
			{
				greyScale();

			}
			if (ImGui::Button("binary"))
			{
				binary();

			}
			if (ImGui::Button("gauss"))
			{
				float gaussian_kernal_3x3[9] = { 1.0 / 16 , 2.0 / 16,1.0 / 16,
												2.0 / 16 , 4.0 / 16,2.0 / 16,
												1.0 / 16 , 2.0 / 16,1.0 / 16 };
				applyFilter(gaussian_kernal_3x3, 3);

			}
			if (ImGui::Button("sharpen"))
			{
				float sharpen_kernal_3x3[9] = { 0.0 , -1.0 ,0.0,
												-1.0 , 4.0 ,-1.0,
												0.0 , -1.0 ,0.0 };
				applyFilter(sharpen_kernal_3x3, 3);

			}
			if (ImGui::Button("segment"))
			{

				segment();

			}

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

void window::drawImg()
{
	int w, h; // texture width & height

	m_texture = SDL_CreateTextureFromSurface(m_renderer, m_surface);
	//m_img = IMG_LoadTexture(m_renderer, img_path);
	SDL_QueryTexture(m_texture, NULL, NULL, &w, &h); // get the width and height of the texture

	// put the location where we want the texture to be drawn into a rectangle
	// I'm also scaling the texture 2x simply by setting the width and height
	SDL_Rect texr = { 0, 0, w ,  h };
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
void setpixel(SDL_Surface* surface, int x, int y, Uint32 value)
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
		*(Uint16*)p = (Uint16)value;
		break;

	case 3:
		/*if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
		{
			*(Uint16*)p = (value & 0x00ff0000) >> 8 | value & 0x0000ff00;
			*((Uint8*)p + 2) = value & 0x00 ;

		}*/
		/*else*/
		*(Uint8*)p = value & 0x000000ff;
		*((Uint8*)p + 1) = (value & 0x0000ff00) >> 8;
		*((Uint8*)p + 2) = (value & 0x00ff0000) >> 16;
		break;

	case 4:
		*(Uint32*)p = (Uint32)value;
		break;

	default:
		break;       /* shouldn't happen, but avoids warnings */
	}
}

void window::modifyImg()
{
	SDL_LockSurface(m_surface);
	SDL_Color rgb;
	for (int y = 0; y < m_surface->h; ++y)
	{
		for (int x = 0; x < m_surface->w; x++)
		{
			Uint32 pix = getpixel(m_surface, x, y);
			SDL_GetRGB(pix, m_surface->format, &rgb.r, &rgb.g, &rgb.b);
			rgb = { 0,rgb.g,0,255 };
			pix = SDL_MapRGB(m_surface->format, rgb.r, rgb.g, rgb.b);
			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);
}

void window::greyScale()
{
	SDL_LockSurface(m_surface);
	SDL_Color rgb;
	for (int y = 0; y < m_surface->h; ++y)
	{
		for (int x = 0; x < m_surface->w; x++)
		{
			Uint32 pix = getpixel(m_surface, x, y);
			SDL_GetRGB(pix, m_surface->format, &rgb.r, &rgb.g, &rgb.b);
			unsigned char grey_value = rgb.r * 0.3 + rgb.g * 0.6 + rgb.b * 0.1;
			rgb = { grey_value,grey_value,grey_value,255 };
			pix = SDL_MapRGB(m_surface->format, rgb.r, rgb.g, rgb.b);
			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);
}

void window::binary()
{
	SDL_LockSurface(m_surface);
	SDL_Color rgb;
	unsigned char threshold = 120;
	for (int y = 0; y < m_surface->h; ++y)
	{
		for (int x = 0; x < m_surface->w; x++)
		{
			Uint32 pix = getpixel(m_surface, x, y);
			SDL_GetRGB(pix, m_surface->format, &rgb.r, &rgb.g, &rgb.b);
			unsigned char grey_value = rgb.r;
			unsigned char binary_value = grey_value >= threshold ? 255 : 0;
			rgb = { binary_value,binary_value,binary_value,255 };
			pix = SDL_MapRGB(m_surface->format, rgb.r, rgb.g, rgb.b);
			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);
}


void window::applyFilter(float* filter, int filter_size)
{
	SDL_LockSurface(m_surface);
	SDL_Color rgb;
	int shifted_index = filter_size / 2;
	for (int y = shifted_index; y < m_surface->h - shifted_index; ++y)
	{
		for (int x = shifted_index; x < m_surface->w - shifted_index; x++)
		{
			SDL_Color accum_rgb = { 0 };
			// for each pixel in the filter
			for (int i = -shifted_index, k = 0; i <= shifted_index; i++, ++k)
			{
				// for each pixel in the filter
				for (int j = -shifted_index, l = 0; j <= shifted_index; j++, l++)
				{
					// extract the pixel rgb
					Uint32 pix = getpixel(m_surface, x + i, y + j);
					SDL_GetRGB(pix, m_surface->format, &rgb.r, &rgb.g, &rgb.b);

					float filter_value = *(filter + (filter_size * k) + l);
					// process the pixel
					rgb = { (unsigned char)(rgb.r * filter_value),
						(unsigned char)(rgb.g * filter_value),
						(unsigned char)(rgb.b * filter_value) };

					accum_rgb = { (unsigned char)std::min(255, (rgb.r + accum_rgb.r)),
									(unsigned char)std::min(255, (rgb.g + accum_rgb.g)),
									(unsigned char)std::min(255, (rgb.b + accum_rgb.b)),
									255 };

				}
			}
			// save the new pixel
			Uint32 pix = SDL_MapRGB(m_surface->format, accum_rgb.r, accum_rgb.g, accum_rgb.b);
			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);
}


// TODO
// assuming pixel values either 255 or 0 (binary but with 3 chanels)
void window::segment()
{
	SDL_Color pix_rgb;
	SDL_Color left_rgb;
	SDL_Color up_rgb;
	SDL_Color upper_left_rgb;

	int width = m_surface->w;
	int height = m_surface->h;

	unsigned int* label_buffer = new unsigned int[width * height * sizeof(unsigned int)];
	std::vector<std::pair<unsigned int, unsigned int >> equivalance_table;

	memset(label_buffer, 0, height * width * sizeof(unsigned int));
	unsigned int current_label = 0;

	SDL_LockSurface(m_surface);
	// FIXME : fix the boundries 
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned int* pix_label = label_buffer + (y * width) + x;
			if (0 == *pix_label) // unlabeled
			{
				Uint32 pix = getpixel(m_surface, x, y);
				SDL_GetRGB(pix, m_surface->format, &pix_rgb.r, &pix_rgb.g, &pix_rgb.b);
				//	X X
				//	X 0
				if (colors_match(pix_rgb, { 0,0,0,255 })) // not an object pixel
					continue;

				// if first row 
				if (0 == y)
				{
					if (0 == x) // first ever pixel
						*pix_label = ++current_label;
					// look left only 
					Uint32 left = getpixel(m_surface, x - 1, y); // left , same row
					SDL_GetRGB(left, m_surface->format, &left_rgb.r, &left_rgb.g, &left_rgb.b);
					if (colors_match(left_rgb, pix_rgb))
					{
						*pix_label = *(label_buffer + (y * width) + (x - 1)); // assign it the same label of the left pixel
					}
					else
					{
						*pix_label = ++current_label;
					}
					continue;
				}
				// if first colomn
				if (0 == x)
				{
					// look up only
					Uint32 up = getpixel(m_surface, x, y - 1); // up 
					SDL_GetRGB(up, m_surface->format, &up_rgb.r, &up_rgb.g, &up_rgb.b);
					if (colors_match(up_rgb, pix_rgb))
					{
						*pix_label = *(label_buffer + ((y - 1) * width) + (x)); // assign it the same label of the upper pixel
					}
					else
					{
						*pix_label = ++current_label;
					}
				}
				// get the 3-neighboring pixels
				Uint32 upper_left = getpixel(m_surface, x - 1, y - 1); // left , upper row
				Uint32 left = getpixel(m_surface, x - 1, y); // left , same row
				Uint32 up = getpixel(m_surface, x, y - 1); // up 
				SDL_GetRGB(upper_left, m_surface->format, &upper_left_rgb.r, &upper_left_rgb.g, &upper_left_rgb.b);
				SDL_GetRGB(up, m_surface->format, &up_rgb.r, &up_rgb.g, &up_rgb.b);
				SDL_GetRGB(left, m_surface->format, &left_rgb.r, &left_rgb.g, &left_rgb.b);

				//  3-neighboring pixels are background pixels
				//	0 0
				//	0 1
				if (colors_match(upper_left_rgb, { 0,0,0,255 }) && colors_match(left_rgb, { 0,0,0,255 }) && colors_match(up_rgb, { 0,0,0,255 }))
				{
					// assign a new label
					*pix_label = ++current_label;
					continue;
				}
				//	D X
				//	X 1
				if (colors_match(upper_left_rgb, pix_rgb))
				{
					*pix_label = *(label_buffer + ((y - 1) * width) + (x - 1)); // assign it the same label of the \diagonal pixel
					continue;
				}
				//	0 0
				//	C 1 
				if (colors_match(left_rgb, pix_rgb) && colors_match(upper_left_rgb, { 0,0,0,255 }) && colors_match(up_rgb, { 0,0,0,255 }))
				{
					*pix_label = *(label_buffer + (y * width) + (x - 1)); // assign it the same label of the left pixel
					continue;
				}
				//	0 B
				//	0 1 
				if (colors_match(up_rgb, pix_rgb) && colors_match(upper_left_rgb, { 0,0,0,255 }) && colors_match(left_rgb, { 0,0,0,255 }))
				{
					*pix_label = *(label_buffer + ((y - 1) * width) + (x)); // assign it the same label of the upper pixel
					continue;
				}
				//	0 B
				//	C 1 
				if (colors_match(up_rgb, pix_rgb) && colors_match(left_rgb, pix_rgb))
				{
					*pix_label = *(label_buffer + ((y - 1) * width) + (x)); // assign it the same label of the upper pixel
					// add the equvalent label pair into the table
					equivalance_table.push_back({ *(label_buffer + ((y - 1) * width) + (x)) ,
													*(label_buffer + ((y)*width) + (x - 1)) });
					continue;
				}

			}
		}
	}
	
	//// resolve the equivalance table 

	for (auto [x,y] : equivalance_table)
	{
		if (x == y)
			continue;
		for (int r = 0; r < height; ++r)
		{
			for (int c = 0; c < width; c++)
			{
				unsigned int * target = label_buffer + (r * width) + c;
				if (*target == x)
					*target = y;
			}
		}

	}
	// count the unique labels -- count the number of objects in the image
	std::set<unsigned int> unique_labels;
	for (int r = 0; r < height; ++r)
	{
		for (int c = 0; c < width; c++)
		{
			unsigned int* target = label_buffer + (r * width) + c;
			if (*target)
				unique_labels.insert(*target);
		}
	}

	// build the color table
	std::vector<SDL_Color> colorTable;
	//float colorStep = 255.0 / current_label;
	float colorStep = 15;
	for (int i = 1; i <= current_label; ++i)
	{
		SDL_Color color = { 0 };

		for (int r = 0; r <= 255; r += colorStep)
		{
			color.r = r ;
			for (int g = 0; g <= 255; g += colorStep)
			{
				color.g = g;

				for (int b = 0; b <= 255; b += colorStep)
				{
					color.b = b;
					colorTable.push_back(color);

				}
			}
		}
	}
	// recolor the image based on assigned labels
	for (int y = 1; y < height; ++y)
	{
		for (int x = 0; x < width; x++)
		{
			unsigned int label = *(label_buffer + (y * width) + x);
			if (0 == label)
				continue;
			Uint32 pix = SDL_MapRGB(m_surface->format, colorTable[label -1].r, colorTable[label -1 ].g, colorTable[label -1].b);

			setpixel(m_surface, x, y, pix);
		}
	}
	SDL_UnlockSurface(m_surface);
	delete[] label_buffer;
}
bool window::colors_match(SDL_Color c1, SDL_Color c2)
{
	return (c1.r == c2.r) && (c1.g == c2.g) && (c1.b == c2.b);
}