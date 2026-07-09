#define STB_TRUETYPE_IMPLEMENTATION // Fonts
#include "STBTT/stb_truetype.h"
#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>
#include <iostream>
#include <array>
#include "fstream"
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>

export module Bloom.Text;

import Bloom.Textures;
import Bloom.Shaders;
import std;

export namespace Bloom {
    class Text {
    public:
        Text(const char* path, glm::mat4* view_matrix, glm::mat4* projection_matrix, float spacing = -0.5, uint16_t resolution = 128) {
            this->path = path; this->resolution = resolution; this->spacing = 2 - spacing; this->view_matrix = view_matrix; this->projection_matrix = projection_matrix; this->scale = (resolution / 128.0f);
            bitsize = 2048 * scale;

            unsigned char* temp_bitmap = new unsigned char[bitsize * bitsize];
            fread(ttf_buffer, 1, 1 << 20, fopen(path, "rb"));

            stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
            stbtt_BakeFontBitmap(ttf_buffer, 0, resolution, temp_bitmap, bitsize, bitsize, 32, 96, char_data);
            texture = new Texture2D(temp_bitmap, glm::vec2(bitsize, bitsize), true, GL_RED);

            delete temp_bitmap;

            glGenVertexArrays(1, &vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
            glBindVertexArray(vao);

            float qVerts[] = {
                // POSITION   Texture
                -0.5, -0.5,   0,0,
                0.5, -0.5,   1,0,
                0.5, 0.5,    1,1,
                -0.5,0.5,    0,1
            };



            glGenBuffers(1, &vbo); // Set vbo id
            glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo as an array buffer
            glBufferData(GL_ARRAY_BUFFER, sizeof(qVerts), &qVerts, GL_STATIC_DRAW);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ((void*)(0 * sizeof(float)))); // Position
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), ((void*)(2 * sizeof(float))));
            glEnableVertexAttribArray(1);


            glGenBuffers(1, &ebo); // Set ebo id
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // bind the ebo as an element array buffer
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

            color_uid = qShader.getUniformId("uColor");
            model_uid = qShader.getUniformId("model");
            view_uid = qShader.getUniformId("view");
            projection_uid = qShader.getUniformId("projection");
            depth_uid = qShader.getUniformId("custom_depth");
        }
        ~Text() {
            delete texture;

            glDeleteVertexArrays(1, &vao);
            glDeleteBuffers(1, &vbo);
            glDeleteBuffers(1, &ebo);
        }
        void Print(glm::vec2 position, std::string text, float size, glm::vec2 screen_size, glm::vec4 color = glm::vec4(1, 1, 1, 1), bool centered = false, float depth = 0.99) {
            const char* ctext = text.c_str();
            glActiveTexture(GL_TEXTURE0);
            texture->bind();
            qShader.use();

            position.x -= screen_size.x / 2;
            position.y -= screen_size.y / 2;
            size = size / scale;

            glm::mat4 model_matrix = glm::mat4(1.0);
            model_matrix = glm::translate(model_matrix, glm::vec3(2 * position.x / screen_size.x, 2 * position.y / screen_size.y, 0.0f));
            //model_matrix = glm::rotate(model_matrix, (float)glm::radians(0.0), glm::vec3(0.0f, 0.0f, 1.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(size, size, 1.0f));

            qShader.uniformVec4(color_uid, color);
            qShader.uniformMat4(model_uid, model_matrix);
            qShader.uniformMat4(view_uid, *view_matrix);
            qShader.uniformMat4(projection_uid, *projection_matrix);
            qShader.uniformFloat(depth_uid, depth);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glm::vec2 offsets = glm::vec2(0, 0);

            if (centered) {
                offsets = GetCenterOffset(text, size, screen_size);
            }

            float x = 0;
            float y = 0;
            float yOffset = offsets.y;
            float xOffset = offsets.x;

            while (*ctext) {
                stbtt_aligned_quad q;
                if (*ctext == '\n') {
                    xOffset -= (q.x0 + xOffset);
                    yOffset -= (q.y1 - q.y0) * 1.8;
                }
                stbtt_GetBakedQuad(char_data, bitsize, bitsize, *ctext - 32, &x, &y, &q, 1);
                float newVerts[24] = {
                    // POSITION                                                                                   Texture
                    (q.x1 + xOffset) / screen_size[0], ((y - q.y1) + yOffset) / screen_size[1],                      q.s1,q.t1,
                    (q.x1 + xOffset) / screen_size[0], (q.y1 - q.y0 + (y - q.y1) + yOffset) / screen_size[1],            q.s1,q.t0,
                    (q.x0 + xOffset) / screen_size[0], (q.y1 - q.y0 + (y - q.y1) + yOffset) / screen_size[1],            q.s0,q.t0,
                    (q.x0 + xOffset) / screen_size[0], ((y - q.y1) + yOffset) / screen_size[1],                      q.s0,q.t1,
                };

                glBufferData(GL_ARRAY_BUFFER, sizeof(newVerts), &newVerts, GL_STATIC_DRAW);
                glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
                ++ctext;
            }
        }

        std::vector<std::array<float, 24>> PreCalculate(std::string text, float size, glm::vec2 screen_size, bool centered = false, float wrap_width = -1, float line_height = 1.25) {
            std::vector<std::array<float, 24>> retval = std::vector<std::array<float, 24>>();

            const char* ctext = text.c_str();

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            glm::vec2 offsets = glm::vec2(0, 0);

            size = size / scale;
            if (centered) {
                offsets = GetCenterOffset(text, size, screen_size);
                if (wrap_width > 0 && offsets.x > wrap_width)
                    offsets.x = wrap_width;
            }
            float x = 0;
            float y = 0;
            float yOffset = offsets.y;
            float xOffset = offsets.x;

            const char* ptr = ctext;
            std::vector<int> word_lengths = { 0 };
            while (*ptr) {
                if (*ptr == ' ')
                    word_lengths.push_back(0);
                else
                    word_lengths.at(word_lengths.size() - 1)++;
                ptr++;
            }

            int curr_word_idx = 0;

            std::vector<stbtt_aligned_quad> quads;
            std::vector<glm::vec2> word_pixel_sizes;

            int x_start = -1;

            stbtt_aligned_quad q;
            while (*ctext) {
                stbtt_GetBakedQuad(char_data, bitsize, bitsize, *ctext - 32, &x, &y, &q, 1);

                quads.push_back(q);

                if (*ctext == ' ') {
                    word_pixel_sizes.push_back(glm::vec2(q.x0, x_start));
                    x_start = -1;
                }
                else {
                    if (x_start == -1)
                        x_start = q.x0;
                }

                ++ctext;
            }
            word_pixel_sizes.push_back(glm::vec2(q.x0, x_start));

            float maxHeight = 35.f;

            for (int i = 0; i < quads.size(); i++) {
                auto q = quads.at(i);
                auto curr_char = text.c_str()[i];

                if (curr_char == '\n') {
                    xOffset -= (q.x1 + xOffset);
                    yOffset -= 1.f/size * line_height * maxHeight;
                }

                if (wrap_width > 0) {
                    auto curr_word_size = word_pixel_sizes.at(curr_word_idx);
                    if ((curr_word_size.x + xOffset) * (size / 2) >= wrap_width - (q.x1 - q.x0) * (size / 2)) {
                        xOffset -= (q.x0 + xOffset);
                        yOffset -= 1.f/size * line_height * maxHeight;
                    }
                }

                std::array<float, 24> newVerts = {
                    // POSITION                                                                                   Texture
                    (q.x1 + xOffset) / screen_size[0], ((y - q.y1) + yOffset) / screen_size[1],                      q.s1,q.t1,
                    (q.x1 + xOffset) / screen_size[0], (q.y1 - q.y0 + (y - q.y1) + yOffset) / screen_size[1],            q.s1,q.t0,
                    (q.x0 + xOffset) / screen_size[0], (q.y1 - q.y0 + (y - q.y1) + yOffset) / screen_size[1],            q.s0,q.t0,
                    (q.x0 + xOffset) / screen_size[0], ((y - q.y1) + yOffset) / screen_size[1],                      q.s0,q.t1,
                };

                retval.push_back(newVerts);

                if (curr_char == ' ')
                    curr_word_idx++;
            }

            return retval;
        }
        void SetData(std::vector<std::array<float, 24>> data) {
            this->precalculated_data = data;
            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER, precalculated_data.size() * sizeof(std::array<float, 24>), precalculated_data.data(), GL_STATIC_DRAW);

            std::vector<int> indices;
            for (int i = 0; i < precalculated_data.size(); i++)
            {
                indices.push_back(0);
                indices.push_back(1);
                indices.push_back(2);
                indices.push_back(0);
                indices.push_back(2);
                indices.push_back(3);
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);
        }

        void PrintPrecalculated(glm::vec2 position, glm::vec2 screen_size, glm::vec2 scale = glm::vec2(1), glm::vec4 color = glm::vec4(1, 1, 1, 1), bool centered = false, float depth = 0.99) {
            this->PrintPrecalculated(precalculated_data, position, screen_size, scale, color, centered, depth);
        }
        void PrintPrecalculated(std::vector<std::array<float, 24>> data, glm::vec2 position, glm::vec2 screen_size, glm::vec2 scale = glm::vec2(1), glm::vec4 color = glm::vec4(1, 1, 1, 1), bool centered = false, float depth = 0.99) {
            glActiveTexture(GL_TEXTURE0);
            texture->bind();
            qShader.use();

            position.x -= screen_size.x / 2;
            position.y -= screen_size.y / 2;

            glm::mat4 model_matrix = glm::mat4(1.0);
            model_matrix = glm::translate(model_matrix, glm::vec3(2 * position.x / screen_size.x, 2 * position.y / screen_size.y, 0.0f));
            //model_matrix = glm::rotate(model_matrix, (float)glm::radians(0.0), glm::vec3(0.0f, 0.0f, 1.0f));
            model_matrix = glm::scale(model_matrix, glm::vec3(scale / this->scale, 1.0f));

            qShader.uniformVec4(color_uid, color);
            qShader.uniformMat4(model_uid, model_matrix);
            qShader.uniformMat4(view_uid, *view_matrix);
            qShader.uniformMat4(projection_uid, *projection_matrix);
            qShader.uniformFloat(depth_uid, depth);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

            glBindVertexArray(vao);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);

            for (int i = 0; i < data.size(); i++) {
                glBufferData(GL_ARRAY_BUFFER, sizeof(data.at(i)), data.at(i).data(), GL_STATIC_DRAW);
                glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);
            }
        }

        const char* GetPath() { return path; };
        float GetResolution() { return resolution; };
        Texture2D* GetTexture() { return texture; };

        glm::vec2 GetPrintSize(std::string text, float size, glm::vec2 screen_size) {
            float leastX = 0; float mostX = 0;
            float leastY = 0; float mostY = 0;

            const char* ctext = text.c_str();

            float x = 0;
            float y = 0;

            while (*ctext) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(char_data, bitsize, bitsize, *ctext - 32, &x, &y, &q, 1);

                float newVerts[24] = {
                    // POSITION                                                                                                                              Texture
                    (q.x1) / screen_size[0], ((y - q.y1)) / screen_size[1],                      q.s1,q.t1,
                    (q.x1) / screen_size[0], (q.y1 - q.y0 + (y - q.y1)) / screen_size[1],            q.s1,q.t0,
                    (q.x0) / screen_size[0], (q.y1 - q.y0 + (y - q.y1)) / screen_size[1],            q.s0,q.t0,
                    (q.x0) / screen_size[0], ((y - q.y1)) / screen_size[1],                      q.s0,q.t1,
                };

                ++ctext;

                leastX = std::min(leastX, std::min(q.x0, q.x1));
                leastY = std::min(leastY, std::min(q.y0, q.y1));

                mostX = std::max(leastX, std::max(q.x0, q.x1));
                mostY = std::max(leastY, std::max(q.y0, q.y1));
            }

            return glm::vec2(mostX - leastX, mostY - leastY) * (size / 2.0f);
        }
    private:
        glm::vec2 GetCenterOffset(std::string text, float size, glm::vec2 screen_size) {
            float leastX = 0; float mostX = 0;
            float leastY = 0; float mostY = 0;

            const char* ctext = text.c_str();

            float x = 0;
            float y = 0;

            while (*ctext) {
                stbtt_aligned_quad q;
                stbtt_GetBakedQuad(char_data, bitsize, bitsize, *ctext - 32, &x, &y, &q, 1);

                float newVerts[24] = {
                    // POSITION                                                                                                                              Texture
                    (q.x1) / screen_size[0], ((y - q.y1)) / screen_size[1],                      q.s1,q.t1,
                    (q.x1) / screen_size[0], (q.y1 - q.y0 + (y - q.y1)) / screen_size[1],            q.s1,q.t0,
                    (q.x0) / screen_size[0], (q.y1 - q.y0 + (y - q.y1)) / screen_size[1],            q.s0,q.t0,
                    (q.x0) / screen_size[0], ((y - q.y1)) / screen_size[1],                      q.s0,q.t1,
                };

                ++ctext;

                leastX = std::min(leastX, std::min(q.x0, q.x1));
                leastY = std::min(leastY, std::min(q.y0, q.y1));

                mostX = std::max(leastX, std::max(q.x0, q.x1));
                mostY = std::max(leastY, std::max(q.y0, q.y1));
            }

            return glm::vec2(-(std::abs(leastX - mostX) / 2), -(std::abs(leastY - mostY) / 2));
        }

        const char* path;
        float resolution;

        unsigned char ttf_buffer[1 << 20];

        stbtt_fontinfo font;
        stbtt_bakedchar char_data[96]; // all characters
        Texture2D* texture;
        GLuint vao;
        GLuint vbo;
        GLuint ebo;
        GLuint bitsize;
        float spacing = 1.9;
        Shader qShader = Shader("Application/res/engine/shaders/font/font_vertex.glsl", "Application/res/engine/shaders/font/font_fragment.glsl");
        int indices[6] = {
            0,1,2,
            0,2,3
        };
        glm::mat4* view_matrix;
        glm::mat4* projection_matrix;
        float scale = 1;

        uint8_t color_uid;
        uint8_t model_uid;
        uint8_t view_uid;
        uint8_t projection_uid;
        uint8_t depth_uid;

        std::vector<std::array<float, 24>> precalculated_data;
    };
}