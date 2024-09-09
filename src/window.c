#include "../headers/header.h"

static SDL_Renderer *renderer;
static color_t *colorBuffer;
static SDL_Texture *colorBufferTexture;
static SDL_Window *window;

/**
 * initializeWindow - Initialize window to display the maze
 *
 * This function initializes the SDL library, sets up a window,
 * and creates a renderer. It also initializes the color buffer and texture
 * used to render the maze.
 *
 * Return: true in case of success, false if it fails
 */
bool initializeWindow(void)
{
		SDL_DisplayMode display_mode;
		int fullScreenWidth, fullScreenHeight;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
				fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
				return (false);
		}

		SDL_GetCurrentDisplayMode(0, &display_mode);
		fullScreenWidth = display_mode.w;
		fullScreenHeight = display_mode.h;

		window = SDL_CreateWindow(
				NULL,
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				fullScreenWidth,
				fullScreenHeight,
				SDL_WINDOW_BORDERLESS
		);

		if (!window)
		{
				fprintf(stderr, "Error creating SDL window: %s\n", SDL_GetError());
				SDL_Quit();
				return (false);
		}

		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
		if (!renderer)
		{
				fprintf(stderr, "Error creating SDL renderer: %s\n", SDL_GetError());
				SDL_DestroyWindow(window);
				SDL_Quit();
				return (false);
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

		/* Allocate memory for the color buffer */
		colorBuffer = malloc(sizeof(color_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
		if (!colorBuffer)
		{
				fprintf(stderr, "Error allocating memory for color buffer.\n");
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
				return (false);
		}

		/* Create an SDL_Texture to display the color buffer */
		colorBufferTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
																				   SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
		if (!colorBufferTexture)
		{
				fprintf(stderr, "Error creating SDL texture: %s\n", SDL_GetError());
				free(colorBuffer);
				SDL_DestroyRenderer(renderer);
				SDL_DestroyWindow(window);
				SDL_Quit();
				return (false);
		}

		return (true);
}

/**
 * destroyWindow - Clean up and destroy SDL components.
 *
 * This function frees allocated memory, destroys the SDL texture,
 * renderer, and window, and quits SDL. It should be called when the
 * game is over or when exiting the program to ensure resources are
 * properly released.
 */
void destroyWindow(void)
{
		free(colorBuffer);
		SDL_DestroyTexture(colorBufferTexture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
}

/**
 * clearColorBuffer - Clear the color buffer to a specified color
 *
 * @color: The color to clear the buffer with
 *
 * This function fills the entire color buffer with the specified
 * color. It is typically called at the beginning of each frame to
 * reset the drawing surface.
 */
void clearColorBuffer(color_t color)
{
        int i;

        for (i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
                colorBuffer[i] = color;
}

/**
 * renderColorBuffer - Render the contents of the color buffer to the screen
 *
 */
void renderColorBuffer(void)
{
        SDL_UpdateTexture(
                colorBufferTexture,
                NULL,
                colorBuffer,
                (int)(SCREEN_WIDTH * sizeof(color_t))
        );
        SDL_RenderCopy(renderer, colorBufferTexture, NULL, NULL);
        SDL_RenderPresent(renderer);
}

/**
 * drawPixel - Draw a pixel on the color buffer at a specific position
 *
 * @x: The x-coordinate of the pixel
 * @y: The y-coordinate of the pixel
 * @color: The color to assign to the pixel
 */
void drawPixel(int x, int y, color_t color)
{
        colorBuffer[(SCREEN_WIDTH * y) + x] = color;
}
