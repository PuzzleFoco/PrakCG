#pragma once
#include "Include/freeglut.h"
#include "image.h"

#define NUM_TEXTURES 1
extern cg_image textures[NUM_TEXTURES];

enum texturObjekte {
	//Metalldings,
	MaximaleTragkraft
};

void initTextures();
