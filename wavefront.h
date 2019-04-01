// Wavefront Loader
// PrakCG
#ifndef WAVEFRONT_H
#define WAVEFRONT_H


#ifndef __EXT
#define __EXT

#include "Include/glext.h"

#ifndef EXPORT_EXT
#define EXTERN_EXT extern
#define EXTERN_NULL
#else
#define EXTERN_EXT
#define EXTERN_NULL =NULL
#endif // EXPORT_EXT

EXTERN_EXT PFNGLGENBUFFERSPROC glGenBuffers EXTERN_NULL;
EXTERN_EXT PFNGLBINDBUFFERPROC glBindBuffer EXTERN_NULL;
EXTERN_EXT PFNGLBUFFERDATAPROC glBufferData EXTERN_NULL;
EXTERN_EXT PFNGLDELETEBUFFERSPROC glDeleteBuffers EXTERN_NULL;

int init_extensions ();

struct object3D
{
	unsigned short int  normal_mode;    // 0: keine Normalen, 1: Normalen per Surface, 2: Normalen per Vertex
	int   numpoints;
	int   numtris;
	int	numnormals;

	bool clustertexcoords;
	bool clusternormals;

	float *points;					  // Liste der importierten Vertex Koordinaten
	float *texcoords;					  // Liste der importierten Texturkoordinaten (Wavefront)
	float *normals;					  // Liste der importierten Normalen (Wavefront)
	int   *tris;						  // surface indexes
	float *f_normals;					  // surface normals, berechnet
	float *p_normals;					  // vertex normals, berechnet aus f_normals
	int	*i_normals;					  // importierte Vertexnormalen
	int   *i_texcoords;				  // importierte Texturkoordinaten

	bool vbo_geladen;
	GLuint vbos[4];					  // IDs der VBOs 0-Points, 1-tris, 2-f_normals, 3-p_normals

	float color[4];					  // Material
	float amb[4];
	float diff[4];
	float spec[4];
	float shine;
	float emis[4];

	float pos[3];

	float borderPoints[4][3];

};


// Laden eines Wavefront-Objektes aus einer Datei,
// Liefert Zeiger auf das neue Objekt, NULL wenn Fehler
// Benötigter Speicher wird in dieser Fkt. allokiert
// rgb: Farbe
// x,y,z: Position
object3D* loadobject ( const char *filename, bool use_vbos, float red = 0.6f, float green = 0.6f, float blue = 0.6f, float x = 0.0f, float y = 0.0f, float z = 0.0f );

// Objekt Material + Farben setzen
void set_obj_material ( object3D *obj, float red, float green, float blue, float alpha, float spec, float shine, float emis );

// Objekt Position setzen
void set_obj_pos ( object3D *obj, float x, float y, float z );

// Objekt Normalenmodus setzen
void set_obj_normalmode ( object3D *obj, int normalmode = 2 );

// Freigeben des belegten Speichers
void freeobject ( object3D *obj );

// Zeichnen des Objektes
void drawobject ( object3D *obj );

bool read_cluster_vertex( char **r_line, int *p, int *t, int *n );
inline float* normieren ( float v[3] );
inline void cross_product( float *n, float *a, float *b );


#endif // __EXT

#endif // WAVEFRONT_H
