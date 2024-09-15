#include "../headers/header.h"

texture_t wallTextures[NUM_TEXTURES];

static const char *textureFileNames[NUM_TEXTURES] = {
	"./images/redbrick.png",
	"./images/purplestone.png",
	"./images/mossystone.png",
	"./images/graystone.png",
	"./images/colorstone.png",
	"./images/bluestone.png",
	"./images/wood.png",
	"./images/eagle.png",
};

/**
 * WallTexturesready - load textures in the respective position
 *
*/
void WallTexturesready(void)
{
	int i;

	for (i = 0; i < NUM_TEXTURES; i++)
	{
		upng_t *upng;

		upng = upng_new_from_file(textureFileNames[i]);

		if (upng != NULL)
		{
			upng_decode(upng);
			if (upng_get_error(upng) == UPNG_EOK)
			{
				wallTextures[i].upngTexture = upng;
				wallTextures[i].width = upng_get_width(upng);
				wallTextures[i].height = upng_get_height(upng);
				wallTextures[i].texture_buffer = (color_t *)upng_get_buffer(upng);
				printf("Successfully loaded texture: %s\n", textureFileNames[i]);
			}
			else
			{	printf("Failed to decode texture: %s, error: %d\n", textureFileNames[i], upng_get_error(upng));			
				upng_free(upng);
				wallTextures[i].upngTexture = NULL;
				wallTextures[i].texture_buffer = NULL;
			}			
		}
		else
		{
			printf("Error: Failed to load texture file %d (%s)\n", i, textureFileNames[i]);
			wallTextures[i].upngTexture = NULL;
			wallTextures[i].texture_buffer = NULL;
		}
	}

}

/**
 * freeWallTextures - free wall textures
 *
*/

void freeWallTextures(void)
{
	int i;

	for (i = 0; i < NUM_TEXTURES; i++)
	{
		if (wallTextures[i].upngTexture != NULL)
		{
			printf("Freeing texture %d (%s)\n", i, textureFileNames[i]);
			upng_free(wallTextures[i].upngTexture);
			wallTextures[i].upngTexture = NULL;
			wallTextures[i].texture_buffer = NULL;
		}
	}
}
