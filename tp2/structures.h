#ifndef STRUCTURE_H__
#define STRUCTURE_H__

#include <GL/gl.h>
#include <glm/glm.hpp>


/**********************\
***** Structures *****
\**********************/

typedef struct
{
	int	button;      // left, middle, right
	int stateButton; // pressed or not
	int	x;
	int	y;
} MouseEvent;

typedef struct
{
	GLfloat	theta;	  // X/Z rotation angle  
	GLfloat	phi;	  // X/Z elevation angle 
	GLfloat	r;		  // distance to the origin  
	GLfloat fovy;     // field of view angle  
	GLfloat ratio;	  // height/width angle ratio  
	GLfloat nearCP;   // near clipping plane  
	GLfloat farCP;    // far clipping plane  
} CamInfo;

typedef struct
{
	glm::vec4 ambient; 	//ambiant color
	glm::vec4 diffuse;	//diffuse color
	glm::vec4 specular;	//specular color
	GLfloat shininess;	//the exponent 'n' to the cos
} Material;

typedef struct
{
	glm::vec4	ambient;  //ambiant component
	glm::vec4	diffuse;  //diffuse component
	glm::vec4	specular; //specular component
	glm::vec4	position; //Position

	GLfloat Kc;	//Constant attenuation factor
	GLfloat Kl;	//Linear attenuation factor
	GLfloat Kq;	//Quadratic attenuation factor

	bool	on; // light on or off

}Light;


#endif
