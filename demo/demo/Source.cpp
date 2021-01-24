#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include "Skybox.h"
#include "camera.h"
#include "tga.h"

#include <vector>


#define ESCAPE 27

GLint Xsize = 1000;
GLint Ysize = 800;
float i, theta;

Skybox* skybox;
Camera cam;

GLfloat xt = 0.0, yt = 0.0, zt = 0.0; //xw = 0.0;
GLfloat xs = 1.0, ys = 1.0, zs = 1.0;
GLfloat xangle = 0, yangle = 0, zangle = 0.0, angle = 0.0;

GLfloat r = 0, g = 0, b = 0;
bool dayNight = 1;
bool fog = 0; //to switch fog effect
GLfloat moonHorizontal = 0, moonVertical = 0, snowmanMove = 0;
bool snowmanMovement = true;
bool movement = true, moon = false, goDown = false, sLeft = true, sRight = false;
GLUquadricObj* t;

static void SpecialKeyFunc(int Key, int x, int y);

/*
#pragma pack(1) // pack it so that I can have shorts & chars in sequence and read directly from file memory
struct TGAHeader {
	char    id_length;
	char    map_type;
	char    type;
	short   map_start;
	short   map_length;
	char    map_depth;
	short   x_origin;
	short   y_origin;
	short   width;
	short   height;
	char    bpp;
	char    descriptor_bits;
};
#pragma pack()

class TGA
{
private:
	// the handle for the texture in opengl
	GLuint textureHandle;
public:
	// Constructs and loads a TGA into opengl from the given image file path
	TGA(const char* imagePath) {
		FILE* file = NULL; // the file handle
		TGAHeader header; // struct for the header info
		char* pixels, * buffer;

		// column counter, row counter, i & j loop counters, and bytes per pixel
		int c, r, i, bytespp = 4;
		char n, packet_header;
		char pixel[4];

		// open the file
		file = fopen(imagePath, "rb");

		// read the header
		fread(&header, 18, 1, file);

		bytespp = header.bpp / 8; // bytes per pixel

		pixels = (char*)malloc(bytespp * header.width * header.height);

		// header type 2 is uncompressed RGB data without a color map / pallette
		if (header.type == 2)
		{
			// seek to the start of the data
			fseek(file, header.map_start + header.map_length * bytespp + 18, SEEK_SET);

			// read the pixel data into a buffer
			buffer = (char*)malloc(bytespp * header.width * header.height);
			fread(buffer, bytespp, header.width * header.height, file);

			// plot the pixel data into pixels buffer
			for (c = 0; c < header.width; c++) // count up columns
			{
				for (r = 0; r < header.height; r++) // rows
				{
					if (bytespp == 4)
					{
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 0] =
							buffer[(c + r * header.width) * bytespp + 2];
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 1] =
							buffer[(c + r * header.width) * bytespp + 1];
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 2] =
							buffer[(c + r * header.width) * bytespp + 0];
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 3] =
							buffer[(c + r * header.width) * bytespp + 3];
					}
					else if (bytespp == 3)
					{
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 0] =
							buffer[(c + r * header.width) * bytespp + 2];
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 1] =
							buffer[(c + r * header.width) * bytespp + 1];
						pixels[(c + (header.height - r - 1) * header.width) * bytespp + 2] =
							buffer[(c + r * header.width) * bytespp + 0];
					}
				}
			}

			free(buffer);

		}
		else if (header.type == 10) // run length encoded, non color mapped rgb
		{
			// find the start of the data
			fseek(file, header.map_start + header.map_length * bytespp + 18, SEEK_SET);

			c = 0; r = header.height - 1; // start at the top left
			// work through the bitmap
			while (r >= 0)
			{
				// read in the packet header
				fread(&packet_header, sizeof(packet_header), 1, file);

				// find the number of reps
				n = packet_header & 0x7F;

				*((int*)pixel) = 0;
				if (n != packet_header) // if bit = 1, the next pixel repeated N times
					fread(&pixel, bytespp, 1, file);

				// loop n times
				for (i = 0; i < n + 1; i++)
				{
					if (n == packet_header) // if bit = 0, N individual pixels
						fread(&pixel, bytespp, 1, file);

					if (bytespp == 4)
					{
						pixels[(c + r * header.width) * bytespp + 0] =
							pixel[2];
						pixels[(c + r * header.width) * bytespp + 1] =
							pixel[1];
						pixels[(c + r * header.width) * bytespp + 2] =
							pixel[0];
						pixels[(c + r * header.width) * bytespp + 3] =
							pixel[3];
					}
					else if (bytespp == 3)
					{
						pixels[(c + r * header.width) * bytespp + 0] =
							pixel[2];
						pixels[(c + r * header.width) * bytespp + 1] =
							pixel[1];
						pixels[(c + r * header.width) * bytespp + 2] =
							pixel[0];
					}
					// move to the next pixel
					c += 1;
					if (c >= header.width)
					{
						c = 0;
						r -= 1;
					}
				}
			}
		}
		// close the file
		fclose(file);

		// make a gl texture
		glGenTextures(1, &textureHandle);

		glBindTexture(GL_TEXTURE_2D, textureHandle);

		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		gluBuild2DMipmaps(GL_TEXTURE_2D, 3, header.width, header.height,
			GL_RGB, GL_UNSIGNED_BYTE, pixels);

		// delete the pixel data
		free(pixels);
	}

	// Returns the handle to the texture created from the image, for binding to opengl
	GLuint getTextureHandle(void) {
		return textureHandle;
	}
};
*/

TGA* sun = new TGA("images/sun.jpg");




GLvoid InitGL(GLfloat Width, GLfloat Height)
{
             /* Add line width,   ditto */
	glClearColor(1, 1, 1, 1);
	glViewport(0, 0, Width, Height);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//glOrtho(-4, 3.0, -4, 2.2, -50.0, 50.0);
    gluPerspective(45.0, Width / Height, 0.1, 200.0);
//	sun = new TGA("images/sun.tga");


	t = gluNewQuadric();
	gluQuadricDrawStyle(t, GLU_FILL);


	GLfloat front_amb_diff[] = { .7, .5, .1, 1.0 }; //front side property
	GLfloat back_amb_diff[] = { .4, .7, .1, 1.0 }; //back side property
	GLfloat spe[] = { .2, .2, .2, 1.0 }; //property for front and back
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, front_amb_diff);
	glMaterialfv(GL_BACK, GL_AMBIENT_AND_DIFFUSE, back_amb_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spe);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_COLOR_MATERIAL); //to change the colors of 3d generated objects
	glEnable(GL_NORMALIZE); //for scaling down objects
}


/*
void display_string(int x, int y, char* string, int font)
{
	int len, i;
	glColor3f(0.8, 0.52, 1.0);
	glRasterPos2f(x, y);
	len = (int)strlen(string);
	for (i = 0; i < len; i++) {
		if (font == 1)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		if (font == 2)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);
		if (font == 3)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, string[i]);
		if (font == 4)
			glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, string[i]);
	}

}

void display1(void)
{

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	display_string(190, 540, "National Institute of Technology Karnataka, Surathkal", 1);
	display_string(225, 500, name3, 1);
	display_string(390 + 10, 470, "HELP", 2);
	display_string(10, 450, "MOUSE", 2);
	display_string(10, 410, "PRESS RIGHT BUTTON FOR MENU", 3);
	display_string(10, 370, "KEYBOARD", 2);
	display_string(10, 340, "X-Y-Z KEYS FOR CORRESPONDING ROTATION", 3);
	display_string(10, 280 + 30, "U-F FOR CAMERA VIEW SETTINGS", 3);
	display_string(10, 250 + 30, "USE LEFT ARROW(<-) AND RIGHT ARROW(->) TO MOVE CAR", 3);
	display_string(10, 220 + 30, "ESCAPE TO EXIT", 3);
	display_string(250, 150 + 30, "PRESS SPACE BAR TO ENTER", 2);
	glutPostRedisplay();
	glutSwapBuffers();

}
*/

//method to create a house
void house(void) {
	glRotated(-20, 0, 1, 0);
	//roof
	glPushMatrix();
	glColor3f(.388, .2, .0039);
	glScaled(.5, .5, .5);
	glRotated(45, 0, 1, 0);
	glutSolidOctahedron();
	glPopMatrix();
	//house
	glColor3f(.871, .612, .416);
	glTranslated(0, -.38, 0);
	glutSolidCube(.73);
	//windows
	glColor3f(0, 0, 0);
	glTranslated(-.2, .13, .32);
	glutSolidCube(.12);
	glTranslated(.4, 0, 0);
	glutSolidCube(.12);
	//door
	glTranslated(-.2, -.355, .046);
	glScaled(.5, 1.1, 0);
	glutSolidCube(.23);
}

//method to create a tree
void tree(void) {
	//trunk
	glPushMatrix();
	glColor3f(.388, .2, .0039);
	GLUquadric* qobj = gluNewQuadric(); //cylinder trunk
	glRotated(90, 1, 0, 0);
	gluCylinder(qobj, .05, .05, .4, 30, 30);
	glPopMatrix();
	//tree leaves
	glColor3f(0, .415, .0156);
	glTranslated(0, -.23, 0);
	glRotated(-90, 1, 0, 0);
	glutSolidCone(.3, .3, 40, 40);
	glTranslated(0, 0, .1);
	glutSolidCone(.25, .3, 40, 40);
	glTranslated(0, 0, .1);
	glutSolidCone(.2, .3, 40, 40);
}

//method to create moon/ sun
void moonOrSun(void) {
	GLfloat pos1[] = { 0, 0, 0, 1 }, //light position
		emission1[] = { 0, 0, 0, 1 },
		emission_default[] = { 0, 0, 0, 1 },
		amb1[] = { .4, .4, .4, 1.0 }; //ambient intensity

/*	GLfloat qaWhite[] = { 1.0,1.0,1.0,1.0 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, qaWhite);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, qaWhite);
	glMaterialfv(GL_FRONT, GL_SPECULAR, qaWhite);
	glMaterialf(GL_FRONT, GL_SHININESS, 60.0);

	// glNormal3f(1.0,0.0,1.0);
*/
	glTranslated(.05, 0, 0);
    glLightfv(GL_LIGHT0, GL_POSITION, pos1);
	glMaterialfv(GL_FRONT, GL_EMISSION, emission1);
//	glEnable(GL_TEXTURE_2D);
//	glBindTexture(GL_TEXTURE_2D, sun->getTextureHandle());


	
	GLUquadricObj* quadric = gluNewQuadric();
	gluQuadricTexture(quadric, true);
	gluQuadricNormals(quadric, GLU_SMOOTH);
	gluSphere(quadric, 0.4, 40, 40);

	glMaterialfv(GL_FRONT, GL_EMISSION, emission_default);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb1);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);



}

//method to create snowman
void snowman(void) {
	//body
	glPushMatrix();
	glColor3f(1, 1, 1);
	glTranslated(0, 1, 0);
	glutSolidSphere(.9, 100, 100);
	glTranslated(0.0, -2.25, 0);
	glutSolidSphere(1.5, 100, 100);
	glTranslated(0.0, -3, 0);
	glutSolidSphere(2, 100, 100);
	glPopMatrix();
	//face
	glPushMatrix();
	//eyes
	glTranslated(-.35, 1.5, .75);
	glColor3f(0, 0, 0);
	glutSolidSphere(.1, 100, 100);
	glTranslated(.65, 0, 0);
	glutSolidSphere(.1, 100, 100);
	glColor3f(.8, .3, 0);
	//nose
	glTranslated(-.325, -.3, .14);
	glutSolidCone(.15, 1, 100, 100);
	//mouth
	glColor3f(0, 0, 0);
	glTranslated(-.3, -.4, 0);
	glutSolidSphere(.07, 100, 100);
	glTranslated(.15, -.075, 0);
	glutSolidSphere(.07, 100, 100);
	glTranslated(.15, -.005, 0);
	glutSolidSphere(.07, 100, 100);
	glTranslated(.15, .005, 0);
	glutSolidSphere(.07, 100, 100);
	glTranslated(.15, .075, 0);
	glutSolidSphere(.07, 100, 100);
	glPopMatrix(); //pop face objects
	//arms
	glPushMatrix();
	glColor3f(.388, .2, .0039);
	GLUquadric* qobj = gluNewQuadric(); //create cylinder object
	glRotated(45, 0, 0, 1);
	glRotated(90, 0, 1, 0);
	glTranslated(-.2, -1.5, 0);
	gluCylinder(qobj, .1, .1, 1.5, 30, 30);
	glRotated(-90, 1, 0, 0);
	glTranslated(0, 1.5, 1.5);
	gluCylinder(qobj, .1, .1, 1.5, 30, 30);
	glPopMatrix();
}

//method to create car
GLvoid DrawCar() {
	glPushMatrix();

	glTranslatef(0, -2.0, 0);
	glTranslatef(xt, 0, 0);
	glBegin(GL_QUADS);            

/* top of cube*/
//************************FRONT BODY****************************************
	glColor3f(r, g, b);
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.2, 0.4, 0.2);

	/* bottom of cube*/
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(0.2, 0.2, 0.2);

	/* front of cube*/
	glVertex3f(0.2, 0.2, 0.6);
	glVertex3f(0.2, 0.4, 0.6);
	glVertex3f(0.2, 0.4, 0.2);
	glVertex3f(0.2, 0.2, 0.2);

	/* back of cube.*/
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.6, 0.2, 0.2);

	/* left of cube*/
	glVertex3f(0.2, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.2, 0.4, 0.6);

	/* Right of cube */
	glVertex3f(0.2, 0.2, 0.2);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.2, 0.4, 0.2);
	//****************************************************************************
	glVertex3f(0.7, 0.65, 0.6);
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(1.7, 0.65, 0.2);        //top cover
	glVertex3f(1.7, 0.65, 0.6);
	//***************************back guard******************************
	glColor3f(r, g, b);            /* Set The Color*/
	glVertex3f(1.8, 0.5, 0.6);
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(2.1, 0.4, 0.2);
	glVertex3f(2.1, 0.4, 0.6);

	/* bottom of cube*/
	glVertex3f(2.1, 0.2, 0.6);
	glVertex3f(2.1, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.2, 0.6);

	/* back of cube.*/
	glVertex3f(2.1, 0.4, 0.6);
	glVertex3f(2.1, 0.4, 0.2);
	glVertex3f(2.1, 0.2, 0.2);
	glVertex3f(2.1, 0.2, 0.6);

	/* left of cube*/
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(2.1, 0.4, 0.2);
	glVertex3f(2.1, 0.2, 0.2);

	/* Right of cube */
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.5, 0.6);
	glVertex3f(2.1, 0.4, 0.6);
	glVertex3f(2.1, 0.2, 0.6);
	//******************MIDDLE BODY************************************
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(1.8, 0.2, 0.6);
	glVertex3f(1.8, 0.5, 0.6);

	/* bottom of cube*/
	glVertex3f(0.6, 0.2, 0.6);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.6);

	/* back of cube.*/
	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.6, 0.2, 0.2);
	glVertex3f(1.8, 0.2, 0.2);
	glVertex3f(1.8, 0.5, 0.2);
	//*********************ENTER WINDOW**********************************
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.77, 0.63, 0.2);
	glVertex3f(0.75, 0.5, 0.2);        //quad front window
	glVertex3f(1.2, 0.5, 0.2);
	glVertex3f(1.22, 0.63, 0.2);

	glVertex3f(1.27, 0.63, .2);
	glVertex3f(1.25, 0.5, 0.2);        //quad back window
	glVertex3f(1.65, 0.5, 0.2);
	glVertex3f(1.67, 0.63, 0.2);

	glColor3f(r, g, b);
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(0.7, 0.5, .2);       //first separation
	glVertex3f(0.75, 0.5, 0.2);
	glVertex3f(0.77, 0.65, 0.2);

	glVertex3f(1.2, 0.65, 0.2);
	glVertex3f(1.2, 0.5, .2);       //second separation
	glVertex3f(1.25, 0.5, 0.2);
	glVertex3f(1.27, 0.65, 0.2);

	glVertex3f(1.65, 0.65, 0.2);
	glVertex3f(1.65, 0.5, .2);     //3d separation
	glVertex3f(1.7, 0.5, 0.2);
	glVertex3f(1.7, 0.65, 0.2);

	glVertex3f(0.75, 0.65, 0.2);
	glVertex3f(0.75, 0.63, 0.2);        //line strip
	glVertex3f(1.7, 0.63, 0.2);
	glVertex3f(1.7, 0.65, 0.2);

	glVertex3f(0.75, 0.65, 0.6);
	glVertex3f(0.75, 0.63, 0.6);        //line strip
	glVertex3f(1.7, 0.63, 0.6);
	glVertex3f(1.7, 0.65, 0.6);

	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.77, 0.63, 0.6);
	glVertex3f(0.75, 0.5, 0.6);        //quad front window
	glVertex3f(1.2, 0.5, 0.6);
	glVertex3f(1.22, 0.63, 0.6);

	glVertex3f(1.27, 0.63, .6);
	glVertex3f(1.25, 0.5, 0.6);        //quad back window
	glVertex3f(1.65, 0.5, 0.6);
	glVertex3f(1.67, 0.63, 0.6);

	glColor3f(r, g, b);
	glVertex3f(0.7, 0.65, 0.6);
	glVertex3f(0.7, 0.5, .6);       //first separation
	glVertex3f(0.75, 0.5, 0.6);
	glVertex3f(0.77, 0.65, 0.6);

	glVertex3f(1.2, 0.65, 0.6);
	glVertex3f(1.2, 0.5, .6);       //second separation
	glVertex3f(1.25, 0.5, 0.6);
	glVertex3f(1.27, 0.65, 0.6);

	glVertex3f(1.65, 0.65, 0.6);
	glVertex3f(1.65, 0.5, .6);
	glVertex3f(1.7, 0.5, 0.6);
	glVertex3f(1.7, 0.65, 0.6);
	glEnd();


	//**************************************************************
	glBegin(GL_QUADS);
	/* top of cube*/
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.6, 0.5, 0.2);        //quad front window
	glVertex3f(0.7, 0.65, 0.2);
	glVertex3f(0.7, 0.65, 0.6);

	glVertex3f(1.7, 0.65, .6);
	glVertex3f(1.7, 0.65, 0.2);        //quad back window
	glVertex3f(1.8, 0.5, 0.2);
	glVertex3f(1.8, 0.5, 0.6);

	glEnd();

	//
	glBegin(GL_TRIANGLES);                /* start drawing the cube

  /* top of cube*/
	glColor3f(0.3, 0.3, 0.3);
	glVertex3f(0.6, 0.5, 0.6);
	glVertex3f(0.7, 0.65, 0.6);       //tri front window
	glVertex3f(0.7, 0.5, 0.6);

	glVertex3f(0.6, 0.5, 0.2);
	glVertex3f(0.7, 0.65, 0.2);       //tri front window
	glVertex3f(0.7, 0.5, 0.2);

	glVertex3f(1.7, 0.65, 0.2);
	glVertex3f(1.8, 0.5, 0.2);       //tri back window
	glVertex3f(1.7, 0.5, 0.2);

	glVertex3f(1.7, 0.65, 0.6);
	glVertex3f(1.8, 0.5, 0.6);       //tri back window
	glVertex3f(1.7, 0.5, 0.6);


	glEnd();
	//************IGNITION SYSTEM
	glPushMatrix();
	glColor3f(0.3, 0.3, 0.7);
	glTranslatef(1.65, 0.2, 0.3);
	glRotatef(90.0, 0, 1, 0);
	gluCylinder(t, 0.02, 0.03, .5, 10, 10);
	glPopMatrix();
	//********************WHEEL

	glColor3f(0.7, 0.7, 0.7);
	glPushMatrix();
	glBegin(GL_LINE_STRIP);
	for (theta = 0; theta < 360; theta = theta + 40)
	{
		glVertex3f(0.6, 0.2, 0.62);
		glVertex3f(0.6 + (0.08 * (cos(((theta + angle) * 3.14) / 180))), 0.2 + (0.08 * (sin(((theta + angle) * 3.14) / 180))), 0.62);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (theta = 0; theta < 360; theta = theta + 40)
	{
		glVertex3f(0.6, 0.2, 0.18);
		glVertex3f(0.6 + (0.08 * (cos(((theta + angle) * 3.14) / 180))), 0.2 + (0.08 * (sin(((theta + angle) * 3.14) / 180))), 0.18);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (theta = 0; theta < 360; theta = theta + 40)
	{
		glVertex3f(1.7, 0.2, 0.18);
		glVertex3f(1.7 + (0.08 * (cos(((theta + angle) * 3.14) / 180))), 0.2 + (0.08 * (sin(((theta + angle) * 3.14) / 180))), 0.18);
	}
	glEnd();

	glBegin(GL_LINE_STRIP);
	for (theta = 0; theta < 360; theta = theta + 40)
	{
		glVertex3f(1.7, 0.2, 0.62);
		glVertex3f(1.7 + (0.08 * (cos(((theta + angle) * 3.14) / 180))), 0.2 + (0.08 * (sin(((theta + angle) * 3.14) / 180))), 0.62);
	}
	glEnd();
	glTranslatef(0.6, 0.2, 0.6);
	glColor3f(0, 0, 0);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(0, 0, -0.4);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(1.1, 0, 0);
	glutSolidTorus(0.025, 0.07, 10, 25);

	glTranslatef(0, 0, 0.4);
	glutSolidTorus(0.025, 0.07, 10, 25);
	glPopMatrix();
	glPopMatrix();
}

//method to create road
GLvoid DrawRoad() {
	//*****************************road and surrounding development***********************************
	glPushMatrix();
	glTranslatef(0, -2.0, 0);

	glBegin(GL_QUADS);

	glPushMatrix();
//	glTranslatef(xw, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(-100, 0.1, -100);
	glVertex3f(-100, 0.1, 0);         //a green surroundings
	glVertex3f(100, 0.1, 0);
	glVertex3f(100, 0.1, -100);

	glColor3f(0.7, 0.7, 0.7);
	glVertex3f(-100, 0.1, 0);
	glVertex3f(-100, 0.1, 0.45);         //a long road
	glVertex3f(100, 0.1, 0.45);
	glVertex3f(100, 0.1, 0);

	glColor3f(1.0, 0.75, 0.0);
	glVertex3f(-100, 0.1, 0.45);       //a median
	glVertex3f(-100, 0.1, 0.55);
	glVertex3f(100, 0.1, 0.55);
	glVertex3f(100, 0.1, 0.45);

	glColor3f(0.7, 0.7, 0.7);
	glVertex3f(-100, 0.1, 0.55);
	glVertex3f(-100, 0.1, 1);         //a long road
	glVertex3f(100, 0.1, 1);
	glVertex3f(100, 0.1, 0.55);

	glColor3f(0, 1, 0);
	glVertex3f(-100, 0.1, 1);
	glVertex3f(-100, 0.1, 100);         //a green surroundings
	glVertex3f(100, 0.1, 100);
	glVertex3f(100, 0.1, 1);
	glPopMatrix();

	glEnd();

//	if (1)
//	{
	    //wheel flag creation
		glPushMatrix();
//		glTranslatef(xw, 0, 0);
		glColor3f(0.5, .2, 0.3);
		glBegin(GL_QUADS);
		for (i = 0; i < 200; i += 0.2)
		{
			glVertex3f(-100 + i, 0, 1);
			glVertex3f(-99.9 + i, 0, 1);
			glVertex3f(-99.9 + i, 0.2, 1);
			glVertex3f(-100 + i, 0.2, 1);
			i += 0.5;
		}
		for (i = 0; i < 200; i += 0.2)
		{
			glVertex3f(-100 + i, 0, 0);
			glVertex3f(-99.9 + i, 0, 0);
			glVertex3f(-99.9 + i, 0.2, 0);
			glVertex3f(-100 + i, 0.2, 0);
			i += 0.5;
		}
		glEnd();
		glPopMatrix();
//	}

	glPopMatrix();

}

//method to create overall scene
GLvoid DrawGLScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	//glTranslatef(cam.eye.x, cam.eye.y, cam.eye.z);
	skybox->draw();
	glPopMatrix();

	if (dayNight == 1)/* Initialize our window. */
		glClearColor(1, 1, 1, 1);
	else
		glClearColor(0.1, 0.1, 0.1, 0);
	glPushMatrix();
	glTranslatef(-1.0, 0.0, -3.5);
	glRotatef(xangle, 1.0, 0.0, 0.0);
	glRotatef(yangle, 0.0, 1.0, 0.0);
	glRotatef(zangle, 0.0, 0.0, 1.0);
	glTranslatef(0, yt, 0);
	glScalef(xs, ys, zs);
	glEnable(GL_COLOR_MATERIAL);

	if (fog)
	{
		glClearColor(0.1, 0.1, 0.1, 0);

		GLfloat fogcolour[4] = { 0,0,0,1.0 };

		glFogfv(GL_FOG_COLOR, fogcolour);
		glFogf(GL_FOG_DENSITY, 1);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glFogf(GL_FOG_START, 3.0);
		glFogf(GL_FOG_END, 100.0);
		glHint(GL_FOG_HINT, GL_NICEST);
		glEnable(GL_FOG);
	}
	if (!fog)
	{
		glClearColor(1, 1, 1, 0);
		glDisable(GL_FOG);
	}



	DrawCar();
	DrawRoad();



	//snowmen creation
	glPushMatrix();
	glTranslated(-1.5, -1, -.7);
	glScaled(.1, .1, .1);
	glRotated(45, 0, 1, 0);
	//idle animation
	if (snowmanMovement) {
		glRotated(snowmanMove, 0, 0, 1);
	}
	snowman();
	glPopMatrix();

	glPushMatrix();
	glTranslated(0, -1, -.5);
	glScaled(.1, .1, .1);
	if (snowmanMovement) {
		glRotated(-snowmanMove, 0, 0, 1);
	}
	snowman();
	glPopMatrix();

	//tree creation
	glPushMatrix();
	glScaled(2.5, 4, 2.5);
	glTranslated(.85, 0, -.3);
	tree();
	glPopMatrix();

	glPushMatrix();
	glScaled(3, 4, 3);
	glTranslated(-0.4, 0, -1);
	tree();
	glPopMatrix();

	//house creation
	glPushMatrix();
	glTranslated(.45, -.3, -1.5);
	glScaled(1.9, 1.5, 1.4);
	house();
	glPopMatrix();

	//moon creation
	glPushMatrix();
	//idle animation for moon
	if (moon == true) {
		glColor3f(.696, .696, .670);
	}
	else {
		glColor3f(0.8, 0.5, 0.2);
	}
	glTranslated(-2.7, 1.5, -.5);
	if (movement) {
		glTranslated(moonHorizontal, moonVertical, 0);
	}
	moonOrSun();
	glPopMatrix();

	//*************************************************************
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	glutPostRedisplay();
	glutSwapBuffers();
	//	}
}

//method for animation
void idle(void) {
	//movement of moon/sun
	moonHorizontal = moonHorizontal + .002;
	if (goDown == false) moonVertical = moonVertical + .0004;
	if (moonHorizontal > 5.5) {
		moonHorizontal = 0;
		moonVertical = 0;
		goDown = false;
	}
	if (moonVertical > .45 || goDown == true) {
		goDown = true;
		moonVertical = moonVertical - .0004;
	}

	//movement for snowmen
	if (sLeft) {
		snowmanMove = snowmanMove + 1.5;
		if (snowmanMove >= 20) {
			sLeft = false;
			sRight = true;
		}
	}
	if (sRight) {
		snowmanMove = snowmanMove - 1.5;
		if (snowmanMove <= -20) {
			sLeft = true;
			sRight = false;
		}
	}
	glutPostRedisplay();
}

//keyboard function
void NormalKey(GLubyte key, GLint x, GLint y)
{
	switch (key) {
	case 'x': xangle += 5.0;
		glutPostRedisplay();
		break;

	case 'X':xangle -= 5.0;
		glutPostRedisplay();
		break;

	case 'y':
		yangle += 5.0;
		glutPostRedisplay();
		break;

	case 'Y':
		yangle -= 5.0;
		glutPostRedisplay();
		break;

	case 'z':
		zangle += 5.0;
		glutPostRedisplay();
		break;

	case 'Z':
		zangle -= 5.0;
		glutPostRedisplay();
		break;

	case 'u':                          /* Move up */
		yt += 0.2;
		glutPostRedisplay();
		break;

	case 'U':
		yt -= 0.2;                      /* Move down */
		glutPostRedisplay();
		break;

	case '1':
		glPolygonMode(GL_FRONT, GL_LINE);
		glutPostRedisplay();
		break;
		
	case '2':
		glPolygonMode(GL_FRONT, GL_FILL);
		glutPostRedisplay();
		break;

	case 'd': 
		cam.slide(0.1, 0, 0);
		glutPostRedisplay();
		break;

	case 'a':
		cam.slide(-0.1, 0, 0);
		glutPostRedisplay();
		break;

	case 's':
		cam.slide(0, 0, 0.5);
		glutPostRedisplay();
		break;

	case 'w':
		cam.slide(0, 0, -0.5);
		glutPostRedisplay();
		break;

	case 'i':
		cam.pitch(-0.1);
		glutPostRedisplay();
		break;

	case 'k':
		cam.pitch(0.1);
		glutPostRedisplay();
		break;

	case 'q':
		cam.yaw(-0.1);
		glutPostRedisplay();
		break;

	case 'e':
		cam.yaw(0.1);
		glutPostRedisplay();
		break;

	case 'j':
		cam.roll(-0.1);
		glutPostRedisplay();
		break;

	case 'l':
		cam.roll(0.1);
		glutPostRedisplay();
		break;

	default:
		break;
	}

}

//car move keyboard function
static void SpecialKeyFunc(int Key, int x, int y)
{
	switch (Key) {
	case GLUT_KEY_RIGHT:
		xt += 0.02;
		angle += 5;

		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		xt -= 0.02;
		angle += 5;

		glutPostRedisplay();
		break;
	}
}

//menu
void myMenu(int id)
{

	if (id == 1)
	{
		dayNight = 1;
		moon = false;
		fog = 0;
		glClearColor(1, 1, 1, 1);
		glDisable(GL_FOG);
		glutPostRedisplay();
	}

	if (id == 2)
	{
		dayNight = 0;
		moon = true;
		fog = 1;
		glClearColor(0.1, 0.1, 0.1, 0);
		GLfloat fogcolour[4] = { 0.0,0.0,0.0,1.0 };

		glFogfv(GL_FOG_COLOR, fogcolour);
		glFogf(GL_FOG_DENSITY, 1);
		glFogi(GL_FOG_MODE, GL_LINEAR);
		glHint(GL_FOG_HINT, GL_NICEST);
		glEnable(GL_FOG);

		glutPostRedisplay();
	}
}

//submenu of change of car color
void colorMenu(int id)
{
	if (id == 3)
	{
		r = g = 0;
		b = 1;
		glutPostRedisplay();

	}
	if (id == 4)
	{
		r = 0.8;
		b = g = 0;
		glutPostRedisplay();
	}
	if (id == 5)
	{
		g = 1;
		r = b = 0;
		glutPostRedisplay();
	}
	if (id == 6)
	{
		r = b = g = 0;
		glutPostRedisplay();
	}
	if (id == 7)
	{
		b = 0;
		r = g = 1;
		glutPostRedisplay();
	}
	if (id == 8)
	{
		b = r = g = .7;
		glutPostRedisplay();
	}

}

void dispose() {
	delete skybox;
}

int main(int argc, char** argv)
{


	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(Xsize, Ysize);
	glutInitWindowPosition(50, 50);
	glutCreateWindow("A highway knifes through the jungle");
	glClearColor(0.03, 0.02, 0.7, 0.0);
	glEnable(GL_DEPTH_TEST);
	//init();
	InitGL(Xsize, Ysize);
	glutDisplayFunc(DrawGLScene);
	glutIdleFunc(idle);
	glutKeyboardFunc(NormalKey);
	glutSpecialFunc(SpecialKeyFunc);
	int submenu = glutCreateMenu(colorMenu);
	glutAddMenuEntry("blue", 3);
	glutAddMenuEntry("red", 4);
	glutAddMenuEntry("green", 5);
	glutAddMenuEntry("black", 6);
	glutAddMenuEntry("yellow", 7);
	glutAddMenuEntry("grey", 8);
	glutCreateMenu(myMenu);
	glutAddSubMenu("car colors", submenu);
	glutAddMenuEntry("daymode", 1);
	glutAddMenuEntry("Night mode", 2);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	glEnable(GL_DEPTH_TEST);
//	glEnable(GL_CULL_FACE);

	skybox = new Skybox();
	cam.set(4, 4, 4, 0, 0, 0, 0, 1, 0);
	cam.setShape(60.0f, 64.0f / 48.0f, 0.5f, 1000.0f);

	cam.slide(-5, -2, -5);
	cam.roll(-35);
	cam.yaw(40);
	cam.pitch(-40);
	atexit(dispose);

	glutMainLoop();
	return 1;
}