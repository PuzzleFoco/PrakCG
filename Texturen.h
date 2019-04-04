#pragma once
#include "Include/freeglut.h"
#include "image.h"

#define NUM_TEXTURES 2
extern cg_image textures[NUM_TEXTURES];

enum texturObjekte {
	//Metalldings,
	MaximaleTragkraft,
	Beton
};

void initTextures();
