#include "Texturen.h"

cg_image textures[NUM_TEXTURES];
const char * texture_files[NUM_TEXTURES] = {
									//"./Textures/metallbackground.bmp",
									"./Textures/maximaleTragkraft.bmp"
};

void initTextures() {

	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		bool success = false;
		
		success = textures[i].load(texture_files[i], true);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		if (!success) printf("Can not load texture %s!\n", texture_files[i]);
	}
}