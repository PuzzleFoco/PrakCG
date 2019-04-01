#pragma once
#include "Include/freeglut.h"
#include "Include/glext.h"
#include "wavefront.h"


#define num_objects 7
extern object3D *blender[num_objects];
enum blenderObjects {
	Krangerust,
	Kranhook,
	Rail,
	Kranhookstab,
	HMitte,
	cargo,
	Spotlight
};

void loadObjects();
void freeObjects();