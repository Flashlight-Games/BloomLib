#include "sprite.h"

using namespace Bloom;

Sprite::Sprite(const char* texture_path,int pixels_per_pixel, glm::mat4 view_matrix, glm::mat4 pixel_projection_matrix, glm::vec2 origin,glm::vec2 size,glm::vec2 textureUVScale,glm::vec2 textureUVOffset){
    this->ppp=pixels_per_pixel;
    this->UVoffset=textureUVOffset; this->UVscale = textureUVScale;this->Origin = origin;

    transformation[3] = size[0];transformation[4] = size[1];

    glGenVertexArrays(1,&vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    this->texture = Bloom::Texture2D(texture_path,false);

    float xScale = (this->texture.getSize()[0]*ppp); // Sprite width 
    float yScale = (this->texture.getSize()[1]*ppp); // Sprite width 

    float verts[] = {
        // POSITION                                          COLOR       Texture
        (origin[0]-1)*xScale,     (origin[1]-1)*yScale,      1,1,1,      textureUVOffset[0],textureUVOffset[1],
        (origin[0])*xScale, (origin[1]-1)*yScale,            1,1,1,      textureUVOffset[0]+textureUVScale[0],textureUVOffset[1],
        (origin[0])*xScale, (origin[1])*yScale,              1,1,1,      textureUVOffset[0]+textureUVScale[0],textureUVOffset[1]+textureUVScale[1],
        (origin[0]-1)*xScale,     (origin[1])*yScale,        1,1,1,      textureUVOffset[0],textureUVOffset[1]+textureUVScale[1]
    };

    glGenBuffers(1,&vbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER,vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),&verts,GL_STATIC_DRAW);
   

    glGenBuffers(1,&ebo); // Set ebo id
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo); // bind the ebo as an element array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),&indices,GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(0*sizeof(float)))); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(2*sizeof(float)))); // Color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(5*sizeof(float)))); // Texture
    glEnableVertexAttribArray(2);

    shader.use();
    uColorUni = shader.getUniformId("uColor");
    uTextureUni = shader.getUniformId("uTexture");

    model_matrix = glm::mat4(1.0);
    model_matrix           = glm::translate(model_matrix,glm::vec3(this->transformation[0],this->transformation[1],0.0f));
    model_matrix           = glm::rotate(model_matrix,(float)glm::radians(this->transformation[2]),glm::vec3(0.0f,0.0f,1.0f));
    model_matrix           = glm::scale(model_matrix,glm::vec3(this->transformation[3],this->transformation[4],1.0f));

    shader.uniformMat4(shader.getUniformId("model"),model_matrix);
    shader.uniformMat4(shader.getUniformId("view"),view_matrix);
    shader.uniformMat4(shader.getUniformId("projection"),pixel_projection_matrix);

    this->view_matrix = &view_matrix;
};

void Sprite::UpdateLocation(){
    model_matrix = glm::mat4(1.0);
    model_matrix           = glm::translate(model_matrix,glm::vec3(this->transformation[0],this->transformation[1],0.0f));
    model_matrix           = glm::rotate(model_matrix,(float)glm::radians(this->transformation[2]),glm::vec3(0.0f,0.0f,1.0f));
    model_matrix           = glm::scale(model_matrix,glm::vec3(this->transformation[3],this->transformation[4],1.0f));
}

void Sprite::Draw(glm::mat4 view_matrix){
    shader.use();
    shader.uniformFloat(uColorUni,1);
    setTextureAtLocation(GL_TEXTURE0,this->texture.getID()); shader.uniformInt(this->uTextureUni,0);
    shader.uniformMat4(shader.getUniformId("model"),model_matrix);
    shader.uniformMat4(shader.getUniformId("view"),(view_matrix));

    this->texture.bind();
    glBindVertexArray(this->vao);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);
}

void Sprite::SetUVOffset(glm::vec2 offset){
    float xScale = (this->texture.getSize()[0]*ppp*UVscale[0]); // Sprite width 
    float yScale = (this->texture.getSize()[1]*ppp*UVscale[1]); // Sprite width 
    float verts[] = {
        // POSITION                                             COLOR       Texture
        (Origin[0]-1)*xScale,     (Origin[1]-1)*yScale,      1,1,1,      offset[0],offset[1],
        (Origin[0])*xScale, (Origin[1]-1)*yScale,            1,1,1,      offset[0]+UVscale[0],offset[1],
        (Origin[0])*xScale, (Origin[1])*yScale,              1,1,1,      offset[0]+UVscale[0],offset[1]+UVscale[1],
        (Origin[0]-1)*xScale,     (Origin[1])*yScale,        1,1,1,      offset[0],offset[1]+UVscale[1]
    };
    glBindBuffer(GL_ARRAY_BUFFER,vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),&verts,GL_STATIC_DRAW);
}