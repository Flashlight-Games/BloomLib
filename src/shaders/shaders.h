#ifndef SHADERS
#define SHADERS

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

class Shader{
    public:
    int programID;
    int vertexID;
    int fragmentID;

    Shader(const char* vertex_source, const char* fragment_source);

    void use() const;
    int getUniformId(const char* name) const;
    
    void uniformFloat(const int id,float value) const;
    void uniformInt(const int id,int value) const;
    void uniformBool(const int id,bool value) const;
    void uniformMat4(const int id,glm::mat4 value) const;
};

void setTextureAtLocation(GLenum location, GLuint texture, int uniformID = -1);

#endif