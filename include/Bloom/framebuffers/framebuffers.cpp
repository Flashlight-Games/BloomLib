#include "framebuffers.h"

using namespace Bloom;

Framebuffer::Framebuffer(glm::vec2 size,bool linear_scaling,bool HDR){
    this->width = size.x;
    this->height = size.y;

    // FRAMEBUFFER CREATION
    glGenVertexArrays(1,&fbVao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(fbVao);

    float fbVerts[] = {
        // POSITION    Texture
        -1, -1,         0,0,
        1, -1,          1,0,
        1, 1,           1,1,
        -1,1,           0,1,
        -1, -1,         0,0,
        1, 1,           1,1,
    };

    glGenBuffers(1,&fbVbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER,fbVbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(fbVerts),&fbVerts,GL_STATIC_DRAW);

    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,4*sizeof(float),((void*)(0*sizeof(float)))); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,4*sizeof(float),((void*)(2*sizeof(float))));
    glEnableVertexAttribArray(1);

    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);

    if (!HDR) {
        fbTexture = new Texture2D(NULL, glm::vec2(width, height), GL_RGBA, linear_scaling);
    }
    else {
        fbTexture = new Texture2D(NULL, glm::vec2(width, height), GL_RGBA16F, linear_scaling);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbTexture->GetID(),0);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        throw std::runtime_error("GL_FRAMEBUFFER NOT COMPLETE!");

    glBindFramebuffer(GL_FRAMEBUFFER,0); // BIND RENDER BUFFER
    setTextureUniform("uTexture");
}

Bloom::Framebuffer::~Framebuffer()
{
    delete fbTexture;
}

void Framebuffer::use(bool clear){

    // DISABLE glViewport for faster performance
    glViewport(0, 0, this->width, this->height); // Change viewport size for correct texture

    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    glActiveTexture(GL_TEXTURE0);
    fbTexture->bind();
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }

    fbShader.use();
};
void Framebuffer::draw(){
    fbShader.use();
    fbShader.uniformMat4(fbShader.getUniformId("offset"), (glm::translate(glm::mat4(1.0), glm::vec3(this->offset.x, this->offset.y, 0.0f))));

    glActiveTexture(GL_TEXTURE0);
    fbTexture->bind();
    glBindVertexArray(fbVao);
    glDrawArrays(GL_TRIANGLES,0,6);
};

void Framebuffer::drawToDefault(glm::vec2 window_size, bool clear,glm::vec2 offset) {
    bindDefaultFramebuffer(clear, window_size.x, window_size.y,offset); draw();
}

void Framebuffer::setShader(const char* fragment_source){
    fbShader = Shader("include/Bloom/res/engine/shaders/framebuffer/framebuffer_vertex.glsl",fragment_source);
};

void Framebuffer::setTextureUniform(const char* uniform){
    fbShader.uniformInt(fbShader.getUniformId(uniform),getTexture()->GetID());
}

void Bloom::Framebuffer::SetLocationOffset(glm::vec2 offset)
{
    this->offset = offset;
}

void bindDefaultFramebuffer(bool clear,int width, int height, glm::vec2 offset){
    if(width && height)
        glViewport(offset.x, offset.y, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }
};