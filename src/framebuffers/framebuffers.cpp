#include "framebuffers.h"

Framebuffer::Framebuffer(int width,int height){

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

    glGenTextures(1,&fbTexture);
    glBindTexture(GL_TEXTURE_2D,fbTexture);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbTexture,0);
    glBindTexture(GL_TEXTURE_2D,0);

    glGenRenderbuffers(1,&rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,rbo);
    glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH24_STENCIL8,width,height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,rbo);
    glBindRenderbuffer(GL_RENDERBUFFER,0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR : GL_FRAMEBUFFER NOT COMPLETE!\n";

    glBindFramebuffer(GL_FRAMEBUFFER,0); // BIND RENDER BUFFER
};
void Framebuffer::use(bool clear){
    glBindFramebuffer(GL_FRAMEBUFFER,fbo);
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }
};
void Framebuffer::draw(){
    glClear(GL_COLOR_BUFFER_BIT);
    fbShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,fbTexture);
    glBindVertexArray(fbVao);
    glDrawArrays(GL_TRIANGLES,0,6);
};
void Framebuffer::setShader(const char* fragment_source){
    fbShader = Shader("src/shaders/framebuffer/framebuffer_vertex.glsl",fragment_source);
};

void bindDefaultFramebuffer(bool clear){
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    if(clear){
        glClear(GL_COLOR_BUFFER_BIT);
    }
};

