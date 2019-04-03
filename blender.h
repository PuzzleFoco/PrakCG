#pragma once
#include "Include/freeglut.h"
#include "wavefront.h"


#define num_objects 8
extern object3D *blender[num_objects];
enum blenderObjects {
	Krangerust,
	Kranhook,
	Rail,
	Kranhookstab,
	HMitte,
	cargo,
	Spotlight,
	Cube
};

void loadObjects();
void freeObjects();