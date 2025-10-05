#include <glad/glad.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

export module Bloom.Shaders;

import Bloom.Textures;
import std;

export namespace Bloom {
    class Shader {
    public:
        Shader(const char* vertex_source, const char* fragment_source, const char* geometry_source = nullptr) {
            // LOADING
            std::string vertexReadData;
            std::string fragmentReadData;

            std::ifstream vertexFile(vertex_source); // OPEN VERTEX FILE
            if (vertexFile.good()) {
                while (vertexFile.good()) {
                    int currChar = vertexFile.get();
                    if (currChar != -1) // IF NOT EOF
                        vertexReadData += currChar;
                }
                vertexFile.close();
            }
            else {
                std::string errmsg = "FAILED TO LOAD VERTEX SHADER FROM "; errmsg.append(vertex_source);
                throw std::runtime_error(errmsg);
            }

            std::ifstream fragmentFile(fragment_source); // OPEN FRAGMENT FILE
            if (fragmentFile.good()) {
                while (fragmentFile.good()) {
                    int currChar = fragmentFile.get();
                    if (currChar != -1) // IF NOT EOF
                        fragmentReadData += currChar;
                }
                fragmentFile.close();
            }
            else {
                std::string errmsg = "FAILED TO LOAD FRAGMENT SHADER FROM "; errmsg.append(fragment_source);
                throw std::runtime_error(errmsg);
            }

            std::string geometryReadData = "";
            if (geometry_source != nullptr) {
                std::ifstream geometryFile(geometry_source); // OPEN GEOMETRY FILE
                if (geometryFile.good()) {
                    while (geometryFile.good()) {
                        int currChar = geometryFile.get();
                        if (currChar != -1) // IF NOT EOF
                            geometryReadData += currChar;
                    }
                    geometryFile.close();
                }
                else {
                    std::string errmsg = "FAILED TO LOAD GEOMETRY SHADER FROM "; errmsg.append(geometry_source);
                    throw std::runtime_error(errmsg);
                }
            }

            //COMPILING
            const char* vertexData = vertexReadData.c_str();
            const char* fragmentData = fragmentReadData.c_str();

            int success;
            char info[512];

            GLuint vertexID = glCreateShader(GL_VERTEX_SHADER);
            glShaderSource(vertexID, 1, &vertexData, NULL);
            glCompileShader(vertexID);

            glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
            if (!success) { glGetShaderInfoLog(vertexID, 512, NULL, info); std::string errmsg = "VERTEX SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }

            GLuint fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
            glShaderSource(fragmentID, 1, &fragmentData, NULL);
            glCompileShader(fragmentID);

            glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success); // FORCE COMPILE SHADERS
            if (!success) { glGetShaderInfoLog(fragmentID, 512, NULL, info); std::string errmsg = "FRAGMENT SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }

            GLuint geometryID = glCreateShader(GL_GEOMETRY_SHADER);
            if (geometryReadData != "") {
                const char* geometryData = geometryReadData.c_str();

                glShaderSource(geometryID, 1, &geometryData, NULL);
                glCompileShader(geometryID);

                glGetShaderiv(geometryID, GL_COMPILE_STATUS, &success); // FORCE COMPILE SHADERS
                if (!success) { glGetShaderInfoLog(geometryID, 512, NULL, info); std::string errmsg = "GEOMETRY SHADER COMPILE ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }
            }

            //ATTACHING
            programID = glCreateProgram();
            glAttachShader(programID, vertexID);
            glAttachShader(programID, fragmentID);

            if (geometryReadData != "")
                glAttachShader(programID, geometryID);

            glLinkProgram(programID);

            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) { glGetProgramInfoLog(programID, 512, NULL, info); std::string errmsg = "SHADER PROGRAM LINK ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }
            glDeleteShader(fragmentID); glDeleteShader(vertexID);
        }

        void use() const {
            glUseProgram(programID);
        };
        int getUniformId(const char* name) const {
            return glGetUniformLocation(programID, name);
        }

        void uniformFloat(const int id, float value) const {
            glUniform1f(id, value);
        }
        void uniformInt(const int id, int value) const {
            glUniform1i(id, value);
        }
        void uniformUInt(const int id, unsigned int value) const {
            glUniform1ui(id, value);
        }
        void uniformBool(const int id, bool value) const {
            glUniform1i(id, (int)value);
        }
        void uniformMat4(const int id, glm::mat4 value) const {
            glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
        }
        void uniformVec2(int id, glm::vec2 value) const {
            glUniform2f(id, value.x, value.y);
        }
        void uniformVec3(int id, glm::vec3 value) const {
            glUniform3f(id, value.r, value.g, value.b);
        }
        void uniformVec4(int id, glm::vec4 value) const {
            glUniform4f(id, value.r, value.g, value.b, value.a);
        }

        int getProgramID() { return programID; };

    private:
        GLuint programID;
    };

    class ComputeShader {
    public:
        ComputeShader(const char* compute_source, glm::vec2 size) {
            int max_size = 10000 * 10000;
            if (size.x * size.y > max_size) {
                throw std::runtime_error("FAILED TO CREATE COMPUTE SHADER, SIZE LARGER THAN " + std::to_string(max_size));
            }
            this->size = size;
            this->texture = new Bloom::Texture2D(NULL, size, false, GL_R32F);

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
                throw std::runtime_error("FAILED TO LOAD COMPUTE SHADER FROM " + std::string(compute_source));
            }
            //COMPILING
            const char* computeData = computeReadData.c_str();

            char info[512];
            GLint success;

            GLuint computeID = glCreateShader(GL_COMPUTE_SHADER);
            glShaderSource(computeID, 1, &computeData, NULL);
            glCompileShader(computeID);

            glGetShaderiv(computeID, GL_COMPILE_STATUS, &success);
            if (!success) { 
                glGetShaderInfoLog(computeID, 512, NULL, info);
                throw std::runtime_error("COMPUTE SHADER COMPILE ERROR: " + std::string(info)); 
            }

            //ATTACHING
            programID = glCreateProgram();
            glAttachShader(programID, computeID);
            glLinkProgram(programID);

            glGetProgramiv(programID, GL_LINK_STATUS, &success);
            if (!success) { glGetProgramInfoLog(programID, 512, NULL, info); std::string errmsg = "COMPUTE SHADER PROGRAM LINK ERROR: "; errmsg.append(info); throw std::runtime_error(errmsg); }
            glDeleteShader(computeID);
        }
        ~ComputeShader() {
            delete this->texture;
        }
        void Use() {
            glUseProgram(programID);
            glActiveTexture(GL_TEXTURE0);
            this->texture->bind();
        }
        void Dispatch(bool wait = false) {
            glDispatchCompute(size.x, size.y, 1);
            if (wait) {
                glMemoryBarrier(GL_ALL_BARRIER_BITS);
            }
        }
        void SetValues(std::vector<float> values) {
            if (values.size() != size.x * size.y)
                throw std::runtime_error("COMPUTE SHADER DATA SIZE DOES NOT MATCH COMPUTE SHDAER SIZE!\n");

            glActiveTexture(GL_TEXTURE0);
            this->texture->bind();
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size.x, size.y, 0, GL_RED, GL_FLOAT, values.data());
        }
        float* ReadData() {
            float* data = new float[size.x * size.y];
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_FLOAT, data);
            return data;
        }

        auto GetSize() { return this->size; };
    private:
        int programID;
        glm::vec2 size;
        Bloom::Texture2D* texture;
    };
}

export void setTextureAtLocation(GLenum location, GLuint texture, int uniformID = -1) {
    glActiveTexture(location);
    glBindTexture(GL_TEXTURE_2D, texture);
    if (uniformID != -1)
        glUniform1i(uniformID, location - 33984);
}