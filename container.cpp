#include <cmath>
#include "container.h"
#include "kran.h"
#include "blender.h"

container::container()
{
	x = 0;
	y = 0;
	z = 0;
	rotated = 0;
	isAttached = false;
}

void container::update(const kran & kra)
{
	if (isAttached)
	{
		x = kra.posKrangerust;
		y = kra.posHook;
		z = kra.hookdown;
		//rotated = kra.hookRotate;
	}
}

void container::draw()
{
	glPushMatrix();
		glTranslatef(x, z, y);
		glRotatef(rotated, 0, 1, 0);

		/*
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		glEnable(GL_CULL_FACE); 
		glCullFace(GL_BACK);
		glBegin(GL_QUADS);
		glColor3f(1, 1, 1);									// Polygon ist GELB
		glNormal3f(0, 0, 1);									// Normale ist Z

		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.6f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(1.0f, -1.6f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(1.0f, 1.6f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f, 1.6f, 0.0f);
		glEnd();
		*/

		drawobject(blender[cargo]);
	glPopMatrix();
}

void container::attatch(const kran & kra)
{
	if (isAttached) {
		if (kra.hookdown == 0) {
			isAttached = false;
			x = kra.posKrangerust;
			y = kra.posHook;
		}
		else return;
	}
	else if (kra.hookdown == 0 && (fabs(kra.posKrangerust - x)) < 0.5 && (fabs(kra.posHook - y)) < 0.5) {
		isAttached = true;
	}
}
