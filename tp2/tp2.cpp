/******************************************************
TP2

STUDENT NAMES : Alexandre Joanisse	Pierre-Emmanuel Goffi
				17094467			18110928

DATE : 15 mars 2021
*******************************************************/


#define GLUT_DISABLE_ATEXIT_HACK

#define NDEBUG

#include <windows.h>
#include <gl/glew.h>
#include <gl/freeglut.h>
#include <GL/gl.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm> 

//GLM Includes
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include <time.h>
#include "structures.h"

// Global variables
GLint windW, windH;
MouseEvent	lastMouseEvt;
CamInfo	gCam;
GLboolean moveAxis = GL_FALSE;
bool CarteHauteur = true;

float pas[] = { 0.01, 0.05, 0.1, 0.2, 0.4, 1.0, 2.0, 5.0, 10.0 };
int p = 2;
float pas_echantillionnage = pas[p];
int objectResolution = ceil(10.0 / pas_echantillionnage);
float f_max= 1.0;
int nb_isocontours = 3;
int nb_segment;

// Transformation modes
enum
{
	Camera,
	ModeTranslateX,
	ModeTranslateY,
	ModeTranslateZ,
	ModeRotateX,
	ModeRotateY,
	ModeRotateZ,
	ActionReset,
	ActionQuit
};
int CurrentMode = Camera;

//color_maps
enum
{
	rainbow,
	grayscale,
	two_hue,
	heat_map,
	diverging
};
int color_map = rainbow;

// Mouse variables
int LeftButtonIsPressed = GL_FALSE;
int LastPosX, LastPosY;

/*
	Vertex Array IDs
	Vertex Buffer IDs
*/
unsigned int vaoCubeID;
unsigned int vboCubeID;

GLuint colorbuffer;
GLuint colorbuffer1;

unsigned int vaoAxisID;
unsigned int vboAxisID;

// Transformation matrices
glm::mat4 projectionMatrix;
glm::mat4 viewMatrix;
glm::mat4 cubeModelMatrix = glm::mat4(1.0f);
glm::mat4 axisModelMatrix = glm::mat4(1.0f);


// Shader
Shader* shader;

std::vector< std::vector<float> > F;

float deg2rad(float deg)
{
	return	0.01745329251994329547437168059786927187815f * deg;
}


void echantillonnage()
{
	std::vector< std::vector<float> > new_data;
	float f, x, y;
	float max = 0.0;

	for (size_t i = 0; i <= objectResolution; ++i)
	{
		x = -5.0 + i * pas_echantillionnage;
		std::vector<float> column;

		for (size_t j = 0; j <= objectResolution; ++j)
		{
			y = -5.0 + j * pas_echantillionnage;
			f = 2.0 * exp(-(x*x+y*y)) + exp(-(pow(x-3.0,2.0)+ pow(y-3.0, 2.0)));
			column.push_back(f);
			max = (max < fabs(f)) ? fabs(f) : max;
		}
		new_data.push_back(column);
	}

	f_max = max;
	F = new_data;
}


glm::vec3 getFcolor(float f)
{
	glm::vec3 color;

	f = f / f_max; //normalise f
	const float dx = 0.8;
	float g, b, r, w, y;
	glm::vec3 red(1.0, 0.0, 0.0);
	glm::vec3 green(0.0, 1.0, 0.0);
	glm::vec3 black(0.0, 0.0, 0.0);
	glm::vec3 yellow(1.0, 1.0, 0.0);
	glm::vec3 white(1.0, 1.0, 1.0);
	
	switch(color_map)
	{
	case rainbow: 
		g = (6.0 - 2.0 * dx)*f + dx;
		color.x = std::max(0.0, (3.0 - fabs(g - 4.0) - fabs(g - 5.0)) / 2.0);
		color.y = std::max(0.0, (4.0 - fabs(g - 2.0) - fabs(g - 4.0)) / 2.0);
		color.z = std::max(0.0, (3.0 - fabs(g - 1.0) - fabs(g - 2.0)) / 2.0);
		break;

	case grayscale:
		color= glm::vec3(f, f, f);
		break;

	case two_hue:
		g = (4.0 - 2.0 * dx)*f + dx;
		color.x = std::max(0.0, (3.0 - fabs(g - 2.0) - fabs(g - 3.0)) / 2.0);
		color.y = std::max(0.0, (3.0 - fabs(g - 1.0) - fabs(g - 2.0)) / 2.0);
		color.z = 0;
		break;

	case heat_map:
		g = 6.0*f;
		w = std::max(0.0, (3.0 - fabs(g - 5.0) - fabs(g - 6.0)) / 2.0);
		y = std::max(0.0, (4.0 - fabs(g - 3.0) - fabs(g - 5.0)) / 2.0);
		r = std::max(0.0, (4.0 - fabs(g - 1.0) - fabs(g - 3.0)) / 2.0);
		b = std::max(0.0, (3.0 - fabs(g - 0.0) - fabs(g - 1.0)) / 2.0);
		color = b * black + r * red + y * yellow + w * white;
		break;

	case diverging:
		g = (6.0 - 2.0 * dx)*f + dx;
		r = std::max(0.0, (3.0 - fabs(g - 4.0) - fabs(g - 5.0)) / 2.0);
		w = std::max(0.0, (4.0 - fabs(g - 2.0) - fabs(g - 4.0)) / 2.0);
		g = std::max(0.0, (3.0 - fabs(g - 1.0) - fabs(g - 2.0)) / 2.0);
		color = r * red + w * white + g * green;
		break;
	}
	
	return color;
}


void initCamParameters()
{
	// Camera parameters
	gCam.theta = 0.f;
	gCam.phi = 0.f;
	gCam.r = 14.f;
	gCam.fovy = 45.f;
	gCam.ratio = 1.f;
	gCam.nearCP = 1.f;
	gCam.farCP = 300.f;
}


void initMatrices(void)
{
	

	float z = gCam.r*cos(deg2rad(gCam.theta))*cos(deg2rad(gCam.phi));
	float x = gCam.r*sin(deg2rad(gCam.theta))*cos(deg2rad(gCam.phi));
	float y = gCam.r*sin(deg2rad(gCam.phi));

	viewMatrix = glm::lookAt(glm::vec3(x, y, z),
		glm::vec3(0.f, 0.f, 0.f),
		glm::vec3(0.f, 1.f, 0.f));

	projectionMatrix = glm::perspective(glm::radians(gCam.fovy), gCam.ratio, gCam.nearCP, gCam.farCP);  // Create our perspective projection matrix

}


void initPlane(void)
{
	float s = -5.f;

	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	glm::vec3 vertex;
	glm::vec3 color;
	
	for (int i = 0; i < objectResolution; i++)	//pour tout les vertex en x
	{
		for (int j = 0; j < objectResolution; j++)		//pour tout les vertex en y
		{
			// First triangle
			vertex.x = (s + (i + 1)*pas_echantillionnage); vertex.y = (s + (j + 1)*pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i + 1][j + 1] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i+1][j+1]);
			colors.push_back(color);

			vertex.x = (s + (i + 1)*pas_echantillionnage); vertex.y = (s + j * pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i + 1][j] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i+1][j]);
			colors.push_back(color);

			vertex.x = (s + i * pas_echantillionnage); vertex.y = (s + (j + 1)*pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i][j + 1] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i][j+1]);
			colors.push_back(color);

			// Second triangle
			vertex.x = (s + (i + 1)*pas_echantillionnage); vertex.y = (s + j * pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i + 1][j] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i+1][j]);
			colors.push_back(color);

			vertex.x = (s + i * pas_echantillionnage); vertex.y = (s + j * pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i][j] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i][j]);
			colors.push_back(color);

			vertex.x = (s + i * pas_echantillionnage); vertex.y = (s + (j + 1)*pas_echantillionnage); vertex.z = (CarteHauteur == true) ? F[i][j + 1] : 0.f;
			vertices.push_back(vertex);
			color = getFcolor(F[i][j+1]);
			colors.push_back(color);

		}
	}

	// Create our vertex array object
	glGenVertexArrays(1, &vaoCubeID);
	glBindVertexArray(vaoCubeID);

	// Create our vertex buffer object
	glGenBuffers(1, &vboCubeID);
	glBindBuffer(GL_ARRAY_BUFFER, vboCubeID);

	// Copy the vertices on the GPU in the "in_Position" variable (see vertex shader).
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	int in_PositionLocation = glGetAttribLocation(shader->id(), "in_Position");
	glEnableVertexAttribArray(in_PositionLocation);
	glVertexAttribPointer(in_PositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
}


void drawPlane(void)
{
	shader->bind(); // Bind our shader

	// Get the locations of our uniforms
	int projectionMatrixLocation = glGetUniformLocation(shader->id(), "projectionMatrix");
	int viewMatrixLocation = glGetUniformLocation(shader->id(), "viewMatrix");
	int modelMatrixLocation = glGetUniformLocation(shader->id(), "modelMatrix");

	// Copy the matrices and the color on the GPU: See the vertex and the fragment shader.
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &cubeModelMatrix[0][0]);

	// Draw our object
	glBindVertexArray(vaoCubeID);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_TRIANGLES, 0, objectResolution * objectResolution * 6);

	glBindVertexArray(0);
	glDisableVertexAttribArray(1);

	shader->unbind();  // Unbind our shader
}


void getArretes(int index, bool &a0, bool &a1, bool &a2, bool &a3, int &nb_arretes)
{
	a0 = false;
	a1 = false;
	a2 = false;
	a3 = false;
	nb_arretes = 0;

	switch (index)
	{
	case 0:
	case 15:
		break;
	case 1:
	case 14:
		a0 = true;
		a3 = true;
		nb_arretes = 2;
		break;
	case 2:
	case 13:
		a0 = true;
		a1 = true;
		nb_arretes = 2;
		break;
	case 3:
	case 12:
		a1 = true;
		a3 = true;
		nb_arretes = 2;
		break;
	case 4:
	case 11:
		a1 = true;
		a2 = true;
		nb_arretes = 2;
		break;
	case 5:
	case 10:
		a0 = true;
		a1 = true;
		a2 = true;
		a3 = true;
		nb_arretes = 4;
		break;
	case 6:
	case 9:
		a0 = true;
		a2 = true;
		nb_arretes = 2;
		break;
	case 7:
	case 8:
		a2 = true;
		a3 = true;
		nb_arretes = 2;
		break;
	}
}


void initIsocontours()
{
	float s = -5.f;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> colors;
	glm::vec3 color;
	if (color_map == rainbow || color_map == grayscale || color_map == diverging)
		color = glm::vec3(1.0,0.0,0.0);
	else
		color = glm::vec3(0.0, 0.4, 1.0);

	int mask0{ 0b0000'0001 }; // represents bit 0
	int mask1{ 0b0000'0010 }; // represents bit 1
	int mask2{ 0b0000'0100 }; // represents bit 2 
	int mask3{ 0b0000'1000 }; // represents bit 3

	int index, nb_arretes;
	nb_segment = 0;
	bool a0, a1, a2, a3;
	float v, delta, deltaZ1, deltaZ2;
	float v_step = f_max / (nb_isocontours+1.0);

	for (int n = 1; n <= nb_isocontours; n++)
	{
		v = float(n) * v_step;
		for (int i = 0; i < objectResolution; i++)	//pour tout les vertex en x
		{
			for (int j = 0; j < objectResolution; j++)		//pour tout les vertex en y
			{
				index = 0;

				if (F[i][j] > v)
					index |= mask0;
				if (F[i + 1][j] > v)
					index |= mask1;
				if (F[i + 1][j + 1] > v)
					index |= mask2;
				if (F[i][j + 1] > v)
					index |= mask3;

				getArretes(index, a0, a1, a2, a3, nb_arretes);

				if (nb_arretes != 0)
				{
					if (nb_arretes == 2)
					{
						nb_segment++;

						if (a0)
						{
							deltaZ1 = fabs(F[i][j] - v);
							deltaZ2 = fabs(F[i + 1][j] - v);
							delta = deltaZ1 / (deltaZ1 + deltaZ2);
							vertices.push_back(glm::vec3(s + i * pas_echantillionnage + (delta * pas_echantillionnage), s + j * pas_echantillionnage, (CarteHauteur == true) ? v : 0.f));
							colors.push_back(color);
						}
						if (a1)
						{
							deltaZ1 = fabs(F[i + 1][j] - v);
							deltaZ2 = fabs(F[i + 1][j + 1] - v);
							delta = deltaZ1 / (deltaZ1 + deltaZ2);
							vertices.push_back(glm::vec3(s + (i + 1) * pas_echantillionnage, s + j * pas_echantillionnage + (delta * pas_echantillionnage), (CarteHauteur == true) ? v : 0.f));
							colors.push_back(color);
						}
						if (a2)
						{
							deltaZ1 = fabs(F[i][j + 1] - v);
							deltaZ2 = fabs(F[i + 1][j + 1] - v);
							delta = deltaZ1 / (deltaZ1 + deltaZ2);
							vertices.push_back(glm::vec3(s + i * pas_echantillionnage + (delta * pas_echantillionnage), s + (j + 1) * pas_echantillionnage, (CarteHauteur == true) ? v : 0.f));
							colors.push_back(color);
						}
						if (a3)
						{
							deltaZ1 = fabs(F[i][j] - v);
							deltaZ2 = fabs(F[i][j + 1] - v);
							delta = deltaZ1 / (deltaZ1 + deltaZ2);
							vertices.push_back(glm::vec3(s + i * pas_echantillionnage, s + j * pas_echantillionnage + (delta * pas_echantillionnage), (CarteHauteur == true) ? v : 0.f));
							colors.push_back(color);
						}

					}
					else if (nb_arretes == 4)
					{
						nb_segment += 2;

						deltaZ1 = fabs(F[i][j] - v);
						deltaZ2 = fabs(F[i + 1][j] - v);
						delta = deltaZ1 / (deltaZ1 + deltaZ2);
						vertices.push_back(glm::vec3(s + i * pas_echantillionnage + (delta * pas_echantillionnage), s + j * pas_echantillionnage, (CarteHauteur == true) ? v : 0.f));
						colors.push_back(color);

						deltaZ1 = fabs(F[i + 1][j] - v);
						deltaZ2 = fabs(F[i + 1][j + 1] - v);
						delta = deltaZ1 / (deltaZ1 + deltaZ2);
						vertices.push_back(glm::vec3(s + (i + 1) * pas_echantillionnage, s + j * pas_echantillionnage + (delta * pas_echantillionnage), (CarteHauteur == true) ? v : 0.f));
						colors.push_back(color);

						deltaZ1 = fabs(F[i][j + 1] - v);
						deltaZ2 = fabs(F[i + 1][j + 1] - v);
						delta = deltaZ1 / (deltaZ1 + deltaZ2);
						vertices.push_back(glm::vec3(s + i * pas_echantillionnage + (delta * pas_echantillionnage), s + (j + 1) * pas_echantillionnage, (CarteHauteur == true) ? v : 0.f));
						colors.push_back(color);

						deltaZ1 = fabs(F[i][j] - v);
						deltaZ2 = fabs(F[i][j + 1] - v);
						delta = deltaZ1 / (deltaZ1 + deltaZ2);
						vertices.push_back(glm::vec3(s + i * pas_echantillionnage, s + j * pas_echantillionnage + (delta * pas_echantillionnage), (CarteHauteur == true) ? v : 0.f));
						colors.push_back(color);
					}
				}
			}
		}
	}
	if (nb_isocontours == 0)
	{
		vertices.push_back(glm::vec3(0.0, 0.0, 0.0));
		colors.push_back(glm::vec3(0.0, 0.0, 0.0));
	}
	
	// Create our vertex array object
	glGenVertexArrays(1, &vaoAxisID);
	glBindVertexArray(vaoAxisID);

	// Create our vertex buffer object
	glGenBuffers(1, &vboAxisID);
	glBindBuffer(GL_ARRAY_BUFFER, vboAxisID);

	// Copy the vertices on the GPU in the "in_Position" variable (see vertex shader).
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	int in_PositionLocation = glGetAttribLocation(shader->id(), "in_Position");
	glEnableVertexAttribArray(in_PositionLocation);
	glVertexAttribPointer(in_PositionLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glGenBuffers(1, &colorbuffer1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);
	glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(glm::vec3), &colors[0], GL_STATIC_DRAW);
}


void drawIsocontours()
{
	shader->bind(); // Bind our shader

	// Set the line width
	glLineWidth(5.0);

	// Get the locations of our uniforms
	int projectionMatrixLocation = glGetUniformLocation(shader->id(), "projectionMatrix");
	int viewMatrixLocation = glGetUniformLocation(shader->id(), "viewMatrix");
	int modelMatrixLocation = glGetUniformLocation(shader->id(), "modelMatrix");

	// Copy the matrices and the color on the GPU: See the vertex and the fragment shader.
	glUniformMatrix4fv(projectionMatrixLocation, 1, GL_FALSE, &projectionMatrix[0][0]);
	glUniformMatrix4fv(viewMatrixLocation, 1, GL_FALSE, &viewMatrix[0][0]);
	glUniformMatrix4fv(modelMatrixLocation, 1, GL_FALSE, &axisModelMatrix[0][0]);

	// Draw our object
	glBindVertexArray(vaoAxisID);

	// 2nd attribute buffer : colors
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer1);
	glVertexAttribPointer(
		1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
		3,                                // size
		GL_FLOAT,                         // type
		GL_FALSE,                         // normalized?
		0,                                // stride
		(void*)0                          // array buffer offset
	);

	glDrawArrays(GL_LINES, 0, 2* nb_segment);

	glBindVertexArray(0);
	glDisableVertexAttribArray(1);

	shader->unbind(); // Unbind our shader
}


void MenuSelection(int value)
{
	if (value == ActionReset)
	{
		cubeModelMatrix = glm::mat4(1.0f);
		axisModelMatrix = glm::mat4(1.0f);
		initCamParameters();
	}
	else if (value == ActionQuit)
	{
		exit(0);
	}
	else
		CurrentMode = value;

	glutPostRedisplay();
}

void Reshape(int width, int height)
{
	windW = (GLint)width;
	windH = (GLint)height;
}


void display(void)
{
	// Configure viewport
	glViewport(0, 0, windW, windH);

	initMatrices();
	// Update projection matrix
	projectionMatrix = glm::perspective(glm::radians(60.f), (float)windW / (float)windH, 1.f, 300.f);  // Create our perspective projection matrix

	// Reinit our color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Draw our objects

	drawPlane();
	drawIsocontours();

	glutSwapBuffers();
}


void MouseButton(int button, int state, int x, int y)
{
	// Make sure the left mouse button is pressed
	if (button == GLUT_LEFT_BUTTON)
		LeftButtonIsPressed = (state == GLUT_DOWN);

	if (LeftButtonIsPressed)
	{
		LastPosX = x;
		LastPosY = y;
	}
	if (state == GLUT_DOWN) {
		lastMouseEvt.button = button;
		lastMouseEvt.x = x;
		lastMouseEvt.y = y;
	}
	else if (state == GLUT_UP) {
		lastMouseEvt.button = -1;
		lastMouseEvt.x = -1;
		lastMouseEvt.y = -1;
	}
}


void MouseMove(int x, int y)
{
	GLfloat delta;
	GLfloat translate, rotate, scale, shear;

	delta = (float)(y - LastPosY);
	rotate = delta * 0.033f;
	translate = delta;
	scale = delta;
	shear = delta / 50.0f;
	if (scale > -0.5 && scale < 0.5)
		scale = 0.f;
	else
		scale *= -0.01f;
	scale += 1;

	glm::mat4 transformMatrix = glm::mat4(1.0f);

	int	dx = x - lastMouseEvt.x;
	int	dy = -y + lastMouseEvt.y;
	lastMouseEvt.x = x;
	lastMouseEvt.y = y;

	if (lastMouseEvt.button == GLUT_LEFT_BUTTON)
	{
		switch (CurrentMode)
		{
		case Camera:
			// Rotation
			gCam.theta -= (float)dx;
			gCam.phi -= (float)dy;
			if (gCam.phi > 89) gCam.phi = 89.f;
			if (gCam.phi < -89) gCam.phi = -89.f;
			break;

		case ModeTranslateX:
			transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translate, 0.0f, 0.0f));
			break;

		case ModeTranslateY:
			transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, translate, 0.0f));
			break;

		case ModeTranslateZ:
			transformMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, translate));
			break;

		case ModeRotateX:
			transformMatrix = glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(1.0f, 0.0f, 0.0f));
			break;

		case ModeRotateY:
			transformMatrix = glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.0f, 1.0f, 0.0f));
			break;

		case ModeRotateZ:
			transformMatrix = glm::rotate(glm::mat4(1.0f), rotate, glm::vec3(0.0f, 0.0f, 1.0f));
			break;

		}
	}
	else if (lastMouseEvt.button == GLUT_MIDDLE_BUTTON)
	{
		// Zoom in/out
		gCam.r += (float)(dx - dy);
		if (gCam.r < 1)	gCam.r = 1.f;
	}

	cubeModelMatrix = cubeModelMatrix * transformMatrix;

	glutPostRedisplay();

	LastPosX = x;
	LastPosY = y;
}


bool wire = false;
void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '+':
		if (p == 8)
			std::cout << "Le pas d'echantillonnage maximum (10.0) est deja atteint.";
		else
		{
			p++;
			pas_echantillionnage = pas[p];
			objectResolution = ceil(10.0 / pas_echantillionnage);
		}
		std::cout << "Le nouveau pas d'echantillonnage est : " << pas_echantillionnage;
		echantillonnage();
		initIsocontours();
		initPlane();
		break;

	case '-':
		if (p == 0)
			std::cout << "Le pas d'echantillonnage maximum (0.01) est deja atteint.";
		else
		{
			p--;
			pas_echantillionnage = pas[p];
			objectResolution = ceil(10.0 / pas_echantillionnage);
		}
		std::cout << "Le nouveau pas d'echantillonnage est : " << pas_echantillionnage;
		echantillonnage();
		initIsocontours();
		initPlane();
		break;

	case 'c':
	case 'C':
		color_map ++;
		if (color_map == 5)
			color_map = rainbow;
		initPlane();
		initIsocontours();
		break;

	case 'z':
	case 'Z':
		if (nb_isocontours > 0)
		{
			nb_isocontours--;
			initIsocontours();
		}
		break;

	case 'x':
	case 'X':
		if (nb_isocontours < 20)
		{
			nb_isocontours++;
			initIsocontours();
		}
		break;

	case 'd':
	case 'D':
		CarteHauteur = (CarteHauteur == true) ? false : true;
		initPlane();
		initIsocontours();
		break;

	case 'w':
	case 'W':/*Wireframe*/
		if (!wire) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		wire = !wire;
		break;

	case 'q':/* quit*/
	case 'Q':
		exit(0);
		break;
	}

	glutPostRedisplay();
}


void init(void)
{
	/* initialize random seed */
	srand((unsigned int)time(NULL));

	initCamParameters();

	initMatrices();

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	glewInit();

	// Enable depth test
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);

	// Black background
	glClearColor(0.0, 0.0, 0.0, 0.0);

	shader = new Shader("shader.vert", "shader.frag");

	echantillonnage();

	// Objects
	initIsocontours();
	initPlane();	
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	// Local variable to configure the menu
	int MenuId;

	// Init the openGL window
	windW = 768;
	windH = 768;
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windW, windH);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(4, 0); //OpenGL version

	if (glutCreateWindow("TP2-IMN-403") == GL_FALSE)
		exit(1);

	init();


	// Menu creation
	MenuId = glutCreateMenu(MenuSelection);
	glutSetMenu(MenuId);
	glutAddMenuEntry("Move camera", Camera);
	glutAddMenuEntry("Translation X", ModeTranslateX);
	glutAddMenuEntry("Translation Y", ModeTranslateY);
	glutAddMenuEntry("Translation Z", ModeTranslateZ);
	glutAddMenuEntry("Rotation X", ModeRotateX);
	glutAddMenuEntry("Rotation Y", ModeRotateY);
	glutAddMenuEntry("Rotation Z", ModeRotateZ);
	glutAddMenuEntry("Retour a l'origine", ActionReset);
	glutAddMenuEntry("Quitter", ActionQuit);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	// Callbacks
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMove);
	glutReshapeFunc(Reshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMainLoop();

	return(0);
}



