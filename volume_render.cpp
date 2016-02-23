/* -------------------------------------------------------- */
/* -------------------------------------------------------- */
/*

TP OpenGL - Image Synthesis course

George Drettakis, Adrien Bousseau

mailto: FirstName.LastName@inria.fr

REVES / INRIA


Documentation:
- OpenGL tutorial, provides detailed instructions but relies on a different representation of matrices and 3D models
http://www.opengl-tutorial.org/beginners-tutorials/
- GLSL reference
https://www.opengl.org/documentation/glsl/
  
*/

#ifdef _WIN32
#include <windows.h>        // windows API header (required by gl.h)
#endif

#include "glew/include/GL/glew.h"	// Support for shaders

#ifdef __APPLE__

#include <gl3.h>          // OpenGL header
#include <gl3ext.h>       // OpenGL header
#include <glut.h>         // OpenGL Utility Toolkit header

#else

#include <GL/gl.h>          // OpenGL header
#include <GL/glu.h>         // OpenGL Utilities header
#include <GL/glut.h>        // OpenGL Utility Toolkit header

#endif

#include <cstdio>
#include <cmath>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

#include "glsl.h" // GLSL Helper functions
#include "TextureLoader.h"

//path of the shaders
#define SRC_PATH	""

#include <iostream>
using namespace std;

/* --------------------- Main Window ------------------- */

int          g_MainWindow; // glut Window Id
int          g_W=512;      // window width
int          g_H=512;      // window width
float        angle = 0.f;
float		 iso = 0.5f;
int			 rendering = 1;

/* --------------------- Geometry ------------------- */

//Vertex Array Object
GLuint VertexArrayID;
// This will identify our vertex buffer
GLuint vertexbuffer;
// This will identify our texture coordinate buffer
GLuint uvbuffer;
// This will identify the textures (volume and normals)
GLuint textureVolumeID;
GLuint textureNormalsID;

/*---------------------- Shaders -------------------*/
GLuint g_glslProgram;

/*-------------------- User Control -----------------*/
// ...

/* -------------------------------------------------------- */

void mainKeyboard(unsigned char key, int x, int y) 
{
	if (key == 'q') {
		exit (0);
	} else if (key == ' ') {
		printf("spacebar pressed\n");
	} else if (key == '+') {
	    angle += 0.10f;
		printf("Rotation: %3.3f\n",angle);
	} else if (key == '-') {
	    angle -= 0.10f;
		printf("Rotation: %3.3f\n",angle);
	} else if (key == 't') {
		iso += 0.05f;
		printf("Isosurface: %3.3f\n",iso);
	} else if (key == 'r') {
		iso -= 0.05f;
		printf("Isosurface: %3.3f\n",iso);
	} else if (key == '1') {
		rendering = 1;
		printf("Rendering: Phong\n");
	} else if (key == '2') {
		rendering = 2;
		printf("Rendering: Normal map\n");
	} else if (key == '3') {
		rendering = 3;
		printf("Rendering: Accumulative\n");
	} else if (key == '4') {
		rendering = 4;
		printf("Rendering: Isosurface\n");
	} else {
		printf("key '%c' pressed\n",key);
	}
}

void print_usage()
{
	printf("[q] \t - Quit\n");
	printf("[+] \t - Turn right\n");
	printf("[-] \t - Turn left\n");
	printf("[t] \t - Increase isosurface value\n");
	printf("[r] \t - Decrease isosurface value\n");
	printf("Rendering: \n");
	printf("\t [1] \t - Phong\n");
	printf("\t [2] \t - Normal map\n");
	printf("\t [3] \t - Accumulative\n");
	printf("\t [4] \t - Isosurface\n");
}

/* -------------------------------------------------------- */

void mainMouse(int btn, int state, int x, int y) 
{
	if (state == GLUT_DOWN) {
		if (btn == GLUT_LEFT_BUTTON) {
			printf("Left mouse button pressed at coordinates %d,%d\n",x,y);
		} else if (btn == GLUT_RIGHT_BUTTON) {
			printf("Right mouse button pressed at coordinates %d,%d\n",x,y);
		}
	} else if (state == GLUT_UP) {
		if        (btn == GLUT_LEFT_BUTTON) {
			printf("Left mouse button release at coordinates %d,%d\n",x,y);
		} else if (btn == GLUT_RIGHT_BUTTON) {
			printf("Right mouse button release at coordinates %d,%d\n",x,y);
		}
	}
}

/* -------------------------------------------------------- */

void mainMotion(int x,int y)
{
	printf("Mouse is at %d,%d\n",x,y);
}

/* -------------------------------------------------------- */

void mainReshape(int w,int h)
{
	printf("Resizing window to %d,%d\n",w,h);

	g_W=w;
	g_H=h;
	// set viewport to the entire window
	glViewport(0,0,g_W,g_H);
	
}

/* -------------------------------------------------------- */

void mainRender()
{
	// Rendering
	GLuint renderingID  = glGetUniformLocation(g_glslProgram, "rendering");
	glUniform1i(renderingID,rendering);

	// Dark background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	
	// clear screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT);
	
	// use our shader
	glUseProgramObjectARB(g_glslProgram);
	
	
	//--- Camera ---//
	// No camera, just draw a quad over the entire screen
	
	
	//--- Texturing: cover the quad with the image
	// Get a handle for our "myTextureSamplerVolume" uniform
	GLuint TexVolumeID  = glGetUniformLocation(g_glslProgram, "myTextureSamplerVolume");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureVolumeID);
	// Set our "myTextureSamplerVolume" sampler to user Texture Unit 0
	glUniform1i(TexVolumeID, 0); 
	
	// Same for the normals, in texture unit 1
	// Get a handle for our "myTextureSamplerVolume" uniform
	GLuint TexNormalsID  = glGetUniformLocation(g_glslProgram, "myTextureSamplerNormals");
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureNormalsID);
	// Set our "myTextureSamplerVolume" sampler to user Texture Unit 1
	glUniform1i(TexNormalsID, 1); 
	
	// Send the user-controled variables
	
	//Rotation matrix
    GLfloat* rot_data = new GLfloat[9];
    rot_data[0] = cos(angle);
	rot_data[1] = sin(angle);
	rot_data[2] = 0.0f;
	rot_data[3] = -sin(angle);
	rot_data[4] = cos(angle);
	rot_data[5] = 0.0f;
	rot_data[6] = 0.0f;
	rot_data[7] = 0.0f;
	rot_data[8] = 1.0f;
	GLuint AngleID  = glGetUniformLocation(g_glslProgram, "rot_mat");
    glUniformMatrix3fv(	AngleID, 1, GL_FALSE, rot_data);

	// Isosurface threshold value
	GLuint IsoID  = glGetUniformLocation(g_glslProgram, "iso");
	glUniform1f(IsoID,iso);
	
	//--- Geometry ---//
	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glVertexAttribPointer(
	  0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	  2,                  // size, 2 coordinates per vertex (x,y)
	  GL_FLOAT,           // type
	  GL_FALSE,           // normalized?
	  0,                  // stride
	  (void*)0            // array buffer offset
	);
	
	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glVertexAttribPointer(
	    1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
	    2,                                // size, 2 texture coordinates (u,v)
	    GL_FLOAT,                         // type
	    GL_FALSE,                         // normalized? No
	    0,                                // stride
	    (void*)0                          // array buffer offset
	);
	
	// Draw the quad (2 triangles)
	glDrawArrays(GL_TRIANGLES, 0, 6); // Starting from vertex 0; 3 vertices total -> 2 triangle.
 
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	
	
	// swap - this call exchanges the back and front buffer
	// swap is synchronized on the screen vertical sync
	glutSwapBuffers();
}

/* -------------------------------------------------------- */

void idle( void )
{
	// whenever the application has free time, ask for a screen refresh
	glutPostRedisplay();
}

/* -------------------------------------------------------- */

void loadShaders()
{
	const char *fp_code=loadStringFromFile(SRC_PATH "SimpleFragmentShader.fp");
	const char *vp_code=loadStringFromFile(SRC_PATH "SimpleVertexShader.vp");
	g_glslProgram = createGLSLProgram(vp_code,fp_code);
	delete [](fp_code);
	delete [](vp_code);
}

/* -------------------------------------------------------- */

void createGeometry()
{
	// Create a quad made of two triangles. A triangle contains 3 vertices, each vertex has 2 coordinates (screen space)
	// Position
	GLfloat* g_vertex_buffer_data;
	g_vertex_buffer_data = new GLfloat[6*2];
	//bottom right triangle. Note: the OpenGL screen covers [-1;1] x [-1;1]
	g_vertex_buffer_data[0] = -1.0f; g_vertex_buffer_data[1] = -1.0f; 
	g_vertex_buffer_data[2] = 1.0f; g_vertex_buffer_data[3] = -1.0f; 
	g_vertex_buffer_data[4] = 1.0f; g_vertex_buffer_data[5] = 1.0f; 
	//top left triangle
	g_vertex_buffer_data[6] = -1.0f; g_vertex_buffer_data[7] = -1.0f; 
	g_vertex_buffer_data[8] = 1.0f; g_vertex_buffer_data[9] = 1.0f; 
	g_vertex_buffer_data[10] = -1.0f; g_vertex_buffer_data[11] = 1.0f; 
	
	// UVs
	GLfloat* g_uv_buffer_data;
	g_uv_buffer_data = new GLfloat[6*2];
	//bottom right triangle. Set the coordinates so that the quad covers [0;1] x [0;1]
	g_uv_buffer_data[0] = 0.0f; g_uv_buffer_data[1] = 1.0f;
	g_uv_buffer_data[2] = 1.0f; g_uv_buffer_data[3] = 1.0f;
	g_uv_buffer_data[4] = 1.0f; g_uv_buffer_data[5] = 0.0f;
	//top left triangle
	g_uv_buffer_data[6] = 0.0f; g_uv_buffer_data[7] = 1.0f;
	g_uv_buffer_data[8] = 1.0f; g_uv_buffer_data[9] = 0.0f;
	g_uv_buffer_data[10] = 0.0f; g_uv_buffer_data[11] = 0.0f;
	

	//--- Send the geometry to OpenGL
	// We need a vertex array object
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// Generate 1 vertex buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// Make the buffer active
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW); 
	//same for UVs...
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, 12*sizeof(GLfloat), g_uv_buffer_data, GL_STATIC_DRAW);
	
	
	//clean up
	delete [] g_vertex_buffer_data;
	delete [] g_uv_buffer_data;
}

// Input: z coordinate of the point in the volume, between 0 and 99
// Output: grid coordinates (i,j) of the slice where the point lies, between (0,0) and (9,9)
// For example, for z = 82, the fonction should return (i=2, j=8)
void slice_coordinate(float z, int & i, int & j)
{
      //coordinate of the slice
      j = z/10;
      i = z - 10*j;
      
      if(i<0) i=0;
      if(i>9) i=9;
      
      if(j<0) j=0;
      if(j>9) j=9;
}

// Input: (x,y,z) coordinates of the point in the volume, between (0,0,0) and (255,255,99)
// Output: (u,v) coordinates of the pixel in the texture, between (0,0) and (2559,2559)
void pixel_coordinate(int x, int y, int z, int & u, int & v)
{
      int i,j;
      //get the coordinates of the slice
      slice_coordinate(z,i,j);

      //compute coordinates of the pixel
      u = x+i*256;
      v = y+j*256;
      
      if(u<0) u=0;
      if(u>2559) u=2559;
      
      if(v<0) v=0;
      if(v>2559) v=2559;
}

void loadTexture(const char * imagePath)
{
    //load the volumetric data from a bitmap file
    unsigned int width, height;
    unsigned char * dataVolume = loadBMPRaw(imagePath, width, height);
    
    // Create one OpenGL texture for the volumetric data V
    glGenTextures(1, &textureVolumeID);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureVolumeID);
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, dataVolume);
    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
    
    //-------------------------------------------------------------------------------------------//
    // Compute the normal vector N = -(Gx,Gy,Gz)/Norm(Gx,Gy,Gz) at each voxel and store it as a color texture 
    //		where 	Gx = V(x+1,y,z)-V(x-1,y,z)/2, 
    //			Gy = V(x,y+1,z)-V(x,y-1,z)/2,
    //			Gz = V(x,y,z+1)-V(x,y,z-1)/2
    //		and V is the intensity of the volume at a given voxel
    //
    // Warning: compute Nx, Ny and Nz as "double", then map them between 0 and 255 for storage in a texture. 
    //
    // Use the fonction pixel_coordinate(...) defined above to identify the coordinates (u,v) of each voxel (x,y,z) in the texture.
    //
    // Use the fonctions getTextureR(...) and setTextureR(...) defined in TextureLoader.h to access pixels of the texture.
    //-------------------------------------------------------------------------------------------//
    
    //Allocate memory
    unsigned char * dataNormals = new unsigned char[width*height*3];
    
    //Compute normal from gradient for each voxel (x,y) of each slice z, store it as a color in dataNormals
    for (int x = 0; x < 256 ;  x++)
	{
		for (int y = 0; y < 256 ;  y++)
		{
			for (int z = 0; z < 100 ;  z++)
			{
				// Get pixel coordinates
				int u_xp, v_xp, u_xm, v_xm, u_yp, v_yp, u_ym, v_ym, u_zp, v_zp, u_zm, v_zm;
				pixel_coordinate(x + 1, y, z, u_xp, v_xp);
				pixel_coordinate(x - 1, y, z, u_xm, v_xm);
				pixel_coordinate(x, y + 1, z, u_yp, v_yp);
				pixel_coordinate(x, y - 1, z, u_ym, v_ym);
				pixel_coordinate(x, y, z + 1, u_zp, v_zp);
				pixel_coordinate(x, y, z - 1, u_zm, v_zm);

				// Compute values. In this case, using getTextureB, getTextureG, getTextureR does not make any difference as the three channels store the same value.
				double Gx = (getTextureB(u_xp,v_xp,dataVolume,width,height) - getTextureB(u_xm,v_xm,dataVolume,width,height))/2.0;
				double Gy = (getTextureG(u_yp,v_yp,dataVolume,width,height) - getTextureG(u_ym,v_ym,dataVolume,width,height))/2.0;
				double Gz = (getTextureR(u_zp,v_zp,dataVolume,width,height) - getTextureR(u_zm,v_zm,dataVolume,width,height))/2.0;
				double norm = sqrt(Gx*Gx + Gy*Gy + Gz*Gz);

				unsigned char Nx = (unsigned char)((-Gx/norm + 1.0)*127.5);
				unsigned char Ny = (unsigned char)((-Gy/norm + 1.0)*127.5);
				unsigned char Nz = (unsigned char)((-Gz/norm + 1.0)*127.5);

				// Set values. In this case, we need to be careul when storing the x,y and z normal coordinates in the appropiate channel
				int u, v;
				pixel_coordinate(x, y, z, u, v);
				setTextureB(u,v,dataNormals,width,height,Nx);
				setTextureG(u,v,dataNormals,width,height,Ny);
				setTextureR(u,v,dataNormals,width,height,Nz);
			}
		}
	}
    
    // Create an OpenGL texture from dataNormals
    glGenTextures(1, &textureNormalsID);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureNormalsID);
    // Give the image to OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, dataNormals);
    // Poor filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    
    delete[] dataVolume;
    delete[] dataNormals;
}

/* -------------------------------------------------------- */

void cleanUp()
{
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteVertexArrays(1, &VertexArrayID);
}

/* -------------------------------------------------------- */

int
main(int argc, char **argv) 
{
	///
	///  glut Window
	///
	// main glut init
	glutInit(&argc, argv);
	// initial window size
	glutInitWindowSize(g_W,g_H); 
	// init display mode
#ifdef __APPLE__
    glutInitDisplayMode( GLUT_3_2_CORE_PROFILE |  GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#else
    glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
#endif
	// create main window
	g_MainWindow=glutCreateWindow("Volume Renderer");
	// set main window as current window
	glutSetWindow(g_MainWindow);
	/// setup glut callbacks
	// mouse (whenever a button is pressed)
	glutMouseFunc(mainMouse);
	// motion (whenever the mouse is moved *while* a button is down)
	glutMotionFunc(mainMotion);
	// keyboard (whenever a character key is pressed)
	glutKeyboardFunc(mainKeyboard);
	// display  (whenerver the screen needs to be painted)
	glutDisplayFunc(mainRender);
	// reshape (whenever the window size changes)
	glutReshapeFunc(mainReshape);
	// idle (whenever the application as some free time)
	glutIdleFunc(idle);

	///
	/// Shaders, geometry, texture and camera
	///
	
	//need to init Glew before anything else
#if defined(__APPLE__) || defined(__linux__)
    glewExperimental = GL_TRUE;
#endif
    glewInit();
	
	loadShaders();
	
	//Send the geometry to OpenGL
	createGeometry();
	
	if(argc > 1){
		loadTexture(argv[1]);
	} else {
		loadTexture("images/cthead_assembled.bmp");
	}

	// print a small documentation
	//printf("[q]     - quit\n");
	print_usage();

	// enter glut main loop - this *never* returns
	glutMainLoop();
	
	cleanUp();
}

/* -------------------------------------------------------- */

