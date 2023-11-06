#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <tinyfiledialogs.h>

#include "myfont.cpp"
#include "style.h"
#include "image.h"

#define WIDTH 1200
#define HEIGHT 800

const int NUM_PAGES = 4;
const int NUM_SIDES = 2;
enum {
	PAGE1,
	PAGE2,
	PAGE3,
	PAGE4,
	// Don't reorder enums
	WelcomePage
} Page;

enum {
	LEFT, RIGHT
} Side;


// Main code
int main(int, char **) {


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
	auto window_flags = (SDL_WindowFlags) (SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	SDL_Window *window = SDL_CreateWindow("Image Viewer", WIDTH, HEIGHT, window_flags);
	if (window == nullptr) {
		printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
		return -1;
	}
	SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	if (renderer == nullptr) {
		SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
		return -1;
	}

	SDL_SetWindowPosition(window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	SDL_ShowWindow(window);

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	// Setup Platform/Renderer backends for SDL
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);
	//
	ImFont *font = io.Fonts->AddFontFromMemoryCompressedTTF(MyFont_compressed_data, MyFont_compressed_size, 18.0f);
	ImFont *fontLg = io.Fonts->AddFontFromMemoryCompressedTTF(MyFont_compressed_data, MyFont_compressed_size, 26.0f);
	IM_ASSERT(font != nullptr);
	// Our state
	static bool show_upload_button = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGui::StyleColorsLight();

	// Set the upper limit of the image size
	// These will be auto-scaled to fit the window with aspect ratio maintained
	// 10 is for padding
	float padding = 80;
	auto image_width = (WIDTH - padding) / 2.0f;
	auto image_height = (HEIGHT - padding) / 2.0f;

	bool image_update = false;
	Image image[NUM_PAGES][NUM_SIDES];
	const char *labels[NUM_PAGES][NUM_SIDES] = {
			{"Original Image",  "Grayscale Image"},
			{"Brightness 50%",  "Brightness 50% + Grayscale"},
			{"Grayscale Image", "Dithering"},
			{"Original Image",  "Color Corrected"},
	};

	// Main loop
	bool done = false;
	int page = WelcomePage;
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
		ImGui::SetNextWindowSize(ImVec2(WIDTH, HEIGHT));
		ImGui::Begin("Image Viewer", &show_upload_button,
					 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
					 ImGuiWindowFlags_NoResize);

		ImGui::Dummy(ImVec2(padding / 2, 10.0f));
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 255.0f, 1.0f), "Select a file to continue");
		ImGui::SameLine();

		if (ImGui::Button("Choose file")) {
			auto filepath = tinyfd_openFileDialog(
					"Select a .tiff file to display",
					"",
					1,
					(const char *[]) {"*.tif"},
					"TIFF files",
					0
			);
			if (filepath == nullptr) {
				continue;
			}

			for (int i = PAGE1; i <= PAGE4; i++) {
				image[i][LEFT].load(filepath);
				image[i][RIGHT].load(filepath);
			}

			page = PAGE1;

			// Tracks consequent image uploads
			image_update = true;
		}
		ImGui::SameLine();
		if (ImGui::Button("Close")) {
			break;
		}

		if (image_update) { // These updates are cached
			// PAGE1
			image[PAGE1][RIGHT].set_grayscale();

			// PAGE2
			image[PAGE2][LEFT].set_brightness(0.5);
			image[PAGE2][RIGHT].set_brightness(0.5).set_grayscale();

			// PAGE3
			image[PAGE3][LEFT].set_grayscale();
			image[PAGE3][RIGHT].set_dithering();

			// PAGE4
			image[PAGE4][RIGHT].set_auto_tone();
			image_update = false;
		}

		if (page == WelcomePage) {
			ImGui::PushFont(fontLg);
			const char *text = "Welcome to the Image Viewer";

			auto textSize = ImGui::CalcTextSize(text);
			AlignForWidth(textSize.x);
			AlignForHeight(textSize.y, 0.25f);

			ImGui::Text("%s", text);
			ImGui::PopFont();

		} else {
			// Rendering the images
			void *t1 = image[page][LEFT].get_texture(renderer, image_width, image_height);
			void *t2 = image[page][RIGHT].get_texture(renderer, image_width, image_height);
			show_images(t1, t2, labels[page][LEFT], labels[page][RIGHT], image_width, image_height);

			// Rendering the buttons and page number
			ImGuiStyle &style = ImGui::GetStyle();

			auto textSize = ImGui::CalcTextSize("Page: 4/4");
			AlignForWidth(textSize.x);
			AlignForHeight(textSize.y);
			ImGui::Text("Page: %d/%d", page + 1, 4);

			textSize = ImGui::CalcTextSize("Previous + Next");
			AlignForWidth(textSize.x);
			AlignForHeight(textSize.y, 0);
			if (ImGui::Button("Previous")) {
				page = (page + 3) % 4;
			}
			ImGui::SameLine();
			if (ImGui::Button("Next")) {
				page = (page + 1) % 4;
			}
		}

		ImGui::End();

		// Rendering
		ImGui::Render();

		SDL_SetRenderDrawColor(renderer, (Uint8) (clear_color.x * 255), (Uint8) (clear_color.y * 255),
							   (Uint8) (clear_color.z * 255), (Uint8) (clear_color.w * 255));
		SDL_RenderClear(renderer);
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData());
		SDL_RenderPresent(renderer);
	}

	// Cleanup
	ImGui_ImplSDLRenderer3_Shutdown();
	ImGui_ImplSDL3_Shutdown();
	ImGui::DestroyContext();


	// Destroy the textures before the renderer
	for (int i = PAGE1; i <= PAGE4; i++) {
		for (int j = LEFT; j <= RIGHT; j++) {
			image[i][j].reset();
		}
	}

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();

	return 0;
}

