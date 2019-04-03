#include<iostream>
#include <stdio.h>
#include "Textures.h"
#include "image.h"

cg_image *Texturen[NUM_TEXTURES];

const char * texture_files[NUM_TEXTURES] = {
	"./Textures/metallbackground.bmp"
};

void initTextures()
{
	for (int i = 0; i < NUM_TEXTURES; i++)
	{
		bool success = false;
		success = Texturen[i]->load(texture_files[i], false);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		if (!success) printf("Can not load texture %s!\n", texture_files[i]);
	}
}