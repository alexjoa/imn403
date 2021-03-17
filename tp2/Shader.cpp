#include "shader.h"
#include <fstream>
#include <iostream>

using namespace std;

// Default constructor
Shader::Shader() {}

// Destructor
Shader::~Shader()
{
    glDetachShader(programId, vertexId);
    glDetachShader(programId, fragmentId);

    glDeleteShader(vertexId);
    glDeleteShader(fragmentId);
    glDeleteProgram(programId);
}

// Constructor
Shader::Shader(const char *vsFile, const char *fsFile)
{
    init(vsFile, fsFile); // Initialize the shader
}

// Return the id of the shader program
unsigned int Shader::id()
{
    return programId;
}

// Bind the shader
void Shader::bind()
{
    glUseProgram(programId);
}

// Unbind the shader
void Shader::unbind()
{
    glUseProgram(0);
}

// Initialize the shader
void Shader::init(const char *vsFile, const char *fsFile)
{
    if (isInitialized)
    {
        return;
    }

    isInitialized = true;

    // Create the shaders
    vertexId = glCreateShader(GL_VERTEX_SHADER);
    fragmentId = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the shaders
    string vsText = readShader(vsFile);
    string fsText = readShader(fsFile);

    const char *vertexText = vsText.c_str();
    const char *fragmentText = fsText.c_str();

    if (vertexText == NULL)
    {
        cout << "Vertex shader file not found." << endl;
        return;
    }

    if (fragmentText == NULL)
    {
        cout << "Fragment shader file not found." << endl;
        return;
    }

    glShaderSource(vertexId, 1, &vertexText, 0);
    glCompileShader(vertexId);
    validateShader(vertexId, vsFile);

    glShaderSource(fragmentId, 1, &fragmentText, 0);
    glCompileShader(fragmentId);
    validateShader(fragmentId, fsFile);

    programId = glCreateProgram();
    glAttachShader(programId, vertexId);
    glAttachShader(programId, fragmentId);

    glBindAttribLocation(programId, 0, "in_Position");
    glBindAttribLocation(programId, 1, "in_Normal");
    glBindAttribLocation(programId, 2, "in_Color");
    glBindAttribLocation(programId, 3, "in_TexCoord");

    glLinkProgram(programId);
    validateProgram(programId);
}

// Read our shader from a text file
string Shader::readShader(const char *fileName)
{
    string line = string();
    string shaderFile = string();

    ifstream file(fileName);
    if ( file.is_open() )
    {
        while ( !file.eof() )
        {
            getline(file, line);
            shaderFile.append(line);
            shaderFile.append("\n");
        }

        file.close();
    }
	
    return shaderFile;
}

// Validate if the shaders compiled correctly
void Shader::validateShader(GLuint shader, const char* file = 0)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer);
    if (length > 0)
    {
        cout << "Shader " << shader << " (" << (file != 0 ? file : "") << ") compilation error: " << buffer << endl;
    }
}

// Validate if the program linked correctly to the shaders
void Shader::validateProgram(GLuint program)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    int length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer);
    if (length > 0)
        cout << "Program " << program << " linking error: " << buffer << endl;

    glValidateProgram(program);
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status);
    if (status == GL_FALSE)
        cout << "Error validating program " << program << endl;
}