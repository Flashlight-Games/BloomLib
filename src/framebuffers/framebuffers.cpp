#include "framebuffers.h"

using namespace Bloom;

Framebuffer::Framebuffer(int width,int height,bool linear_scaling,bool HDR){
    this->width = width;
    this->height = height;

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

    if(!HDR)
        fbTexture = new Texture2D(NULL,glm::vec2(width,height),GL_RGBA,linear_scaling);
    else
        fbTexture = new Texture2D(NULL,glm::vec2(width,height),GL_RGBA16F,linear_scaling);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbTexture->getID(),0);
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
};
void Framebuffer::use(bool clear){

    // DISABLE glViewport for faster performance
    glViewport(0, 0, this->width, this->height); // Change viewport size for correct texture

    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }

    fbShader.use();
};
void Framebuffer::draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    fbShader.use();
    glActiveTexture(GL_TEXTURE0);
    fbTexture->bind();
    glBindVertexArray(fbVao);
    glDrawArrays(GL_TRIANGLES,0,6);
};

void Framebuffer::drawToDefault(int w, int h){
    bindDefaultFramebuffer(true,w,h); draw();
}

void Framebuffer::setShader(const char* fragment_source){
    fbShader = Shader("res/shaders/framebuffer/framebuffer_vertex.glsl",fragment_source);
};

void Framebuffer::setTextureUniform(const char* uniform){
    fbShader.uniformInt(fbShader.getUniformId(uniform),getTexture()->getID());
};

void bindDefaultFramebuffer(bool clear,int width, int height){
    if(width && height)
        glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }
};


