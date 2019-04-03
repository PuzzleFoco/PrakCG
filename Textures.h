#pragma once
#include "Include/freeglut.h"
#include "Include/glext.h"
#include"image.h"

#define NUM_TEXTURES 1
extern cg_image *Texturen[NUM_TEXTURES];
enum textureObjects {
	Metall,
};

void initTextures();
