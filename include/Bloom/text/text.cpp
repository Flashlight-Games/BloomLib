#define STB_TRUETYPE_IMPLEMENTATION // Fonts
#include "text.h"
#include "fstream"

using namespace Bloom; 

Text::Text(const char* path, glm::mat4* view_matrix, glm::mat4 projection_matrix, float spacing, uint16_t resolution) {
    this->path = path; this->resolution = resolution; this->spacing = 2 - spacing; this->view_matrix = view_matrix; this->projection_matrix = &projection_matrix; this->scale = (resolution / 128.0f);
    bitsize = 2048 * scale;

    unsigned char* temp_bitmap = new unsigned char[bitsize* bitsize];
    fread(ttf_buffer, 1, 1<<20, fopen(path, "rb"));

    stbtt_InitFont(&font, ttf_buffer, stbtt_GetFontOffsetForIndex(ttf_buffer, 0));
    stbtt_BakeFontBitmap(ttf_buffer,0, resolution, temp_bitmap, bitsize, bitsize, 32,96, char_data);
    texture = new Texture2D(temp_bitmap, glm::vec2(bitsize, bitsize), GL_RED, true);
   
    delete temp_bitmap;

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

Bloom::Text::~Text()
{
    delete texture;
    delete path;
    delete projection_matrix;
}

void Text::Print(glm::vec2 position, std::string text, float size, glm::vec2 screen_size, glm::vec4 color,bool centered){
    const char* ctext = text.c_str();
    texture->bind();
    qShader.use();

    position.x -= screen_size.x/2;
    position.y -= screen_size.y / 2;
    size = size / scale;
    
    glm::mat4 model_matrix = glm::mat4(1.0);
    model_matrix = glm::translate(model_matrix, glm::vec3(2*position.x/screen_size.x, 2 * position.y / screen_size.y, 0.0f));
    //model_matrix = glm::rotate(model_matrix, (float)glm::radians(0.0), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(size, size, 1.0f));

    qShader.uniformVec4(qShader.getUniformId("uColor"), color);
    qShader.uniformMat4(qShader.getUniformId("model"), model_matrix);
    qShader.uniformMat4(qShader.getUniformId("view"), *view_matrix);
    qShader.uniformMat4(qShader.getUniformId("projection"), *projection_matrix);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);

    glm::vec2 offsets = glm::vec2(0, 0);

    if (centered) {
        offsets = GetCenterOffset(text, size, screen_size);
    }
    float x = 0;
    float y = 0;
    float yOffset = offsets.y;

    float xCounter = 0;
    float xOffset = offsets.x;

    float originalX = x;
    float originalY = y;
    while (*ctext) {
        if (*ctext == '\n') {
           /* yOffset -= (resolution / spacing) * scale;
            xOffset -= xCounter; xCounter = 0;
            */
        }
        stbtt_aligned_quad q;
        stbtt_GetBakedQuad(char_data, bitsize, bitsize, *ctext-32, &x,&y,&q,1);
        float newVerts[24] = {
            // POSITION                                                                         Texture
            (q.x1 + xOffset) / screen_size[0], ((y-q.y1)+ yOffset) / screen_size[1],                      q.s1,q.t1,
            (q.x1 + xOffset) / screen_size[0], (q.y1-q.y0+(y-q.y1)+ yOffset) / screen_size[1],            q.s1,q.t0,
            (q.x0 + xOffset) / screen_size[0], (q.y1-q.y0+(y-q.y1)+ yOffset) / screen_size[1],            q.s0,q.t0,
            (q.x0 + xOffset) / screen_size[0], ((y-q.y1)+ yOffset) / screen_size[1],                      q.s0,q.t1,
        };

        xCounter = std::max(q.x0, q.x1);
            
        glBufferData(GL_ARRAY_BUFFER,sizeof(newVerts),&newVerts,GL_STATIC_DRAW);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);
        ++ctext;
    }
    
}

glm::vec2 Text::GetCenterOffset(std::string text, float size, glm::vec2 screen_size) {
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