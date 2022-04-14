#define STB_TRUETYPE_IMPLEMENTATION // Fonts
#include "text.h"
#include "fstream"

using namespace Bloom;

Text::Text(const char* path,float size, float spacing){
    this->path = path; this->size = size;this->spacing = 2 - spacing;

    bitsize = size*6;

    unsigned char temp_bitmap[256* 256];

    fread(ttf_buffer, 1, 1<<20, fopen(path, "rb"));
    stbtt_BakeFontBitmap(ttf_buffer,0, size, temp_bitmap,bitsize,bitsize, 32,96, char_data);

    texture = Texture2D(temp_bitmap,glm::vec2(bitsize,bitsize),GL_RED,true);
    glGenVertexArrays(1,&vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    float qVerts[] = {
        // POSITION   Texture
        -0.5, -0.5,   0,0,
        0.5, -0.5,   1,0,
        0.5, 0.5,    1,1,
        -0.5,0.5,    0,1
    };

    

    glGenBuffers(1,&vbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER,vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(qVerts),&qVerts,GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),((void*)(0*sizeof(float)))); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),((void*)(2*sizeof(float))));
    glEnableVertexAttribArray(1);


    glGenBuffers(1,&ebo); // Set ebo id
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo); // bind the ebo as an element array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),&indices,GL_STATIC_DRAW);
}

void Text::Print(glm::vec2 position, std::string text,glm::vec2 screen_size, glm::vec4 color){
    const char* ctext = text.c_str();
    texture.bind();
    qShader.use();
    qShader.uniformVec4(qShader.getUniformId("uColor"),color);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    float x = position[0];
    float y = position[1];
    while (*ctext) {
        if(*ctext == '\n'){
            position[1]-=size/spacing;
            x = position[0];}
        else if (*ctext >= 32 && *ctext < 127) {
            stbtt_aligned_quad q;
            stbtt_GetBakedQuad(char_data, bitsize,bitsize, *ctext-32, &x,&y,&q,1);

            float newVerts[24] = {
                // POSITION                                                                                                                            Texture
                (-screen_size[0] + q.x1+position[0]) / screen_size[0], (-screen_size[1] + (y-q.y1)+position[1]*2) / screen_size[1],                      q.s1,q.t1,
                (-screen_size[0] + q.x1+position[0]) / screen_size[0], (-screen_size[1] + q.y1-q.y0+(y-q.y1)+position[1]*2) / screen_size[1],            q.s1,q.t0,
                (-screen_size[0] + q.x0+position[0]) / screen_size[0], (-screen_size[1] + q.y1-q.y0+(y-q.y1)+position[1]*2) / screen_size[1],            q.s0,q.t0,
                (-screen_size[0] + q.x0+position[0]) / screen_size[0], (-screen_size[1] + (y-q.y1)+position[1]*2) / screen_size[1],                      q.s0,q.t1,
            };
            glBufferData(GL_ARRAY_BUFFER,sizeof(newVerts),&newVerts,GL_STATIC_DRAW);
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);
        }
        ++ctext;
    }
    
}