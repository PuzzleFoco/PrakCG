#include <list>

#include "kran.h"
#include "container.h"
#include "blender.h"
#include "help.h"
#include "light.h"
#include "Objects.h"
#include "Texturen.h"
#include "MyCube.h"

kran::kran()
{
	speedKrangerust = 0;
	speedHook = 0;
	speedHookdown = 0;
	posKrangerust = 0;
	posHook = 0;
	hookdown = 0;
	hookRotate = 0;
	hookMitteScale = 0;
	rotatinglight = true;
	spotlight = true;
	isAttachable = false;
	hasAttached = false;
}

void kran::update()
{
	canAttach();

	//Begrenzung der Animationsparameter:
	if (speedKrangerust > 3.0f) speedKrangerust = 3.0f; //Max 5 schnell
	else if (speedKrangerust < -3.0f) speedKrangerust = -3.0f; //Max -5 schnell

	//Begrenzung der Animationsparameter:
	if (speedHook > 1.0f) speedHook = 1.0f; //Max 5 schnell
	else if (speedHook < -1.0f) speedHook = -1.0f; //Max -5 schnell

	if (speedHookdown > 0.5f) speedHookdown = 0.5f;
	else if (speedHookdown < -0.5f) speedHookdown = -0.5f;


	float speed_perframeKrangerust = speedKrangerust / (MAX(1, cg_help::getFps())); //Geschwindigkeit Krangerüst
	float speed_perframeHook = speedHook / (MAX(1, cg_help::getFps())); //Geschwindigkeit
	float speed_perframeHookdown = speedHookdown / (MAX(1, cg_help::getFps())); //Geschwindigkeit

	posKrangerust += speed_perframeKrangerust; //Positionsupdate
	posHook += speed_perframeHook;
	hookdown += speed_perframeHookdown;

	if (posKrangerust > 6.4) posKrangerust = 6.4;
	else if (posKrangerust < -6.4) posKrangerust = -6.4;

	if (posHook > 1.5) posHook = 1.5;
	else if (posHook < -1.5) posHook = -1.5;

	if (hookdown > 0.9) hookdown = 0.9;
	else if (hookdown < 0) hookdown = 0;

	if (hookRotate > 30) hookRotate = 30;
	else if (hookRotate < -30) hookRotate = -30;

	if (posHook > 0.8 && hookRotate > 10.0) posHook = 0.8;
	else if (posHook > 0.8 && hookRotate < -10.0) posHook = 0.8;
	else if (posHook < -0.8 && hookRotate > 10.0) posHook = -0.8;
	else if (posHook < -0.8 && hookRotate < -10.0) posHook = -0.8;

	else if (posHook > 1.2 && hookRotate > 5.0) posHook = 1.2;
	else if (posHook > 1.2 && hookRotate < -5.0) posHook = 1.2;
	else if (posHook < -1.2 && hookRotate > 5.0) posHook = -1.2;
	else if (posHook < -1.2 && hookRotate < -5.0) posHook = -1.2;

	speedKrangerust *= 0.9f; //Reibung
	speedHook *= 0.9f; //Reibung 
	speedHookdown *= 0.8f;
}

void kran::draw()
{
	glPushMatrix();
	drawobject(blender[Rail]);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(posKrangerust, 0, 0);
	drawobject(blender[Krangerust]);
	
	setSchild();

	glPushMatrix();
	glTranslatef(0, 0, posHook);
	drawobject(blender[Kranhook]);

	glPushMatrix();
	glTranslatef(0, hookdown, 0);
	glRotatef(hookRotate, 0, 1, 0);
	drawobject(blender[Kranhookstab]);
	drawobject(blender[HMitte]);
	glPopMatrix();
	rotatingLight();
	glPopMatrix();
	setAttachableLight();
	glPopMatrix();
	setHeadlight();
	drawground();
}

void kran::rotatingLight() {
	static bool init = true;
	static cg_light r(7);

	if (init) {
		/*
		l.setAmbient(0.5, 0.5, 0.5, 1); //Ganzheitliches Leuchten
		l.setDiffuse(0.2, 0.2, 1, 1);		//Volumenleuchten
		l.setSpecular(0.1, 0.1, 1, 1);	//Glanzleuchten
		l.setAttentuation(1, 0.05, 0.0);	//Lichtabschwächung

		Rundumleuchte
		Position: posKrangerust, 2.2 , posHook
		Farbe: blau
		halbe Öffnungswinkel: 20°
		Exponent: 10
		GL_Licht: GL_LIGHT7
		*/

		r.setPosition(posKrangerust, hookdown + 2.2, posHook, 1);
		r.setSpotlight(0.0, -0.3, 1.0, 20.0, 10.0);
		r.setAmbient(0.1, 0.1, 0.1, 1.0);
		r.setDiffuse(0.2, 0.2, 1.0, 1.0);
		r.setSpecular(0.2, 0.2, 1.0, 1.0);
		r.setAttentuation(1.0, 0.05, 0.0);
		init = false;
	}

	static float w = 0.0; //Drehwinkel der Rundumleuchte
	if (rotatinglight) {
		w += 3;
		r.enable();

		r.setSpotRotation(0, w, 0);
		r.draw();
		glColor3f(0.2, 0.2, 1.0);
	}
	else {
		r.disable();
		r.draw();
		glColor3f(0.0, 0.0, 0.3);
	}

	// Objekt
	glPushMatrix();
		glTranslatef(r.pos[0], hookdown+2.2, r.pos[2]);
		glRotatef(w, 0, 1, 0);
		GLUquadricObj *q = gluNewQuadric();
		gluCylinder(q, 0.0, 0.05, 0.15, 20.0, 20.0);
		glTranslatef(0, 0, 0.15);
		gluDisk(q, 0.0, 0.05, 30, 30);
		gluDeleteQuadric(q);
	glPopMatrix();

	glPushMatrix();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glTranslatef(0, hookdown + 2.3, 0);
		glScalef(0.8, 0.8, 0.8);
		drawobject(blender[Cube]);
		glDisable(GL_BLEND);
	glPopMatrix();
}

void kran::setHeadlight()
{
	static bool init = true;
	static cg_light spot(6);

	if (init) {

		spot.setPosition(3.0f, 0.0f, 17.0f, 1.0f);
		spot.setSpotlight(0.0f, 0.0f, -1.0f, 20.0f, 64.0f);
		spot.setAmbient(0.0f, 0.0f, 0.0f, 1.0f);
		spot.setDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
		spot.setSpecular(0.9f, 0.9f, 0.9f, 1.0f);

	}


	if (spotlight) {
		set_obj_material(blender[Spotlight],1.0, 1.0, 0.0, 1.0, 1.3, 46.0, 0.0);
		spot.enable();
	}
	else {
		set_obj_material(blender[Spotlight], 0.1, 0.1, 0.0, 1.0, 1.3, 46.0, 0.0);
		spot.disable();
	}

	spot.draw();
	glPushMatrix();
		glTranslatef(0.0f, 0.0f, 17.0f);
		drawobject(blender[Spotlight]);
	glPopMatrix();
}

void kran::setAttachableLight()
{
	static bool init = true;
	static cg_light point(6);
	if (init) 
	{
			point.setPosition(1.0f, 2.0f, 2.0f, 1.0f);
			point.setAmbient(0.1f, 0.1f, 0.1f, 1.0f);
			point.setDiffuse(0.9f, 0.9f, 0.9f, 1.0f);
			point.setSpecular(0.9f, 0.9f, 0.9f, 1.0f);
	}
	if (isAttachable) {
		glColor3f(0, 1, 0);
		point.setDiffuse(0.2, 1, 0.2, 1);
		point.enable();
	}
	else {
		glColor3f(1, 0, 0);
		point.setDiffuse(1, 0.2, 0.2, 1);
		point.enable();
	}
	point.draw();
	glPushMatrix();
		glTranslatef(point.pos[0], point.pos[1], point.pos[2]);
		glutSolidSphere(0.03, 20, 20);
	glPopMatrix();
}

void kran::canAttach() 
{
	hasAttached = false;
	for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
		if (it->isAttached) hasAttached = true;
		break;
	}
	if (hasAttached && hookdown == 0)
	{
		isAttachable = true;
		return;
	}
	else if (hasAttached && hookdown > 0)
	{
		isAttachable = false;
		return;
	}

	if (!hasAttached) {
		for (std::list<container>::iterator it = objects.listContainer.begin(); it != objects.listContainer.end(); ++it) {
			if (hookdown == 0 && (fabs(posKrangerust - it->x)) < 0.5 && (fabs(posHook - it->y)) < 0.5) {
				isAttachable = true;
				break;
			}
			else isAttachable = false;
		}
	}
}

void kran::setSchild() {
	glEnable(GL_TEXTURE_2D);
	textures[MaximaleTragkraft].bind();

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);									// Polygon ist GELB
	glNormal3f(0, 0, 1);									// Normale ist Z

	glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.6f, 1.0f, -2.0f);	//Untenlinks
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.6f, 1.0f, -2.0f);	//Untenrechts
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.6f, 1.9f, -2.0f);		//Oben rechts
	glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.6f, 1.9f, -2.0f);	//Oben links
	glEnd();
	glDisable(GL_TEXTURE_2D);

	glPushMatrix();
		glTranslatef(0, 1.45, -2.01);
		glScalef(1.2, 0.9, 0.01);
		glColor3f(0.8, 0.8, 0.8);
		MyCube cube;
		cube.draw();
	glPopMatrix();	
}

void kran::drawground() 
{
	int tessX, tessZ;
	tessX = 200;
	tessZ = 200;

	int useVBOs = 0;

	float sizeX, sizeZ;
	sizeX = 20;
	sizeZ = 20;

	// Untergrund aus einem Array von Quadstrips

	if (!tessX) tessX = 1;						// Vermeiden DIV/0, d.h. wir haben immer mind. 1 Quad
	if (!tessZ) tessZ = 1;

	float stepX = sizeX / tessX;					// die "Schrittweite", d.h. Größe eines Quads in Modellkoordinaten
	float stepZ = sizeZ / tessZ;

	float posZ = 0.0;								// wir beginnen bei Z == 0

	GLfloat curX = 0.0;								// und bei X == 0
	GLfloat curZ = 0.0;

	glPushMatrix();
	glTranslatef(-sizeX / 2.0, posZ, -sizeZ / 2.0);	// der Boden soll mittig liegen

	glNormal3f(0, 1.0, 0);							// und die Normale in Richtung Y zeigen (nach oben)
										// wenn wir traditionell zeichnen
	for (int n = 0; n <= tessZ; n++)				// arbeiten wir uns zeilenweise von 0 bis tessZ vorwärts
	{
		curX = 0.0;									// Beginnen an den aktuellen Quadstrip bei X == 0

		glBegin(GL_QUAD_STRIP);
		glColor3f(0.6, 0.6, 0.6);
		for (int i = 0; i <= tessX; i++)			// und gehen bis X == tessX
		{
				glVertex3f(curX, 0.0, curZ);			// generieren die Vertices
				glVertex3f(curX, 0.0, curZ + stepZ);

				//glMatrixMode(GL_TEXTURE);

				curX = curX + stepX;					// und gehen einen Schritt weiter
			}
		glEnd();									// und schließen den aktuellen Quadstrip ab
		curZ = curZ + stepZ;						// und gehen zur nächsten Zeile
	}
	

	glDisable(GL_COLOR_MATERIAL);								// glColor() setzt nur noch Farbwerte, keine Materialien
	glPopMatrix();
}
