/* Linux, build: gcc "%f" -o "%e" -Wall -Wextra -lGL -lglfw -lm
   download: glfw 3.4
			 gcc 14
			 geany 2.0

   Windows, build: gcc "%f" -o "%e" -Wall -Wextra -I C:\glfw-3.4.bin.WIN64\include -L C:\glfw-3.4.bin.WIN64\lib-mingw-w64 -lglfw3dll -lopengl32
			run in terminal:  ./main-test.exe
   copy glfw3.dll from C:\glfw-3.4.bin.WIN64\lib-mingw-w64 to C:\Windows\System32
   download: glfw: glfw-3.4.bin.WIN64
			 gcc:  x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0
			 Setup your windows Path: in extended setup system/Variable environments/System variables
							   then select item: Path and click on button Change
							   button Add: C:\mingw64\bin
			 IDE:  geany-2.0_setup
*/
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "stb/stb_image.h"

// Model
struct Textures{
	char filename[100];
	unsigned int id;
};
struct Points{
	float x, y, z;
};
struct TexCoord{
	float x, y;
};
struct Faces{
	unsigned int a, b, c;
	struct TexCoord ta, tb, tc;
	unsigned int id;
};
struct Textures tex[10];
struct Points points[100];
struct Faces faces[100];
unsigned int numTex = 0;
unsigned int numPoints = 0;
unsigned int numFaces = 0;

const unsigned int windowwidth = 800;
const unsigned int windowheight = 600;
const unsigned int pointSize = 5;
const unsigned int lineWidth = 2;
const float axesSize = 3.0;

bool mouseRightButton = false;
bool mouseMiddleButton = false;
bool mouseLeftButton = false;

float rotx = 0.0f;
float roty = 0.0f;
float rotz = 0.0f;
float scale = 1.0f;

//----- FUNCTIONS ------------------------------------------------------
void loadTexture(const char *filename, unsigned int i);
void textureBind(unsigned int ID);
void textureUnbind();
void textureHide();
void textureShow();

void drawLine2D(float sx, float sy, float ex, float ey);
void drawLine(float sx, float sy, float sz, float ex, float ey, float ez);
void drawAxes();
void drawGrid(float gridLength, float gridCell);
bool stringIsEmpty(const char *str);
long fileSize(const char *filename);
void loadModel(const char *filedir);

void key(GLFWwindow * window, int key, int scancode, int action, int mods);
void mouseButton(GLFWwindow* window, int button, int action, int mods);
void mousePosition(GLFWwindow* window, double xpos, double ypos);

void error(int error, const char * description);
void perspectiveGL( GLdouble fovY, GLdouble aspect, GLdouble zNear,
					GLdouble zFar );
void display(GLFWwindow * window);
int main();

//----------------------------------------------------------------------

void loadTexture(const char *filename, unsigned int i){
	unsigned int width, height, channels;
	unsigned char *pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
	
	if (pixels == NULL){
		fprintf(stderr, "Cannot load file image %s\nSTB Reason: %s\n", filename, stbi_failure_reason());
		exit(EXIT_FAILURE);
	}
	
	glGenTextures(1, &tex[i].id);
	glBindTexture(GL_TEXTURE_2D, tex[i].id);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	
	stbi_image_free(pixels);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void textureBind(unsigned int ID){
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ID);
	glEnable(GL_TEXTURE_2D);
}

void textureUnbind(){	
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(numTex, 0);
}

void textureShow(){
	glEnable(GL_TEXTURE_2D);
}

void textureHide(){
	glDisable(GL_TEXTURE_2D);
}

void drawLine2D(float sx, float sy, float ex, float ey){
	glVertex2f(sx, sy); glVertex2f(ex, ey);
}

void drawLine(float sx, float sy, float sz, float ex, float ey,
	float ez){
	glVertex3f(sx, sy, sz); glVertex3f(ex, ey, ez);
}

void drawAxes(){
	glDisable(GL_TEXTURE_2D);
	glLineWidth(lineWidth);
	glBegin(GL_LINES);
		glColor3f(1.0, 0.0, 0.0);
		glVertex3f(-axesSize, 0.0, 0.0); glVertex3f(axesSize, 0.0, 0.0);
		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0.0, -axesSize, 0.0); glVertex3f(0.0, axesSize, 0.0);
		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0.0, 0.0, -axesSize); glVertex3f(0.0, 0.0, axesSize);
	glEnd();	
}

void drawGrid(float gridLength, float gridCell){
	textureHide();
	glLineWidth(1);
	glColor4f(0.3, 0.3, 0.3, 0.3);
	glBegin(GL_LINES);	
	for(int k=0; k<gridLength*(1/gridCell); k++) {
		drawLine(-gridLength, 0.0, gridCell*(1+k), gridLength, 0.0, gridCell*(1+k));
		drawLine(-gridLength, 0.0, -gridCell*(1+k), gridLength, 0.0, -gridCell*(1+k));
		drawLine(gridCell*(1+k), 0.0, -gridLength, gridCell*(1+k), 0.0,gridLength);
		drawLine(-gridCell*(1+k), 0.0, -gridLength, -gridCell*(1+k), 0.0,gridLength);
	}
	glEnd();
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

bool stringIsEmpty(const char *str){
	char ch;
	do {
		ch = *str++;
	  
		if(ch != ' ' &&
		   ch != '\t' &&
		   ch != '\n' &&
		   ch != '\r' &&
		   ch != '\0')
		return false;
		
	} while (ch != '\0');
	
	return true;
}

long fileSize(const char *filename){
	FILE *fp = fopen(filename, "r");
	
	if (fp == NULL)
		return -1;
	
	if (fseek(fp, 0, SEEK_END) < 0) {
		fclose(fp);
		return -1;
	}
	long size = ftell(fp);
	return size;
}

void loadModel(const char *filedir){
	char *filename = (char *)malloc(50);
	char *fileDir = (char *)malloc(50);	
	strcpy(filename, "data");
	strcpy(filename+strlen(filename), "/");
	strcpy(filename+strlen(filename), filedir);
	strcpy(filename+strlen(filename), "/");
	
	strcpy(fileDir, filename);
	strcpy(filename+strlen(filename), filedir);
	strcpy(filename+strlen(filename), ".txt");
	printf("Model: %s\n", filename);

	FILE *fp;
	char line[255];
	fp = fopen(filename, "r");
	
	if (fp == NULL)
		exit(EXIT_FAILURE);
	
	bool loadTextures = false;
	bool loadPoints = false;
	bool loadFaces = false;
	
	// Read content of the file
	while (fgets(line, 255, fp)){
		if( strstr(line, "// Textures") != NULL) {
			loadTextures = true;
			loadPoints = false;
			loadFaces = false;
		} else if( strstr(line, "// Points") != NULL) {
			loadTextures = false;
			loadPoints = true;
			loadFaces = false;
		} else if( strstr(line, "// Faces") != NULL) {
			loadTextures = false;
			loadPoints = false;
			loadFaces = true;
		} else if (stringIsEmpty(line)){
			loadTextures = false;
			loadPoints = false;
			loadFaces = false;
		} else {
			if(loadTextures){
				sscanf(line, "%s", tex[numTex].filename);
				//printf("tex %s\n", tex[numTex].filename);
				numTex++;
			} else if(loadPoints){
				sscanf(line, "%f %f %f",
					&points[numPoints].x,
  					&points[numPoints].z,
					&points[numPoints].y);
				//printf("point x = %f\n", points[numPoints].x);
				numPoints++;
			} else if(loadFaces){
				sscanf(line, "%d %d %d %f %f %f %f %f %f %d",
					&faces[numFaces].a,
					&faces[numFaces].b,
					&faces[numFaces].c,					
					&faces[numFaces].ta.x,
					&faces[numFaces].ta.y,					
					&faces[numFaces].tb.x,
					&faces[numFaces].tb.y,					
					&faces[numFaces].tc.x,
					&faces[numFaces].tc.y,					
					&faces[numFaces].id);
					numFaces++;
			}
		
		}
	}
	fclose(fp);
	
	printf(":Tex %d, Points %d, Faces %d, Size = %d B\n", 
		numTex,
		numPoints,
		numFaces,
		fileSize(filename));
	
	int lengthDir = strlen(fileDir)+1;
	char *fileDire = (char *)malloc(50);
	for(int i=0; i<numTex; i++){
		strncpy(fileDire, fileDir, lengthDir);
		strcpy(fileDire+strlen(fileDire), tex[i].filename);
		loadTexture(fileDire, i);
	}
}

void drawModel(){
	// Points
	glColor4f(1, 1, 1, 1);
	glPointSize(pointSize);
	glBegin(GL_POINTS);
		for(int k=0; k<numPoints; k++){
			glVertex3f(points[k].x,points[k].y,points[k].z );
		}
	glEnd();
	
	// Faces
	for(int l=0; l<numFaces; l++){
		glColor4f(1, 1, 1, 1);
		textureBind(tex[faces[l].id].id);
		glBegin(GL_TRIANGLES);
			glTexCoord2f(faces[l].ta.x, faces[l].ta.y);
			glVertex3f(points[faces[l].a].x, points[faces[l].a].y, points[faces[l].a].z );
			glTexCoord2f(faces[l].tb.x, faces[l].tb.y);
			glVertex3f(points[faces[l].b].x, points[faces[l].b].y, points[faces[l].b].z );
			glTexCoord2f(faces[l].tc.x, faces[l].tc.y);
			glVertex3f(points[faces[l].c].x, points[faces[l].c].y, points[faces[l].c].z );
		glEnd();
	}
	glFlush();
}

void key(GLFWwindow * window, int key, int scancode, int action, int mods){
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void mouseButton(GLFWwindow* window, int button, int action, int mods){
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        mouseRightButton = true;
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS) {
        mouseMiddleButton = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseLeftButton = true;
    } else {
		mouseRightButton = false;
		mouseMiddleButton = false;
		mouseLeftButton = false;
	}
}

void mousePosition(GLFWwindow* window, double xpos, double ypos){
    if (mouseLeftButton) {
		glfwGetCursorPos(window, &xpos, &ypos);
        rotz=xpos;
        roty=ypos;
	} else if (mouseRightButton) {
		glfwGetCursorPos(window, &xpos, &ypos);
        scale=ypos/100.0f;
	}
}

void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s (%d)\n", description, error);
}

void init(){
	loadModel("Cube");	
}

void perspectiveGL(GLdouble fovY, GLdouble aspect, GLdouble zNear, GLdouble zFar){
	const GLdouble pi = 3.1415926535897932384626433832795;
	GLdouble fW, fH;
	fH = tan( fovY / 360 * pi) * zNear;
	fW = fH * aspect;
	
	glFrustum(-fW, fW, -fH, fH, zNear, zFar);
}

void display(GLFWwindow * window){
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glClearColor(0.6, 0.6, 0.6, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT); 
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	perspectiveGL(45.0, (float)width/height, 0.1, 100.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glTranslatef(0.0, 0.0, -5.0);
	
	//glRotatef((float)glfwGetTime() * 50.f, 35.f, 20.f, 1.0f);
	glRotatef(roty, 1.0f, 0.0f, 0.0f);
	glRotatef(rotz, 0.0f, 1.0f, 0.0f);
	glRotatef(rotx, 0.0f, 0.0f, 1.0f);	
	//glRotatef((float)glfwGetTime() * 10.f, 0.0f, 1.0f, 0.0f);
	glScalef(scale, scale, scale);
	
	// Draw List
	drawAxes();
	drawGrid(2.0, 0.1);
	drawModel();
	
	// Draw 2D List
}

int main(){
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(1);
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
	GLFWwindow * window = glfwCreateWindow(windowwidth, windowheight, "GLFW", NULL, NULL);

	if (!window){
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	init();
	
	// transparency of image
	glEnable(GL_BLEND);
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Hardware inputs
	glfwSetKeyCallback(window, key);
	glfwSetCursorPosCallback(window, mousePosition);
	glfwSetMouseButtonCallback(window, mouseButton);
            
	while (!glfwWindowShouldClose(window)){
		display(window);
		glfwSwapBuffers(window);
		glfwPollEvents();		
	}

	textureUnbind();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
