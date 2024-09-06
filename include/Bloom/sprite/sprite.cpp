#include "sprite.h"

using namespace Bloom;

Sprite::Sprite(const char* texture_path,float pixels_per_pixel, glm::mat4* view_matrix, glm::mat4* pixel_projection_matrix, glm::vec2 origin,glm::vec2 size,glm::vec2 textureUVScale,glm::vec2 textureUVOffset){
    Init(texture_path, pixels_per_pixel, view_matrix, pixel_projection_matrix, origin, size, textureUVScale, textureUVOffset);

    SetShader(new Bloom::Shader("include/Bloom/res/engine/shaders/sprite/spriteshader_vertex.glsl", "include/Bloom/res/engine/shaders/sprite/spriteshader_fragment.glsl"),pixel_projection_matrix);
    original_shader = true;

    this->model_uid = shader->getUniformId("model");
    this->view_uid = shader->getUniformId("view");
    this->projection_uid = shader->getUniformId("projection");
    this->depth_uid = shader->getUniformId("custom_depth");
}

Bloom::Sprite::Sprite(const char* texture_path, float pixels_per_pixel, glm::mat4* view_matrix, glm::mat4* pixel_projection_matrix, Bloom::Shader* shader, glm::vec2 origin, glm::vec2 size, glm::vec2 textureUVScale, glm::vec2 textureUVOffset) {
    Init(texture_path, pixels_per_pixel, view_matrix, pixel_projection_matrix, origin, size, textureUVScale, textureUVOffset);

    SetShader(shader, pixel_projection_matrix);
    original_shader = false;

    this->model_uid = shader->getUniformId("model");
    this->view_uid = shader->getUniformId("view");
    this->projection_uid = shader->getUniformId("projection");
    this->depth_uid = shader->getUniformId("custom_depth");
}

Bloom::Sprite::~Sprite()
{
    if (original_shader)
        delete shader;
    if(texture != nullptr)
        delete texture;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}


void Bloom::Sprite::Init(const char* texture_path, float pixels_per_pixel, glm::mat4* view_matrix, glm::mat4* pixel_projection_matrix, glm::vec2 origin, glm::vec2 size, glm::vec2 textureUVScale, glm::vec2 textureUVOffset)
{
    this->ppp = pixels_per_pixel; this->view_matrix = view_matrix; this->projection_matrix = pixel_projection_matrix;
    this->UVoffset = textureUVOffset; this->UVscale = textureUVScale; this->Origin = origin;

    transformation[3] = size[0]; transformation[4] = size[1];

    glGenVertexArrays(1, &vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    this->texture = new Bloom::Texture2D(texture_path, false);

    float xScale = (this->texture->GetSize()[0] * ppp * size[0]); // Sprite width 
    float yScale = (this->texture->GetSize()[1] * ppp * size[1]); // Sprite height 

    float verts[] = {
        // POSITION                                          COLOR       Texture                                                                   
        (origin[0] - 1) * xScale,     (origin[1] - 1) * yScale,      1,1,1,      textureUVOffset[0],textureUVOffset[1],
        (origin[0]) * xScale, (origin[1] - 1) * yScale,            1,1,1,      textureUVOffset[0] + textureUVScale[0],textureUVOffset[1],
        (origin[0]) * xScale, (origin[1]) * yScale,              1,1,1,      textureUVOffset[0] + textureUVScale[0],textureUVOffset[1] + textureUVScale[1],
        (origin[0] - 1) * xScale,     (origin[1]) * yScale,        1,1,1,      textureUVOffset[0],textureUVOffset[1] + textureUVScale[1],
    };

    glGenBuffers(1, &vbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), &verts, GL_STATIC_DRAW);


    glGenBuffers(1, &ebo); // Set ebo id
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // bind the ebo as an element array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(0 * sizeof(float)))); // Position

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(2 * sizeof(float)))); // Color

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), ((void*)(5 * sizeof(float)))); // Texture

    model_matrix = glm::mat4(1.0);
    model_matrix = glm::translate(model_matrix, glm::vec3(this->transformation[0], this->transformation[1], 0.0f));
    model_matrix = glm::rotate(model_matrix, (float)glm::radians(this->transformation[2]), glm::vec3(0.0f, 0.0f, 1.0f));
    model_matrix = glm::scale(model_matrix, glm::vec3(this->transformation[3], this->transformation[4], 1.0f));
}

void Sprite::UpdateLocation(){
    model_matrix = glm::mat4(1.0);
    model_matrix           = glm::translate(model_matrix,glm::vec3(this->transformation[0],this->transformation[1],0.0f));
    model_matrix           = glm::rotate(model_matrix,(float)glm::radians(this->transformation[2]),glm::vec3(0.0f,0.0f,1.0f));
    model_matrix           = glm::scale(model_matrix,glm::vec3(this->transformation[3],this->transformation[4],1.0f));
}

void Sprite::Draw(float depth, bool shader_bound){
    UpdateLocation();

    if (!shader_bound) {
        shader->use();
    }
    setTextureAtLocation(GL_TEXTURE0,this->texture->GetID()); shader->uniformInt(uTextureUni,0);
    shader->uniformMat4(model_uid,model_matrix);
    shader->uniformMat4(view_uid,(*(view_matrix)));
    shader->uniformMat4(projection_uid, (*(projection_matrix)));
    shader->uniformFloat(depth_uid,depth);

    this->texture->bind();
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);
}

void Bloom::Sprite::DrawInstanced(int amount, bool bind_texture, float depth)
{
    shader->use();
    if(bind_texture){
        this->texture->bind();
        setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
    }
    shader->uniformMat4(view_uid, *(view_matrix));
    shader->uniformFloat(depth_uid, depth);

    UpdateLocation();

    glBindVertexArray(this->vao);
    glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0,amount);
}

void Bloom::Sprite::DrawInstanced(int amount, std::vector<GLuint> buffers_to_bind, float depth)
{
    shader->use();
    setTextureAtLocation(GL_TEXTURE0, this->texture->GetID()); shader->uniformInt(uTextureUni, 0);
    shader->uniformMat4(view_uid, *(view_matrix));
    shader->uniformFloat(depth_uid, depth);

    UpdateLocation();

    this->texture->bind();
    glBindVertexArray(this->vao);
    for (int i = 0; i < buffers_to_bind.size(); i++)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffers_to_bind.at(i));
    }
    glDrawElementsInstanced(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0, amount);
}

void Sprite::SetUVOffset(glm::vec2 offset){
    float xScale = (this->texture->GetSize()[0]*ppp*UVscale[0]); // Sprite width 
    float yScale = (this->texture->GetSize()[1]*ppp*UVscale[1]); // Sprite width 
    float verts[] = {
        // POSITION                                          COLOR       Texture
        (Origin[0]-1)*xScale,     (Origin[1]-1)*yScale,      1,1,1,      offset[0],offset[1],
        (Origin[0])*xScale, (Origin[1]-1)*yScale,            1,1,1,      offset[0]+UVscale[0],offset[1],
        (Origin[0])*xScale, (Origin[1])*yScale,              1,1,1,      offset[0]+UVscale[0],offset[1]+UVscale[1],
        (Origin[0]-1)*xScale,     (Origin[1])*yScale,        1,1,1,      offset[0],offset[1]+UVscale[1]
    };
    glBindBuffer(GL_ARRAY_BUFFER,vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),&verts,GL_STATIC_DRAW);
}

void Bloom::Sprite::SetInstanced(glm::mat4* projection_matrix)
{
    SetShader(new Bloom::Shader("include/Bloom/res/engine/shaders/sprite/spriteshader_instanced_vertex.glsl", "include/Bloom/res/engine/shaders/sprite/spriteshader_instanced_fragment.glsl"),projection_matrix);
}
