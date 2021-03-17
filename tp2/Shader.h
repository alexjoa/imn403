#include <string>
#include <gl/glew.h>

class Shader {
public:
    Shader();
    ~Shader();
    
    Shader(const char *vertexShaderFile, const char *fragmentShaderFile);

    unsigned int id();
    void bind();
    void unbind();
    void init(const char *vertexShaderFile, const char *fragmentShaderFile);

private:
    std::string readShader( const char *fileName);
    void validateShader(GLuint shader, const char* file);
    void validateProgram(GLuint program);

    unsigned int programId;
    unsigned int vertexId;
    unsigned int fragmentId;
    bool isInitialized = false;
};