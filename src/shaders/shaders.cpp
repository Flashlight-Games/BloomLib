#include "shaders.h"

using namespace Bloom;

Shader::Shader(const char* vertex_source, const char* fragment_source){
    // LOADING
    std::string vertexReadData;
    std::string fragmentReadData;

    std::ifstream vertexFile(vertex_source); // OPEN VERTEX FILE
    if(vertexFile.good()) {
        while(vertexFile.good()) {
            int currChar = vertexFile.get();
            if(currChar != -1) // IF NOT EOF
                vertexReadData += currChar;}
        vertexFile.close();}
    else{
        std::string errmsg = "FAILED TO LOAD VERTEX SHADER FROM "; errmsg.append(vertex_source);
        throw std::runtime_error(errmsg);
    }

    std::ifstream fragmentFile(fragment_source); // OPEN FRAGMENT FILE
    if(fragmentFile.good()) {
        while(fragmentFile.good()) {
            int currChar = fragmentFile.get();
            if(currChar != -1) // IF NOT EOF
                fragmentReadData += currChar;}
        fragmentFile.close(); }
    else{
        std::string errmsg = "FAILED TO LOAD FRAGMENT SHADER FROM "; errmsg.append(fragment_source);
        throw std::runtime_error(errmsg);
    }

    //COMPILING
    const char* vertexData = vertexReadData.c_str();
    const char* fragmentData = fragmentReadData.c_str();

    int success;
    char info[512];

    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID,1,&vertexData,NULL);
    glCompileShader(vertexID);

    glGetShaderiv(vertexID, GL_COMPILE_STATUS,&success);
    if (!success) { glGetShaderInfoLog(vertexID, 512, NULL, info); std::string errmsg = "VERTEX SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }

    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID,1,&fragmentData,NULL);
    glCompileShader(fragmentID);

    glGetShaderiv(fragmentID, GL_COMPILE_STATUS,&success);
    if(!success) {glGetShaderInfoLog(fragmentID,512,NULL,info); std::string errmsg = "FRAGMENT SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg);}

    //ATTACHING
    programID = glCreateProgram();
    glAttachShader(programID,vertexID);
    glAttachShader(programID,fragmentID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS,&success);
    if(!success) {glGetProgramInfoLog(programID,512,NULL,info); std::string errmsg = "SHADER PROGRAM LINK ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg);}
    glDeleteShader(fragmentID);glDeleteShader(vertexID);
};
void Shader::use() const {
    glUseProgram(programID);
}
void Shader::uniformFloat(int id,float value) const {
    glUniform1f(id,value);
    };
void Shader::uniformInt(int id,int value) const {
    glUniform1i(id,value);
};
void Shader::uniformVec2(int id, glm::vec2 value) const {
    glUniform2f(id, value.x, value.y);
};
void Shader::uniformVec3(int id, glm::vec3 value) const {
    glUniform3f(id, value.r, value.g, value.b);
};
void Shader::uniformVec4(int id,glm::vec4 value) const {
    glUniform4f(id,value.r,value.g,value.b,value.a);
};
void Shader::uniformBool(int id,bool value) const {
    glUniform1i(id,(int)value);
}
int Shader::getUniformId(const char* name) const{
    return glGetUniformLocation(programID,name);
}
void Shader::uniformMat4(int id,glm::mat4 value) const {
    glUniformMatrix4fv(id,1,GL_FALSE,glm::value_ptr(value));
}


void setTextureAtLocation(GLenum location, GLuint texture, int uniformID){
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D,texture);
    if(uniformID != -1)
        glUniform1i(uniformID,location -33984);
};