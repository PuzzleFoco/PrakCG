#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Include/freeglut.h"
#define EXPORT_EXT
#include "wavefront.h"

/* wavefront */

int init_extensions ()
{
	glGenBuffers = ( PFNGLGENBUFFERSARBPROC ) wglGetProcAddress( "glGenBuffers" );
	glBindBuffer = ( PFNGLBINDBUFFERARBPROC ) wglGetProcAddress( "glBindBuffer" );
	glBufferData = ( PFNGLBUFFERDATAARBPROC ) wglGetProcAddress( "glBufferData" );
	glDeleteBuffers = ( PFNGLDELETEBUFFERSPROC ) wglGetProcAddress( "glDeleteBuffers" );

	if ( glGenBuffers ) return ( 1 );
	else return ( 0 );
}

inline float* normieren ( float v[3] )
{
	float l = 1.0f / sqrt( v[0] * v[0] + v[1] * v[1] + v[2] * v[2] );
	v[0] *= l;
	v[1] *= l;
	v[2] *= l;
	return v;
}


inline void cross_product( float *n, float *a, float *b )
{
	n[0] = a[1] * b[2] - a[2] * b[1];
	n[1] = a[2] * b[0] - a[0] * b[2];
	n[2] = a[0] * b[1] - a[1] * b[0];
}

void set_obj_material ( object3D *obj, float red, float green, float blue, float alpha, float spec, float shine, float emis )
{
	if ( !obj ) return;
	obj->color[0] = red;
	obj->color[1] = green;
	obj->color[2] = blue;
	obj->color[3] = alpha;
	obj->amb[0] = 0.1;
	obj->amb[1] = 0.1;
	obj->amb[2] = 0.1;
	obj->amb[3] = alpha;
	obj->diff[0] = red;
	obj->diff[1] = green;
	obj->diff[2] = blue;
	obj->diff[3] = alpha;
	obj->spec[0] = spec;
	obj->spec[1] = spec;
	obj->spec[2] = spec;
	obj->spec[3] = alpha;
	obj->shine = shine;
	// Emission = r,g,b * emis
	obj->emis[0] = red * emis;
	obj->emis[1] = blue * emis, obj->emis[2] = green * emis, obj->emis[3] = alpha * emis;
}

void set_obj_pos ( object3D *obj, float x, float y, float z )
{
	if ( !obj ) return;
	obj->pos[0] = x;
	obj->pos[1] = y;
	obj->pos[2] = z;
}

void set_obj_normalmode ( object3D *obj, int normalmode )
{
	if ( !obj ) return;
	obj->normal_mode = normalmode;
}


bool read_cluster_vertex( char **r_line, int *p, int *t, int *n )
{

	char *h = r_line[0];										// Zeiger auf den Anfang der Zeile
	p[0] = -1;
	t[0] = -1;
	n[0] = -1;

	while ( h[0] != 0 && h[0] == ' ' ) h++;							// überlese führende Blanks

	if ( sscanf( h, "%i", &p[0] ) == 1 )  							// Vertex-Index wurde gelesen, es folgt ein Slash oder ein Blank bzw. das Zeilenende
	{
		// Wenn ein Slash folgt, dann haben wir einen Vertex-Cluster

		while ( h[0] != 0 && h[0] != '/' && h[0] != ' ' ) h++;		// Vorsetzen bis Zeilenende, Slash oder Leerzeichen

		switch ( h[0] )
		{
			case 0:												// Zeilenende erreicht
				r_line[0] = h;
				return true;
			case ' ':											// Leerzeichen, d.h. kein Vertex-Cluster
				r_line[0] = h;
				return true;
			case '/':											// Vertex-Cluster
				h++;											// den Slash überlesen, es folgt  die Texturkoordinate oder ein zweiter Slash

				sscanf( h, "%i", &t[0] );							// wir versuchen die Texturkoordinate zu lesen
				while ( h[0] != 0 && h[0] != '/' && h[0] != ' ' ) h++;	// zum zweiten Slash vorsetzen

				if ( h[0] == '/' )  								// die Vertexnormale muss eigentlich folgen, wenn sie fehlt setzen wir die Standardnormale
				{
					h++;										// den zweiten Slash überlesen
					if ( h[0] == ' ' )  							// Fehler: Normale fehlt
					{
						n[0] = 0;
					}
					else
					{
						sscanf( h, "%i", &n[0] );
						while ( h[0] != 0 && h[0] != ' ' ) h++;		// über die Vertexnormale vorsetzen
					}
					r_line[0] = h;
					return true;
				}
				break;
			default:
				p[0] = -1;
				r_line[0] = h;
				return false;
		}
	}
	r_line[0] = h;
	return false;

}


object3D* loadobject ( const char *filename, bool use_vbos, float red, float green, float blue, float x, float y, float z )
{
	struct object3D *obj = NULL;

	if ( !filename ) return NULL;
	FILE *f = fopen( filename, "r" );
	if ( !f ) return NULL;

	int npoints = 0, ntris = 0, nnormals = 0, ntexcoords = 0;
#define ZEILENLAENGE 200
	char s[ZEILENLAENGE];

	bool clusternormals = false;
	bool clustertexcoords = false;

// Pass1: Speicherbedarf ermitteln
	int line = 0;
	while ( !feof( f ) )
	{
		fgets( s, ZEILENLAENGE - 1, f );					// eine Zeile lesen
		++line;
		switch ( s[0] )
		{
			case 'v':
				switch ( s[1] )
				{
					case ' ':
						npoints++;
						break;
					case 'n':
						clusternormals = true;
						nnormals++;
						break;
					case 't':
						clustertexcoords = true;
						ntexcoords++;
						break;
				}
				break;
			case 'f':
				int p[3];
				int n[3];
				int t[3];

				char *h = &s[1];	// starte am Zeichen s[1]

				if ( read_cluster_vertex( &h, &p[0], &t[0], &n[0] ) &&
						read_cluster_vertex( &h, &p[1], &t[1], &n[1] ) &&
						read_cluster_vertex( &h, &p[2], &t[2], &n[2] ) )
				{
					++ntris;										// erstes Dreieck
					// TODO: Polygone zerlegen
					while ( read_cluster_vertex( &h, &p[0], &t[0], &n[0] ) )	// weitere Punkte
						ntris++;
				}
				else
				{
					printf( "Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line );
					return NULL;
				}
				break;
		}
	}

// #define __DEBUG
#ifdef __DEBUG

	printf( "OBJ Loader PASS 1\n" );
	printf( "npoints: %d, ntris: %d, nnormals: %d, ntexcoords: %d\n", npoints, ntris, nnormals, ntexcoords );

#endif

// PASS2: Punkte, Normalen und Texturkoordinaten laden

	if ( npoints > 0 && ntris > 0 )
	{

		obj = ( object3D* )malloc( sizeof( object3D ) );											// Objekt anlegen
		memset( obj, 0, sizeof( object3D ) );												// Objekt initialisieren

		obj->points = ( GLfloat* )malloc( npoints * 3 * sizeof( GLfloat ) );							// Punktarray anlegen
		obj->tris  = ( GLint* )malloc( ntris * 3 * sizeof( GLint ) );									// Index-Array für Flächen anlegen

		if ( clustertexcoords )
		{
			use_vbos = false;
			obj->clustertexcoords = true;
			obj->i_texcoords = ( GLint* )malloc( 3 * ntris * sizeof( GLint ) );						// importierte Texturkoordinatenindexe
			obj->texcoords = ( GLfloat* )malloc( ( ntexcoords + 1 ) * 2 * sizeof( GLfloat ) );			// u,v - Koordinaten aus dem File
			memset( obj->i_texcoords, 0, 3 * ntris * sizeof( GLint ) );
			memset( obj->texcoords, 0, ( ntexcoords + 1 ) * 2 * sizeof( GLfloat ) );
		}

		if ( clusternormals )
		{
			use_vbos = false;
			obj->clusternormals = true;
			nnormals++;																		// wir nehmen noch die Standardnormale dazu
			obj->i_normals = ( GLint* )malloc( 3 * ntris * sizeof( GLint ) );							// importierte Normalenindexe
			obj->normals = ( GLfloat* )malloc( ( nnormals ) * 3 * sizeof( GLfloat ) );				 // nx, ny, nz aus dem File
			memset( obj->i_normals, 0, 3 * ntris * sizeof( GLint ) );
			memset( obj->normals, 0, ( nnormals ) * 3 * sizeof( GLfloat ) );
			obj->normals[0] = 0;
			obj->normals[1] = 0;
			obj->normals[2] = 1;						// Standard-Normale
			obj->numnormals = nnormals;
		}

		int hnormals = 1;																	// die Standard-Normale steht im Index 0
		npoints = 0;
		ntris = 0;
		ntexcoords = 0;													// Initialisierung

		rewind( f );

		line = 0;
		while ( !feof( f ) )
		{
			fgets( s, ZEILENLAENGE - 1, f );					// eine Zeile lesen
			++line;

			switch ( s[0] )
			{
				case 'v':
					switch ( s[1] )
					{
						case ' ':
							{
								float *p = &obj->points[3 * npoints];									// pointer auf Vertex setzen
								if ( sscanf( &s[2], "%f %f %f", &p[0], &p[1], &p[2] ) == 3 )					// 3 Floats lesen
									npoints++;
								else
								{
									printf( "Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line );
									return NULL;
								}
							}
							break;
						case 'n':
							{
								float *n = &obj->normals[3 * ( hnormals )];
								if ( sscanf( &s[2], "%f %f %f", &n[0], &n[1], &n[2] ) == 3 )					// 3 Floats lesen
									hnormals++;
								else
								{
									printf( "Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line );
									return NULL;
								}
							}
							break;
						case 't':
							{
								float *t = &obj->texcoords[2 * ( ntexcoords + 1 )];
								if ( sscanf( &s[2], "%f %f", &t[0], &t[1] ) == 2 )							// 2 Floats lesen
									ntexcoords++;
								else
								{
									printf( "Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line );
									return NULL;
								}
							}
							break;
					}
					break;
				case 'f':
					int *p = &obj->tris[3 * ntris];
					int *on = &obj->i_normals[3 * ntris];
					int *ot = &obj->i_texcoords[3 * ntris];
					int n[3];
					int t[3];

					char *h = &s[1];	// starte am Zeichen s[1]

					if ( read_cluster_vertex( &h, &p[0], &t[0], &n[0] ) &&
							read_cluster_vertex( &h, &p[1], &t[1], &n[1] ) &&
							read_cluster_vertex( &h, &p[2], &t[2], &n[2] ) )
					{
						--p[0];
						--p[1];
						--p[2];											// Indizes dekrementieren (1->0)
						for ( int i = 0; i < 3; i++ )  										// Textur- und Normalenindizes nur dekrementieren, wenn gelesen
						{
							if ( clustertexcoords )
							{
								if ( t[i] > 0 )
								{
									if ( t[i] <= ntexcoords )   							// Index liegt im gültigen Bereich
										ot[i] = t[i];										// sonst steht da (-1)
									else
									{
										//printf("Index zu groß, korrigiert\n\r");
										ot[i] = 0;
									}
								}
								else
									ot[i] = 0;											// die Standard-Texturkoordinate 0,0 steht im Index 0
							}

							if ( clusternormals )
							{
								if ( n[i] > 0 )
									on[i] = n[i];
								else
									on[i] = 0;											// die Standardnormale 0,0,1 steht im Index 0
							}
						}
						++ntris;														// erstes Dreieck gelesen
						// TODO: Surfaces mit mehr als 3 Ecken zerlegen
						int hp;
						int ht;
						int hn;
						while ( read_cluster_vertex( &h, &hp, &ht, &hn ) )  				// weitere Punkte
						{
							int *q = &obj->tris[3 * ntris];									// das nächste Dreieck
							q[0] = p[0];
							q[1] = p[2];
							q[2] = hp - 1;
							p = q;
							if ( clusternormals )
							{
								int *hon = &obj->i_normals[3 * ntris];
								hon[0] = on[0];
								hon[1] = on[2];
								hon[2] = hn;
								on = hon;
							}
							if ( clustertexcoords )
							{
								int *hot = &obj->i_texcoords[3 * ntris];
								hot[0] = ot[0];
								hot[1] = ot[2];
								hot[2] = ht;
								ot = hot;
							}
							ntris++;
						}
					}
					else
					{
						printf( "Fehler in Wavefrontdatei %s in Zeile %d\n", filename, line );
						return NULL;
					}
					break;
			}
		}

		obj->numpoints = npoints;
		obj->numtris = ntris;

		//printf("npoints: %d, ntris: %d\n", npoints, ntris);

	}
// END PASS 2

	fclose( f );


#ifdef __DEBUG

	printf( "\nOBJ Loader PASS 2\n" );

	printf( "Vertices: \n" );
	for ( int i = 0; i < obj->numpoints; i++ )
	{
		printf( "Index: %d, x: %3.5f, y: %3.5f, z: %3.5f\n", i, obj->points[3 * i], obj->points[3 * i + 1], obj->points[3 * i + 2] );
	};

	printf( "\n\nNormals: \n" );
	for ( int i = 0; i < obj->numnormals; i++ )
	{
		printf( "Index: %d, x: %3.5f, y: %3.5f, z: %3.5f\n", i, obj->normals[3 * i], obj->normals[3 * i + 1], obj->normals[3 * i + 2] );
	};

	printf( "\n\nTexCoords: \n" );
	for ( int i = 0; i < ntexcoords; i++ )
	{
		printf( "Index: %d, u: %3.3f, v: %3.3f\n", i, obj->texcoords[2 * i], obj->texcoords[2 * i + 1] );
	};

	printf( "\n\nTRIs: \n" );
	for ( int i = 0; i < obj->numtris; i++ )
	{
		if ( obj->clustertexcoords )
			printf( "Index: %d, a: %d/%d/%d, b: %d/%d/%d, c: %d/%d/%d\n", i,
					obj->tris[3 * i],  obj->i_texcoords[3 * i], obj->i_normals[3 * i],
					obj->tris[3 * i + 1], obj->i_texcoords[3 * i + 1], obj->i_normals[3 * i + 1],
					obj->tris[3 * i + 2], obj->i_texcoords[3 * i + 2], obj->i_normals[3 * i + 2]
				  );
	};
#endif



	if ( !obj ) return NULL;

	#pragma region NORMALEN_BERECHNEN
	if ( !clusternormals )  																	// Normalen wurden aus dem File geladen
	{
		// calculate surface normals (one normal per surface)
		// let's start with surface normals
		obj->f_normals  = ( float* )malloc( 3 * ntris * sizeof( float ) );
		for ( int i = 0; i < ntris; i++ )
		{
			// calculate edges per triangle

			int *act_tri = &( obj->tris[3 * i] );
			float *p0 = &( obj->points[3 * act_tri[0]] );
			float *p1 = &( obj->points[3 * act_tri[1]] );
			float *p2 = &( obj->points[3 * act_tri[2]] );

			float v1[3];
			v1[0] = p1[0] - p0[0];
			v1[1] = p1[1] - p0[1];
			v1[2] = p1[2] - p0[2];

			float v2[3];
			v2[0] = p2[0] - p0[0];
			v2[1] = p2[1] - p0[1];
			v2[2] = p2[2] - p0[2];

			float *act_n = &( obj->f_normals[3 * i] );
			cross_product( act_n, v1, v2 );
			normieren( act_n );
		}

		// now let's continue with interpolated normals per vertex
		obj->p_normals = ( float* )malloc( 3 * npoints * sizeof( float ) );
		memset( obj->p_normals, 0, 3 * npoints * sizeof( float ) );
		for ( int i = 0; i < npoints; i++ )
		{
			// for each point accumulate the face normals of faces the act. point is member of
			float *act_n = &( obj->p_normals[3 * i] );
			for ( int j = 0; j < ntris; j++ )
			{
				//loop over tris and check if p is member of act triangle
				int *act_tri = &( obj->tris[3 * j] );
				if ( ( act_tri[0] == i ) || ( act_tri[1] == i ) || ( act_tri[2] == i ) )
				{
					// add face normal
					float *act_fn = &( obj->f_normals[3 * j] );
					act_n[0] += act_fn[0];
					act_n[1] += act_fn[1];
					act_n[2] += act_fn[2];
				}
			}
			normieren( act_n );
		}
	}
	#pragma endregion NORMALEN_BERECHNEN

	#pragma region VBO_Erzeugen
	//////////////////////////////////////////////////
	// Objekt geladen: Erzeugen und Laden nun die VBOs
	//////////////////////////////////////////////////

	if ( use_vbos )
	{
		glGenBuffers( 4, obj->vbos );
		if ( obj->vbos[0] )      // kein Fehler beim VBO-Generieren
		{

			glEnableClientState( GL_VERTEX_ARRAY );
			//glEnableClientState(GL_COLOR_ARRAY);

			glBindBuffer( GL_ARRAY_BUFFER, obj->vbos[0] );		// Points (Geometry)
			glBufferData( GL_ARRAY_BUFFER,
						  obj->numpoints * 3 * sizeof( GLfloat ),
						  obj->points,
						  GL_STREAM_DRAW );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, obj->vbos[1] ); // Tris (Indexes)
			glBufferData( GL_ELEMENT_ARRAY_BUFFER,
						  obj->numtris * 3 * sizeof( GLuint ),
						  obj->tris,
						  GL_STREAM_DRAW );

			glBindBuffer( GL_ARRAY_BUFFER, obj->vbos[2] );		// f_normals (Geometry)
			glBufferData( GL_ARRAY_BUFFER,
						  obj->numtris * 3 * sizeof( GLfloat ),
						  obj->f_normals,
						  GL_STATIC_DRAW );

			glBindBuffer( GL_ARRAY_BUFFER, obj->vbos[3] );		// p_normals (Geometry)
			glBufferData( GL_ARRAY_BUFFER,
						  obj->numpoints * 3 * sizeof( GLfloat ),
						  obj->p_normals,
						  GL_STATIC_DRAW );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );					// unbind Arrays
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			//glDisableClientState(GL_COLOR_ARRAY);
			glDisableClientState( GL_VERTEX_ARRAY );

			obj->vbo_geladen = 1;
		}
		else exit( 1 );
	}

	#pragma endregion VBO_Erzeugen

	set_obj_material ( obj, red, green, blue, 1.0f, 0.0f, 128.0f, 0.0f );
	set_obj_pos( obj, x, y, z );
	set_obj_normalmode( obj, 2 );

	return obj;
}

void freeobject ( object3D *obj )
{
	if ( obj->points ) free( obj->points );
	if ( obj->tris ) free( obj->tris );
	if ( obj->f_normals ) free( obj->f_normals );
	free( obj );
}

//
// Zeichnet ein Wavefront-Object *obj
//


void drawobject ( object3D *obj )
{
	int *p = NULL;
	float *n = NULL;

	if ( obj )
	{

		glPushMatrix();

		glTranslatef ( obj->pos[0], obj->pos[1], obj->pos[2] );

		glColor4fv( obj->diff );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_AMBIENT, obj->amb );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_DIFFUSE, obj->diff );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_SPECULAR, obj->spec );
		glMaterialfv ( GL_FRONT_AND_BACK, GL_EMISSION, obj->emis );
		glMaterialf ( GL_FRONT_AND_BACK, GL_SHININESS, obj->shine );

		if ( !obj->vbo_geladen )  // haben keine VBOs
		{

			if ( obj->clusternormals ) obj->normal_mode = 2;			// immer die Clusternormalen verwenden, falls welche aus dem File geladen wurden

			glBegin ( GL_TRIANGLES );
			for ( int i = 0; i < obj->numtris; ++i )
			{
				switch ( obj->normal_mode )
				{
					case 0:
						//set no normals
						p = &obj->tris[3 * i];
						glVertex3fv ( &obj->points[3 * p[0]] );
						glVertex3fv ( &obj->points[3 * p[1]] );
						glVertex3fv ( &obj->points[3 * p[2]] );
						break;
					case 1:	//set surface normal
						n = &( obj->f_normals[3 * i] );
						glNormal3fv( n );
						p = &obj->tris[3 * i];
						glVertex3fv ( &obj->points[3 * p[0]] );
						glVertex3fv ( &obj->points[3 * p[1]] );
						glVertex3fv ( &obj->points[3 * p[2]] );
						break;
					case 2:
						if ( ( obj->clusternormals ) && ( obj->clustertexcoords ) )  		// Normalen und Texturkoordinaten
						{
							p = &obj->tris[3 * i];
							int *in = &obj->i_normals[3 * i];
							int *it = &obj->i_texcoords[3 * i];

							glNormal3fv( &obj->normals[3 * in[0]] );
							glTexCoord2fv( &obj->texcoords[2 * it[0]] );
							glVertex3fv ( &obj->points[3 * p[0]] );

							glNormal3fv( &obj->normals[3 * in[1]] );
							glTexCoord2fv( &obj->texcoords[2 * it[1]] );
							glVertex3fv ( &obj->points[3 * p[1]] );

							glNormal3fv( &obj->normals[3 * in[2]] );
							glTexCoord2fv( &obj->texcoords[2 * it[2]] );
							glVertex3fv ( &obj->points[3 * p[2]] );
						}
						else if ( obj->clusternormals )  								// Normalen ohne Texturen
						{
							p = &obj->tris[3 * i];
							int *in = &obj->i_normals[3 * i];
							glNormal3fv( &obj->normals[3 * in[0]] );
							glVertex3fv ( &obj->points[3 * p[0]] );
							glNormal3fv( &obj->normals[3 * in[1]] );
							glVertex3fv ( &obj->points[3 * p[1]] );
							glNormal3fv( &obj->normals[3 * in[2]] );
							glVertex3fv ( &obj->points[3 * p[2]] );
						}
						else  														// ohne Normalen und Texturkoordinaten
						{
							p = &obj->tris[3 * i];
							// set vertex normals
							glNormal3fv ( &obj->p_normals[3 * p[0]] );
							glVertex3fv ( &obj->points[3 * p[0]] );
							glNormal3fv ( &obj->p_normals[3 * p[1]] );
							glVertex3fv ( &obj->points[3 * p[1]] );
							glNormal3fv ( &obj->p_normals[3 * p[2]] );
							glVertex3fv ( &obj->points[3 * p[2]] );
						}

						break;
				}
			}
			glEnd();
		}
		else  															// wir haben VBOs geladen
		{
			glEnableClientState( GL_VERTEX_ARRAY );

			if ( obj->normal_mode == 2 )
			{
				glEnableClientState( GL_NORMAL_ARRAY );
				glBindBuffer( GL_ARRAY_BUFFER, obj->vbos[3] );
				glNormalPointer( GL_FLOAT, 0, 0 );
			}															// Normalen per Vertex

			glBindBuffer( GL_ARRAY_BUFFER, obj->vbos[0] );				// Points
			glVertexPointer( 3, GL_FLOAT, 0, 0 );

			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, obj->vbos[1] );		// Indexes
			glDrawElements( GL_TRIANGLES, 3 * obj->numtris, GL_UNSIGNED_INT, 0 );

			glBindBuffer( GL_ARRAY_BUFFER, 0 );
			glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

			if ( obj->normal_mode == 2 )  								// Normalen per Vertex
			{
				glDisableClientState( GL_NORMAL_ARRAY );
			}

			glDisableClientState( GL_VERTEX_ARRAY );

		}
		glPopMatrix();
	}
}
