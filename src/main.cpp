#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <cstdio>
#include <tinyfiledialogs.h>

#include <iostream>

#include "myfont.cpp"

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL3/SDL_opengles2.h>
#else
#include <SDL3/SDL_opengl.h>
#endif

void get_image_texture(const char* filename, SDL_Renderer* renderer, SDL_Texture** texture_ptr, float &window_width, float& window_height);
// Main code
int main(int, char**) {
	// Setup SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMEPAD) != 0) {
		printf("Error: SDL_Init(): %s\n", SDL_GetError());
		return -1;
	}

	// Setup SDL_Image
	IMG_Init(IMG_INIT_TIF);

	// Enable native IME.
	SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");

	// Create window with SDL_Renderer graphics context
	auto window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	SDL_Window* window = SDL_CreateWindow("Image Viewer", 800, 800, window_flags);
	if (window == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_Texture* texture = nullptr;

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Dear ImGui style
	// ImGui::StyleColorsDark();
	ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

	ImFont* font = io.Fonts->AddFontFromMemoryCompressedTTF(MyFont_compressed_data, MyFont_compressed_size, 18.0f);
	IM_ASSERT(font != nullptr);
	// Our state
	static bool show_upload_button = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	float image_width = 700;
	float image_height = 700;
	bool show_image = false;

	// Main loop
	bool done = false;
	while (!done) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT)
				done = true;
			if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(window))
				done = true;
		}

		// Start the Dear ImGui frame
		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(800, 800));
		ImGui::Begin("Image Viewer", &show_upload_button, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

		ImGui::Dummy(ImVec2(0.0f, 10.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 255.0f, 1.0f), "Select a file to continue");
		ImGui::SameLine();


		if (ImGui::Button("Choose file")) {
			auto filepath = tinyfd_openFileDialog("Select a .tiff file to display", "", 1, (const char*[]){"*.tif"}, "TIFF files", 0);
			if (filepath != nullptr) {
				get_image_texture(filepath, renderer, &texture, image_width, image_height);
				show_image = true;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) {
			done = true;
		}

		if (show_image) {
			ImGui::Separator();

			float offX = (800 - image_width) / 2;
			float offY = (ImGui::GetContentRegionAvail().y - image_height) / 2;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + offX);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + offY);

			ImGui::Image((void*)texture, ImVec2(image_width, image_height));
		}

		ImGui::End();

		// Rendering
		ImGui::Render();

		SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();

	IMG_Quit();

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

void get_image_texture(const char* filename, SDL_Renderer* renderer, SDL_Texture** texture_ptr, float& window_width, float& window_height) {

	auto image = IMG_Load(filename);
	if (image == nullptr) {
		printf("Error: IMG_Load(): %s\n", IMG_GetError());
		return;
	}
	// Aspect ratio should be maintained but should fit in the provided window
	float aspect_ratio = 1;
	if (image->w != image->h) {
		aspect_ratio = (float)image->w / (float)image->h;
	}

	if (image->w > image->h) {
		window_height = window_width / aspect_ratio;
	} else {
		window_width = window_height * aspect_ratio;
	}
	*texture_ptr = SDL_CreateTextureFromSurface(renderer, image);
	if (*texture_ptr == nullptr) {
		printf("Error: SDL_CreateTextureFromSurface(): %s\n", SDL_GetError());
		return;
	}
	SDL_DestroySurface(image);
}