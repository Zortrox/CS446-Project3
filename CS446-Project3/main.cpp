/* Matthew Clark
   CS 446 - Computer Graphics
================================================
	- 0/1 to select a lght source
	- J/L to move light left and right
	- right click to select shading model

	- WASD to move around
	- mouse move to look around
*/

#include <GL\glew.h>
#include <GL\freeglut.h>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>

#define PI 3.14159

//move when key is held
enum keys {KEY_W, KEY_A, KEY_S, KEY_D, KEY_MAX};
bool keyPressed[KEY_MAX] = { false };

//screen width and height to modify matrices (like when resizing)
int screenWidth = 800;
int screenHeight = 600;

//mouse coords to determine how much to move camera
int mouseX = screenWidth / 2;
int mouseY = screenHeight / 2;

class Camera {
public:
	void look() {
		//set aspect & perspective
		GLfloat aspect = (GLfloat)screenWidth / (screenHeight > 0 ? screenHeight : 1);
		gluPerspective(m_fov, aspect, 0.1f, 1000.0f);

		//set change amount by pitch and yaw of the camera
		GLfloat dx = cos(m_pitch) * sin(m_yaw);
		GLfloat dy = sin(m_pitch);
		GLfloat dz = cos(m_pitch) * cos(m_yaw);

		//change lookAt variables based on what keys are being held
		if (keyPressed[KEY_W]) {
			m_x += dx * m_speedMod;
			m_y += dy * m_speedMod;
			m_z += dz * m_speedMod;
		}
		if (keyPressed[KEY_S]) {
			m_x -= dx * m_speedMod;
			m_y -= dy * m_speedMod;
			m_z -= dz * m_speedMod;
		}
		if (keyPressed[KEY_A]) {
			m_x += cos(m_yaw) * m_speedMod;
			m_z += -sin(m_yaw) * m_speedMod;
		}
		if (keyPressed[KEY_D]) {
			m_x -= cos(m_yaw) * m_speedMod;
			m_z -= -sin(m_yaw) * m_speedMod;
		}

		//move the camera
		gluLookAt(m_x, m_y, m_z, m_x + dx, m_y + dy, m_z + dz, 0, 1, 0);
	}

	void changePitch(GLfloat pitch) {
		m_pitch += pitch * m_lookMod;

		//set between -PI/2 and PI/2
		if (m_pitch > PI / 2) m_pitch = (GLfloat)PI / 2;
		if (m_pitch < -PI / 2) m_pitch = (GLfloat)-PI / 2;
	}

	void changeYaw(GLfloat yaw) {
		m_yaw += yaw * m_lookMod;

		//set between 0 & 2*PI
		while (m_yaw > 2 * PI) {
			m_yaw -= 2 * (GLfloat)PI;
		}
		while (m_yaw < 0) {
			m_yaw += 2 * (GLfloat)PI;
		}
	}

	void changeFoV(GLfloat fov) {
		m_fov += fov;

		//set between 0 and 360
		if (m_fov > 360) m_fov = 360.0f;
		else if (m_fov < 0) m_fov = 0.0f;
	}

private:
	GLfloat m_x = 0;
	GLfloat m_y = 0;
	GLfloat m_z = 0;
	GLfloat m_pitch = 0;
	GLfloat m_yaw = 0;

	GLfloat m_fov = 45.0f;

	//modify movement and look speeds
	GLfloat m_speedMod = 0.25;
	GLfloat m_lookMod = 0.5;
};

//shape to be transformed
class GLObject {
public:
	std::vector<GLfloat> buf_vertices;
	std::vector<GLfloat> buf_normals;
	std::vector<GLuint> buf_faces;
	std::vector<GLfloat> buf_colors;
};

//global camera and drawing object
Camera cam = Camera();
GLObject objModel;

void displayLoop(void) {
	//clear screen bits and enable 3D stuff
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);

	//===========================================================================
	//	DRAW MODEL
	//===========================================================================
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//change camera view
	cam.look();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//draw everything in front of the camera
	glTranslatef(0, 0, 10);
	glRotatef(90, 1.0, 0.0f, 0.0f);
	glRotatef(180, 0.0f, 0.0f, 1.0f);

	//Buddha model
	GLfloat color[] = { 1.0f, 0.843f, 0.0f, 1.0f };	//gold
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &objModel.buf_vertices[0]);
	glNormalPointer(GL_FLOAT, 0, &objModel.buf_normals[0]);
	glDrawElements(GL_TRIANGLES, (GLsizei)objModel.buf_faces.size(), GL_UNSIGNED_INT, &objModel.buf_faces[0]);

	//===========================================================================
	//	DRAW GUI
	//===========================================================================
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	//set screen space coords and disable 3D stuff
	gluOrtho2D(0, screenWidth, screenHeight, 0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_LIGHTING);

	//draw stuff on screen
	//just a test that I removed
	/*
	glBegin(GL_TRIANGLES);
	glVertex3f(0, 200, 0);
	glVertex3f(100, 100, 0);
	glVertex3f(200, 200, 0);
	glEnd();
	*/

	//swap buffers (redraw screen)
	glutSwapBuffers();
}

//sets the screen's stored size & new viewport when the window is resized
void resizeScreen(int width, int height) {
	screenWidth = width;
	screenHeight = height;
	glViewport(0, 0, screenWidth, screenHeight);
}

void pressNormalKey(unsigned char key, int x, int y) {
	switch (key) {
		//quit when Esc key is pressed
	case 27:
		exit(1);
		break;

		//move with WASD
	case 'w':
		keyPressed[KEY_W] = true;
		break;
	case 'a':
		keyPressed[KEY_A] = true;
		break;
	case 's':
		keyPressed[KEY_S] = true;
		break;
	case 'd':
		keyPressed[KEY_D] = true;
		break;
	}
}

//when a key is released
void releaseNormalKey(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		keyPressed[KEY_W] = false;
		break;
	case 'a':
		keyPressed[KEY_A] = false;
		break;
	case 's':
		keyPressed[KEY_S] = false;
		break;
	case 'd':
		keyPressed[KEY_D] = false;
		break;
	}
}

//handler for mouse buttons (not used in this program)
void mouseButton(int button, int state, int x, int y) {
	switch (button) {
	//handle the left mouse button
	case GLUT_LEFT_BUTTON:
		if (state == GLUT_UP) {
			
		} else {// state = GLUT_DOWN
			
		}
		break;
	//handle the right mouse button
	case GLUT_RIGHT_BUTTON:
		if (state == GLUT_UP) {

		} else {// state = GLUT_DOWN
			
		}
		break;
	//handle the middle mouse button
	case GLUT_MIDDLE_BUTTON:
		if (state == GLUT_UP) {

		}
		else {// state = GLUT_DOWN
			
		}
		break;
	}
}

//when mouse moves
void mouseMove(int x, int y) {
	int oldMouseX = mouseX;
	int oldMouseY = mouseY;
	mouseX = x;
	mouseY = y;

	//update 
	GLfloat deltaPitch = (GLfloat)(PI * (oldMouseY - mouseY) / 180);
	GLfloat deltaYaw = (GLfloat)(PI * (oldMouseX - mouseX) / 180);

	cam.changePitch(deltaPitch);
	cam.changeYaw(deltaYaw);
}

void generateNormal(std::vector<GLfloat>* vecFaceNormals, GLuint f1, GLuint f2, GLuint f3) {
	GLfloat v1a = objModel.buf_vertices[f1 * 3];
	GLfloat v1b = objModel.buf_vertices[f1 * 3 + 1];
	GLfloat v1c = objModel.buf_vertices[f1 * 3 + 2];

	GLfloat v2a = objModel.buf_vertices[f2 * 3];
	GLfloat v2b = objModel.buf_vertices[f2 * 3 + 1];
	GLfloat v2c = objModel.buf_vertices[f2 * 3 + 2];

	GLfloat v3a = objModel.buf_vertices[f3 * 3];
	GLfloat v3b = objModel.buf_vertices[f3 * 3 + 1];
	GLfloat v3c = objModel.buf_vertices[f3 * 3 + 2];

	GLfloat a = v2a - v1a;
	GLfloat b = v2b - v1b;
	GLfloat c = v2c - v1c;

	GLfloat x = v3a - v1a;
	GLfloat y = v3b - v1b;
	GLfloat z = v3c - v1c;

	vecFaceNormals->push_back(b*z - c*y);
	vecFaceNormals->push_back(c*x - a*z);
	vecFaceNormals->push_back(a*y - b*x);
}

std::vector<GLfloat> averageNormals(std::vector<GLfloat> vecTempNormals) {
	std::vector<GLfloat> vecNormal;

	GLfloat avgX = 0.0f;
	GLfloat avgY = 0.0f;
	GLfloat avgZ = 0.0f;

	size_t numNormals = vecTempNormals.size() / 3;
	for (size_t i = 0; i < numNormals; i++) {
		avgX += vecTempNormals[i * 3];
		avgY += vecTempNormals[i * 3 + 1];
		avgZ += vecTempNormals[i * 3 + 2];
	}

	vecNormal.push_back(avgX / numNormals);
	vecNormal.push_back(avgY / numNormals);
	vecNormal.push_back(avgZ / numNormals);

	return vecNormal;
}

//load object
void init() {
	objModel = GLObject();

	//open object file
	std::ifstream file("cube.obj");
	if (file.is_open()) {
		std::string line;

		std::vector<GLfloat> vecFaceNormals;
		std::list<GLuint> lstTempFaces;

		bool first = true;

		//read all lines
		while(std::getline(file, line))
		{
			std::stringstream ss(line);

			std::string type;
			ss >> type;

			//grab all vertices and store in the object's vector
			if (type == "v") {
				GLfloat v1, v2, v3;
				ss >> v1;
				ss >> v2;
				ss >> v3;

				objModel.buf_vertices.push_back(v1);
				objModel.buf_vertices.push_back(v2);
				objModel.buf_vertices.push_back(v3);
			}

			//grab all faces and store in the object's vector
			else if (type == "f") {
				GLuint f1, f2, f3;
				ss >> f1;
				ss >> f2;
				ss >> f3;

				//faces
				objModel.buf_faces.push_back(f1 - 1);
				objModel.buf_faces.push_back(f2 - 1);
				objModel.buf_faces.push_back(f3 - 1);

				lstTempFaces.push_back(f1 - 1);
				lstTempFaces.push_back(f2 - 1);
				lstTempFaces.push_back(f3 - 1);
				
				generateNormal(&vecFaceNormals, f1 - 1, f2 - 1, f3 - 1);
				int lastPos = vecFaceNormals.size() - 1;

				if (first) {
					//resize normal vector buffer to vertex buffer
					objModel.buf_normals.resize(objModel.buf_vertices.size());
					first = false;
				}

				objModel.buf_normals[(f1 - 1) * 3] = vecFaceNormals[lastPos - 2];
				objModel.buf_normals[(f1 - 1) * 3 + 1] = vecFaceNormals[lastPos - 1];
				objModel.buf_normals[(f1 - 1) * 3 + 2] = vecFaceNormals[lastPos];

				/*objModel.buf_normals[(f2 - 1) * 3] = vecFaceNormals[lastPos - 2];
				objModel.buf_normals[(f2 - 1) * 3 + 1] = vecFaceNormals[lastPos - 1];
				objModel.buf_normals[(f2 - 1) * 3 + 2] = vecFaceNormals[lastPos];

				objModel.buf_normals[(f3 - 1) * 3] = vecFaceNormals[lastPos - 2];
				objModel.buf_normals[(f3 - 1) * 3 + 1] = vecFaceNormals[lastPos - 1];
				objModel.buf_normals[(f3 - 1) * 3 + 2] = vecFaceNormals[lastPos];*/
			}
		}
		file.close();

		//resize normal vector buffer to vertex buffer
		//objModel.buf_normals.resize(objModel.buf_vertices.size());

		//add flat normals
		//objModel.buf_normals = vecFaceNormals;

		//generate normal vector by averaging normals for all faces
		size_t numVertices = objModel.buf_vertices.size() / 3;
		for (size_t i = 0; i < 0/*numVertices*/; i++) {
			std::vector<GLfloat> vecTempNormals;
			std::vector<GLuint> vecIndices;

			std::cout << i << " / " << numVertices << std::endl;

			int idxFace = 0;
			for each (GLuint face in lstTempFaces) {
				if (face == i) {
					vecIndices.push_back((GLuint)i * 3);
					vecTempNormals.push_back(vecFaceNormals[i * 3]);
					vecTempNormals.push_back(vecFaceNormals[i * 3 + 1]);
					vecTempNormals.push_back(vecFaceNormals[i * 3 + 2]);

					lstTempFaces.remove(idxFace);
				}
				else {
					idxFace++;
				}
			}

			std::vector<GLfloat> avgNormal = averageNormals(vecTempNormals);

			for (int j = 0; j < vecIndices.size(); j++) {
				objModel.buf_normals[vecIndices[j]] = avgNormal[0];
				objModel.buf_normals[vecIndices[j] + 1] = avgNormal[1];
				objModel.buf_normals[vecIndices[j] + 2] = avgNormal[2];
			}
		}
	}
}

//redraw the screen @ 60 FPS
void redraw(int) {
	glutPostRedisplay();
	glutTimerFunc(1000 / 60, redraw, 0);
}

int main(int argc, char* argv[]) {
	//create double-buffer display & setup
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);

	//set depth function
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	glutInitWindowSize(screenWidth, screenHeight);
	glutInitWindowPosition(50, 50);
	glViewport(0, 0, screenWidth, screenHeight);
	glutCreateWindow("CS446 - Project 1");

	//handle drawing and screen resizing
	glutDisplayFunc(displayLoop);
	glutReshapeFunc(resizeScreen);
	//start screen refresh timer
	glutTimerFunc(1000 / 60, redraw, 0);

	//ignore repeated key holding
	glutIgnoreKeyRepeat(true);

	//handle keyboard
	//glutSpecialFunc(pressKey);
	//glutSpecialUpFunc(releaseKey);
	glutKeyboardFunc(pressNormalKey);
	glutKeyboardUpFunc(releaseNormalKey);

	//handle mouse
	//glutSetCursor(GLUT_CURSOR_NONE);
	glutWarpPointer(screenWidth / 2, screenHeight / 2);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMove);
	glutPassiveMotionFunc(mouseMove);

	//initialize the 3D model
	init();

	//lighting and shading
	glShadeModel(GL_FLAT);
	glEnable(GL_LIGHT0);
	GLfloat light0Pos[] = { 1.0f, 1.0f, 0, 0.0f };
	GLfloat light0Amb[] = { .2f, .2f, .2f, 1 };
	GLfloat light0Dif[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light0Pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0Amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0Dif);

	//glEnable(GL_LIGHT1);
	GLfloat light1Pos[] = { -1.0f, 0, 0, 0.0f };
	GLfloat light1Amb[] = { .2f, .2f, .2f, 1 };
	GLfloat light1Dif[] = { 1, 1, 1, 1 };
	glLightfv(GL_LIGHT1, GL_POSITION, light1Pos);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1Amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1Dif);

	//start the main OpenGL loop
	glutMainLoop();
	return 0;
}