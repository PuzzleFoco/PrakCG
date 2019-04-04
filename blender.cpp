#include "blender.h"
#include <stdio.h>

object3D *blender[num_objects];
const char* blender_paths[num_objects] = {
	"Krangerüst.obj",
	"Kranhook.obj",
	"Rail.obj" ,
	"Kranhookstab.obj" ,
	"HMitte.obj" ,
	"cargo.obj" ,
	"Spotlight.obj",
	"Cube.obj"
};

void loadObjects() {
	for (int i = 0; i < num_objects; i++) {
		if (strlen(blender_paths[i])) {// alle Objekte Laden, wenn der Pfad verfügbar ist (Pfad != "")
			char file[255] = "";
			strcat(file, "./Scene1/");
			strcat(file, blender_paths[i]);
			blender[i] = loadobject(file, true);
		}
	}
	// Materialeigenschaften
	set_obj_material(blender[Krangerust],0.8, 0.8, 0.8, 1.0, 1.0, 128.0, 0.0);
	set_obj_material(blender[Kranhook], 0.1, 0.1, 0.1, 1.0, 1.2, 64.0, 0.0);
	set_obj_material(blender[Rail], 0.2, 0.2, 0.2, 1.0, 1.3, 46.0, 0.0);
	set_obj_material(blender[Kranhookstab], 0.3, 0.3, 0.3, 1.0, 1.4, 100.0, 0.0);
	set_obj_material(blender[HMitte], 0.9, 0.5, 0.1, 1.0, 1.2, 64.0, 0.0);
	set_obj_material(blender[cargo], 0.5, 0.5, 0.9, 0.3, 0.5, 80.0, 0.0);
	set_obj_material(blender[Cube], 0.5, 0.5, 0.9, 0.3, 0.5, 80.0, 0.0);
}

void freeObjects() {
	for (int i = 0; i < num_objects; i++)
		freeobject(blender[i]);
}