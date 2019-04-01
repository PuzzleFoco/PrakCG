#define _CRT_SECURE_NO_WARNINGS

#include <cstdlib>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <list>
#include <iostream>

#include "Include/freeglut.h"
#include "Include/glext.h"
#include "help.h"
#include "window.h"
#include "wavefront.h"
#include "input.h"
#include "light.h"
#include "kran.h"
#include "container.h"
#include "blender.h"
#include "Objects.h"

using namespace std;


const char *spalte1[] =
{
	"Maus",
	"",
	"linke Taste      Kamerabewegung",
	"mittlere Taste   Zoom",
	"rechte Taste     Kontextmenue",

	NULL
};
const char *spalte2[] =
{
	"Tastatur:",
	"",
	"f,F    - Framerate (An/Aus)",
	"l,L    - Licht global (An/Aus)",
	"h,H,F1 - Hilfe (An/Aus)",
	"w,W    - WireFrame (An/Aus)",
	"k,K    - Koordinatensystem (An/Aus)",
	"Cursor - Fahren/Lenken des Trucks",
	"Enter  - An-/Abkoppeln des Trailers",
	"x,X    - Scheinwerfer (An/Aus)",
	"y,Y    - Kameramodus wechseln",
	"r,R    - Truck und Trailer resetten",
	"",
	"ESC    - Beenden",

	NULL
};


void setCamera();		// Kamera platzieren, siehe Maus-Callbacks
void drawScene();		// Zeichnet die Szene im Weltkoordinatensystem

void loadObjects();


/////////////////////////////////////////////////////////////////////////////////
//	Anfang des OpenGL Programmes
/////////////////////////////////////////////////////////////////////////////////
int main(int argc, char **argv)
{
	init(argc, argv);

	// hier Objekte laden, erstellen etc.
	loadObjects();

	// Die Hauptschleife starten
	glutMainLoop();
	return 0;
}



void displayFunc()
{
	// Hilfe-Instanzen
	cg_help help;
	cg_globState globState;
	cg_key key;

	// Tastatur abfragen
	// Achtung: einmaliges Betätigen funktioniert so nur mit glutIgnoreKeyRepeat(true) (siehe main())
	if (key.keyState(27))
	{
		exit(0); // Escape -> Programm beenden
	}
	else if (1 == key.keyState('f') || 1 == key.keyState('F'))
	{
		help.toggleFps();	// Framecounter on/off
	}
	else if (1 == key.keyState('h') || 1 == key.keyState('H') || 1 == key.specialKeyState(GLUT_KEY_F1))
	{
		help.toggle();	// Hilfetext on/off
	}
	else if (1 == key.keyState('k') || 1 == key.keyState('K'))
	{
		help.toggleKoordsystem();	// Koordinatensystem on/off
	}
	else if (1 == key.keyState('w') || 1 == key.keyState('W'))
	{
		globState.drawMode = (globState.drawMode == GL_FILL) ? GL_LINE : GL_FILL; // Wireframe on/off
	}
	else if (1 == key.keyState('l') || 1 == key.keyState('L'))
	{
		globState.lightMode = !globState.lightMode;	// Beleuchtung on/off
	}
	else if (1 == key.keyState('c') || 1 == key.keyState('C'))
	{
		globState.cullMode = !globState.cullMode; // Backfaceculling on/off
	}
	else if (1 == key.keyState('i') || 1 == key.keyState('I'))
	{
		globState.cameraHelper[0] = 0;	// Initialisierung der Kamera
		globState.cameraHelper[1] = 0;
	}


	// Szene zeichnen: CLEAR, SETCAMERA, DRAW_SCENE

	// Back-Buffer neu initialisieren
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Zeichenmodus einstellen (Wireframe on/off)
	glPolygonMode(GL_FRONT_AND_BACK, globState.drawMode);

	// Backface Culling on/off);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	if (globState.cullMode) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	// Farbmodus oder Beleuchtungsmodus ?
	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	if (globState.lightMode == GL_TRUE) // Beleuchtung aktivieren
	{
		float m_amb[4] = { 0.2, 0.2, 0.2, 1.0 };
		float m_diff[4] = { 0.2, 0.2, 0.6, 1.0 };
		float m_spec[4] = { 0.8, 0.8, 0.8, 1.0 };
		float m_shine = 32.0;
		float m_emiss[4] = { 0.0, 0.0, 0.0, 1.0 };

		SetMaterial(GL_FRONT_AND_BACK, m_amb, m_diff, m_spec, m_shine, m_emiss);

		SetLights();

		glEnable(GL_LIGHTING);
	}
	else   // Zeichnen im Farbmodus
	{
		glDisable(GL_LIGHTING);
		glColor4f(0.2, 0.2, 0.6, 1.0);
	}

	// Kamera setzen (spherische Mausnavigation)
	setCamera();

	// Koordinatensystem zeichnen
	help.drawKoordsystem(-8, 10, -8, 10, -8, 10);


	// Geometrie zeichnen /////////////////!!!!!!!!!!!!!!!!!!!!!!!!///////////////////////
	drawScene();


	// Hilfetext zeichnen
	help.draw();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);

	glFlush();				// Daten an Server (fuer die Darstellung) schicken
	glutSwapBuffers();		// Buffers wechseln
}


//kran kra;
//list<container> listContainer;
//container containe;


char cameraMode = 0;
float cockpit[3] = { 0.3f, 2.4f, 4.4f };


/////////////////////////////////////////////////////////////////////////////////
//	Kamerafunktion
/////////////////////////////////////////////////////////////////////////////////
void setCamera()
{
	cg_mouse mouse;
	// Ansichtstransformationen setzen,
	// SetCamera() zum Beginn der Zeichenfunktion aufrufen
	double x, y, z, The, Phi;
	static double radius = 25;
	// Maus abfragen
	if (cg_mouse::buttonState(GLUT_LEFT_BUTTON))
	{
		cg_globState::cameraHelper[0] += mouse.moveX();
		cg_globState::cameraHelper[1] += mouse.moveY();
	}
	if (cg_mouse::buttonState(GLUT_MIDDLE_BUTTON))
	{
		radius += 0.1 * mouse.moveY();
		if (radius < 1.0) radius = 1.0;
	}
	Phi = 0.2 * cg_globState::cameraHelper[0] / cg_globState::screenSize[0] * M_PI + M_PI * 0.5;
	The = 0.2 * cg_globState::cameraHelper[1] / cg_globState::screenSize[1] * M_PI;
	x = radius * cos(Phi) * cos(The);
	y = radius * sin(The);
	z = radius * sin(Phi) * cos(The);
	int Oben = (The <= 0.5 * M_PI || The > 1.5 * M_PI) * 2 - 1;

	// globale, mausgesteuerte Sicht
	if (cameraMode == 0)
		gluLookAt(x, y, z, 0, 0, 0, 0, Oben, 0);
	// "Trailer"-Ansicht
	/*
	else if (cameraMode == 1)
	{
		float alpha = trailer.rot * M_PI / 180.0f;
		float c = cos(alpha), s = sin(alpha);
		float vec[3] = { s*-10.0f, 50, c*-10.0f };
		gluLookAt(trailer.pos[0] + vec[0], vec[1], trailer.pos[1] + vec[2],
			truck.pos[0], 0, truck.pos[1],
			0, Oben, 0);
	}
	// "Cockpit"-Ansicht
	else
	{
		float alpha = truck.rot * M_PI / 180.0f;
		float c = cos(alpha), s = sin(alpha);
		float pos[3] = { cockpit[0] * c + cockpit[2] * s + truck.pos[0],
						 cockpit[1],
						-cockpit[0] * s + cockpit[2] * c + truck.pos[1] };
		// (x,y,z) noch um Winkel alpha der Zugmaschine drehen (um Y)
		float vec2[3] = { x*c + s * z, y, -s * x + c * z };
		gluLookAt(pos[0], pos[1], pos[2],
			pos[0] + vec2[0], pos[1] + vec2[1], pos[2] + vec2[2],
			0, Oben, 0);
	}
	*/
}

/*
void erstellecontainer()
{
	for (int i = 0; i < 1; i++)
	{
		container contemp;
		contemp.x = i;
		listContainer.push_back(contemp);
	}
}
*/


void drawScene()
{
	// Zeichnet die Szene 1x im Weltkoordinatensystem

	// TODO: Boden bei Y=0 zeichnen
	/*
	objects[GROUND_OBJ1].draw();
	objects[GROUND_OBJ2].draw();
	*/
	// Animationsparameter berechnen
	cg_key key;

	if (key.specialKeyState(GLUT_KEY_UP))			objects.kra.speedKrangerust += 0.25f;	// Beschleunigen
	else if (key.specialKeyState(GLUT_KEY_DOWN))	objects.kra.speedKrangerust -= 0.25f;	// Bremsen

	if (key.specialKeyState(GLUT_KEY_RIGHT))		objects.kra.speedHook += 0.25f;	// Beschleunigen
	else if (key.specialKeyState(GLUT_KEY_LEFT))	objects.kra.speedHook -= 0.25f;	// Bremsen

	if (key.keyState('+'))
	{
		objects.kra.speedHookdown += 0.2f;
	}
	if (key.keyState('-'))
	{
		objects.kra.speedHookdown -= 0.2f;
	}

	if (key.keyState('x') == 1|| key.keyState('X') == 1) objects.kra.rotatinglight = !objects.kra.rotatinglight;

	if (key.keyState('t') == 1|| key.keyState('T') == 1) objects.kra.spotlight = !objects.kra.spotlight;

	/*
	if (key.keyState('1')) kra.eins += 0.1;
	if (key.keyState('4')) kra.eins -= 0.1;
	if (key.keyState('2')) kra.zwei += 0.1;
	if (key.keyState('5')) kra.zwei -= 0.1;
	if (key.keyState('3')) kra.drei += 0.1;
	if (key.keyState('6')) kra.drei -= 0.1;

	cout << "Eins: " << kra.eins << " Zwei: " << kra.zwei << " Drei: " << kra.drei << endl;
	*/

	if (key.specialKeyState(GLUT_KEY_PAGE_UP))
	{
		objects.kra.hookRotate++;
		for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
			if (it->isAttached) it->rotated++;
			if (it->rotated > objects.kra.hookRotate) it->rotated = objects.kra.hookRotate;
		}
	}
	else if (key.specialKeyState(GLUT_KEY_PAGE_DOWN)) {
		objects.kra.hookRotate--;
		for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
			if (it->isAttached) it->rotated--;
			if (it->rotated < objects.kra.hookRotate) it->rotated = objects.kra.hookRotate;
		}
	}

	
	if (key.keyState(char(13)) == 1) {
		for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
			it->attatch(objects.kra);
		}
	}
	
	objects.kra.update();
	objects.kra.draw();
		
	for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
		it->update(objects.kra);
		it->draw();
	}	
	
}
