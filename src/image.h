
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>

#include <string>
#include <algorithm>


class Image {
	std::string filename;
	SDL_Surface *image{};
	SDL_Texture *texture{};
	SDL_PixelFormat *format = SDL_CreatePixelFormat(SDL_PIXELFORMAT_RGB24);

public:
	Image() = default;

	explicit Image(const char *name) {
		load(name);
	}

	Image(const Image &other) {
		load(other.filename); // Open the file again
	}

	Image &operator=(const Image &other);

	void load(const std::string &name);

	~Image() {
		SDL_DestroySurface(image);
		if (texture != nullptr)
			SDL_DestroyTexture(texture);
	}

	void reset();

	[[nodiscard]] bool is_old() const {
		return image == nullptr && texture == nullptr && filename.empty();
	}

	// Essential function for displaying the image with imgui
	SDL_Texture *get_texture(SDL_Renderer *renderer, float &window_width, float &window_height);

	// Filters
	Image &set_grayscale();

	Image &set_brightness(float factor);

	Image &set_dithering();

	Image &set_auto_tone();

	friend Uint8 truncate_color(float color);
};

Uint8 truncate_color(float color) {
	return std::max((Uint8) 0, std::min((Uint8) 255, (Uint8) color));
}

Image &Image::operator=(const Image &other) {
	if (this == &other)
		return *this;

	load(other.filename);
	return *this;
}

void Image::load(const std::string &name) {
	image = IMG_Load(name.c_str());
	SDL_ConvertSurface(image, format);

	this->filename = name;
	if (image == nullptr) {
		printf("Error: IMG_Load(): %s\n", IMG_GetError());
		return;
	}
}

void Image::reset() {
	filename = "";
	SDL_DestroySurface(image);
	image = nullptr;
	if (texture != nullptr)
		SDL_DestroyTexture(texture);
	texture = nullptr;
}

SDL_Texture *Image::get_texture(SDL_Renderer *renderer, float &window_width, float &window_height) {
	// Aspect ratio should be maintained but should fit in the provided window
	if (is_old()) {
		return nullptr;
	}

	float aspect_ratio = 1;
	if (image->w != image->h) {
		aspect_ratio = (float) image->w / (float) image->h;
	}

	if (image->w > image->h) {
		window_height = window_width / aspect_ratio;
	} else {
		window_width = window_height * aspect_ratio;
	}
	texture = SDL_CreateTextureFromSurface(renderer, image);
	if (texture == nullptr) {
		printf("Error: SDL_CreateTextureFromSurface(): %s\n", SDL_GetError());
		return nullptr;
	}
	return texture;
}

Image &Image::set_grayscale() {
	SDL_LockSurface(image);
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 r, g, b;
		SDL_GetRGB(((Uint32 *) image->pixels)[i], image->format, &r, &g, &b);
		Uint8 gray = (r + g + b) / 3;
		((Uint32 *) image->pixels)[i] = SDL_MapRGB(image->format, gray, gray, gray);
	}
	SDL_UnlockSurface(image);

	return *this;
}

Image &Image::set_brightness(const float factor = 0.5) {
	if (factor < 0 || factor > 2 || factor == 1) {
		return *this;
	}

	SDL_LockSurface(image);
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 r, g, b;
		SDL_GetRGB(((Uint32 *) image->pixels)[i], image->format, &r, &g, &b);

		r = truncate_color(r * factor);
		g = truncate_color(g * factor);
		b = truncate_color(b * factor);

		((Uint32 *) image->pixels)[i] = SDL_MapRGB(image->format, r, g, b);
	}
	SDL_UnlockSurface(image);
	return *this;
}


Image &Image::set_dithering() {
	const int n = 8; // Matrix Size
	const int shades = (n * n);
	const int bayer[] = {
			1, 49, 13, 61, 4, 52, 16, 64,
			33, 17, 45, 29, 36, 20, 48, 32,
			9, 57, 5, 53, 12, 60, 8, 56,
			41, 25, 37, 21, 44, 28, 40, 24,
			3, 51, 15, 63, 2, 50, 14, 62,
			35, 19, 47, 31, 34, 18, 46, 30,
			11, 59, 7, 55, 10, 58, 6, 54,
			43, 27, 39, 23, 42, 26, 38, 22
	};

	SDL_LockSurface(image);
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 r, g, b;

		SDL_GetRGB(((Uint32 *) image->pixels)[i], image->format, &r, &g, &b);

		const int x = i % image->w;
		const int y = i / image->w;

		float un_normalized_gray = (r + g + b) / 3.0f;
		auto gray = (Uint8) (un_normalized_gray * shades / 255.0f);

		int matrix_value = bayer[((y % n) * n) + (x % n)];

		Uint8 output = 0;
		auto bias = -5;
		if (gray + bias > matrix_value) {
			output = 255;
		}

		((Uint32 *) image->pixels)[i] = SDL_MapRGB(image->format, output, output, output);
	}
	SDL_UnlockSurface(image);

	return *this;
}

Image &Image::set_auto_tone() {
	size_t histogramR[256] = {0};
	size_t histogramG[256] = {0};
	size_t histogramB[256] = {0};

	// Compute separate histograms for R, G, and B channels
	SDL_LockSurface(image);
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 r, g, b;
		SDL_GetRGB(((Uint32 *) image->pixels)[i], image->format, &r, &g, &b);
		histogramR[r]++;
		histogramG[g]++;
		histogramB[b]++;
	}
	SDL_UnlockSurface(image);

	// Calculate the cumulative distribution functions (CDF) for each channel
	size_t cdfR[256] = {0};
	size_t cdfG[256] = {0};
	size_t cdfB[256] = {0};
	size_t sumR = 0, sumG = 0, sumB = 0;

	for (int i = 0; i < 256; i++) {
		sumR += histogramR[i];
		sumG += histogramG[i];
		sumB += histogramB[i];
		cdfR[i] = sumR;
		cdfG[i] = sumG;
		cdfB[i] = sumB;
	}

	// Perform histogram equalization for each channel
	SDL_LockSurface(image);
	for (int i = 0; i < image->w * image->h; i++) {
		Uint8 r, g, b;
		SDL_GetRGB(((Uint32 *) image->pixels)[i], image->format, &r, &g, &b);

		r = (Uint8) ((float) cdfR[r] / (image->w * image->h) * 255.0f);
		g = (Uint8) ((float) cdfR[g] / (image->w * image->h) * 255.0f);
		b = (Uint8) ((float) cdfR[b] / (image->w * image->h) * 255.0f);

		((Uint32 *) image->pixels)[i] = SDL_MapRGB(image->format, r, g, b);
	}
	SDL_UnlockSurface(image);

	return *this;
}
