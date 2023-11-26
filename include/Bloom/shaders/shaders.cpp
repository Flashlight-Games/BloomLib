#include "shaders.h"
#include <string>

/*
    this->cstest = new Bloom::ComputeShader("include/Bloom/res/engine/shaders/compute/multiplication_compute.glsl", glm::vec2(size));

    this->cstest->Use();
    this->cstest->Dispatch(true);
    float* data = this->cstest->ReadData();
    
*/


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

    glGetShaderiv(fragmentID, GL_COMPILE_STATUS,&success); // FORCE COMPILE SHADERS
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

Bloom::ComputeShader::ComputeShader(const char* compute_source, glm::vec2 size)
{
    int max_size = 10000 * 10000;
    if (size.x * size.y > max_size) {
        std::string errmsg = "FAILED TO CREATE COMPUTE SHADER, SIZE LARGER THAN "; errmsg.append(std::to_string(max_size));
        throw std::runtime_error(errmsg);
    }
    this->size = size;
    this->texture = new Bloom::Texture2D(NULL,size,GL_R32F, false);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, NULL);
    glBindImageTexture(0, this->texture->GetID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);
    
    // LOADING
    std::string computeReadData;

    std::ifstream computeFile(compute_source); // OPEN VERTEX FILE
    if (computeFile.good()) {
        while (computeFile.good()) {
            int currChar = computeFile.get();
            if (currChar != -1) // IF NOT EOF
                computeReadData += currChar;
        }
        computeFile.close();
    }
    else {
        std::string errmsg = "FAILED TO LOAD COMPUTE SHADER FROM "; errmsg.append(compute_source);
        throw std::runtime_error(errmsg);
    }
    //COMPILING
    const char* computeData = computeReadData.c_str();

    char info[512];
    GLint success;

    GLuint computeID = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeID, 1, &computeData, NULL);
    glCompileShader(computeID);

    glGetShaderiv(computeID, GL_COMPILE_STATUS, &success);
    if (!success) { glGetShaderInfoLog(computeID, 512, NULL, info); std::string errmsg = "COMPUTE SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }

    //ATTACHING
    programID = glCreateProgram();
    glAttachShader(programID, computeID);
    glLinkProgram(programID);

    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    if (!success) { glGetProgramInfoLog(programID, 512, NULL, info); std::string errmsg = "COMPUTE SHADER PROGRAM LINK ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }
    glDeleteShader(computeID);
}

Bloom::ComputeShader::~ComputeShader()
{
    delete this->texture;
}

void Bloom::ComputeShader::Use()
{
    glUseProgram(programID);
    glActiveTexture(GL_TEXTURE0);
    this->texture->bind();
}

void Bloom::ComputeShader::Dispatch(bool wait)
{
    glDispatchCompute(size.x, size.y, 1);
    if (wait) {
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
    }
}

void Bloom::ComputeShader::SetValues(std::vector<float> values)
{
    if(values.size() != size.x * size.y)
        throw std::runtime_error("COMPUTE SHADER DATA SIZE DOES NOT MATCH COMPUTE SHDAER SIZE!\n");

    glActiveTexture(GL_TEXTURE0);
    this->texture->bind();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, values.data());
}

float* Bloom::ComputeShader::ReadData()
{
    float* data = new float[size.x * size.y];
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
    return data;
}

