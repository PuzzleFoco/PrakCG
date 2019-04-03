#include "MyCube.h"
#include "Include/freeglut.h"


MyCube::MyCube()
{
}


MyCube::~MyCube()
{
}

void MyCube::draw()
{
	glPushMatrix();
	glTranslatef(0, 0, 0);
	glutSolidCube(1);
	glPushAttrib(GL_CURRENT_BIT);
	glPushAttrib(GL_DEPTH_BUFFER_BIT);
	glColor3f(0.1, 0.1, 0.1);
	glDepthFunc(GL_LEQUAL);
	glutWireCube(1.005);
	glPopAttrib();
	glPopAttrib();
	glPopMatrix();
}
