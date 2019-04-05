#include <cmath>
#include "container.h"
#include "kran.h"
#include "blender.h"
#include "Texturen.h"

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
	}
}

void container::draw()
{
	glPushMatrix();
		glTranslatef(x, z, y);
		glRotatef(rotated, 0, 1, 0);
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
