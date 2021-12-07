#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include <math.h>

#include "shaders/shaders.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STBI/stb_image.h"

#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "framebuffers/framebuffers.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); } // Resize viewport with window
void processInput(GLFWwindow* window) { if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS == GLFW_PRESS) {glfwSetWindowShouldClose(window,true);} } // Close window

int main()
{ 
    // INITIALIZATION
    glfwInit(); 
    
    #if defined(__APPLE__) // If using Apple device, use apple version
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // set minimum major
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);// set minimum minor
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    uint16_t windowWidth = 800; uint16_t windowHeight = 600;
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Set window not resizable
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Window", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  
    glfwMakeContextCurrent(window);
    

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) // GLAD INIT, GL Context init
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }    
    std::cout << "GL Version " <<  glGetString(GL_VERSION) << std::endl; // Requires GL Context
    stbi_set_flip_vertically_on_load(true);

    glm::mat4 projection_matrix = glm::mat4(1.0f);
    float nearClip = -1000;
    float farClip = 1000;
    projection_matrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, nearClip, farClip); // ORTHO PROJECTION

    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, -1.0f, 0.0f)); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);
    
    // PROCESSING //
    uint32_t vao;
    glGenVertexArrays(1,&vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    float verts[] = {
        // POSITION    COLOR    Texture
        -100, -100,    1,1,1,      0,0,
        100, -100,     1,1,1,      1,0,
        100, 100,      1,1,1,      1,1,
        -100,100,       1,1,1,      0,1
    };

    int indices[] = {
        0,1,2,
        0,2,3
    };


    uint32_t vbo; // Vertex buffer objects store verts on the GPU
    glGenBuffers(1,&vbo); // Set vbo id
    glBindBuffer(GL_ARRAY_BUFFER,vbo); // bind the vbo as an array buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),&verts,GL_STATIC_DRAW);
   

    uint32_t ebo; // elemt array buffer tells the gpu what indices to connect
    glGenBuffers(1,&ebo); // Set ebo id
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo); // bind the ebo as an element array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),&indices,GL_STATIC_DRAW);


    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(0*sizeof(float)))); // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(2*sizeof(float)))); // Color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(5*sizeof(float))));
    glEnableVertexAttribArray(2);

    Shader shader = Shader("res/shaders/newfile_vertex.glsl","res/shaders/newfile_fragment.glsl");
    shader.use();
    float uColorUni = shader.getUniformId("uColor");
    float uTextureUni = shader.getUniformId("uTexture");

    int width, height, channels;
    const char* source = "res/textures/window.png";
    unsigned char* image = stbi_load(source,&width,&height,&channels,0);
    uint32_t texture;
    if(image) {
        glGenTextures(1,&texture);
        glActiveTexture(GL_TEXTURE0 + 0);
        glBindTexture(GL_TEXTURE_2D,texture);
        std::cout << channels << "\n";
        if(channels == 4)
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,image); // ENABLE alpha IF 4 CHANNEL IMAGE
        else{
            glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,image); // REGULAR RGB IMAGE
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else{
        std::cout << "Failed to load resource [ " << source << " ]\n";
    }
    stbi_image_free(image);


    // EXAMPLE MODEL MATRIX
    glm::mat4 model_matrix = glm::mat4(1.0);
    model_matrix           = glm::translate(model_matrix,glm::vec3(240.0f,200.0f,0.0f));
    model_matrix           = glm::rotate(model_matrix,(float)glm::radians((float)0),glm::vec3(0.0f,0.0f,1.0f));
    model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
    shader.uniformMat4(shader.getUniformId("model"),model_matrix);

    shader.uniformMat4(shader.getUniformId("view"),view_matrix);
    shader.uniformMat4(shader.getUniformId("projection"),projection_matrix);

    Framebuffer fb = Framebuffer(windowWidth,windowHeight);
    fb.setShader("res/shaders/framebufferMulti_fragment.glsl");
    Framebuffer fb2 = Framebuffer(windowWidth,windowHeight);    
    fb.fbShader.uniformInt(fb.fbShader.getUniformId("uTexture"),fb.fbTexture);
    fb.fbShader.uniformInt(fb.fbShader.getUniformId("uTextureTwo"),fb2.fbTexture);
    
    float PosX = 0;

    while(!glfwWindowShouldClose(window)) // GAME LOOP
    {
        PosX+= sin(glfwGetTime())*0.75;
        glfwPollEvents(); processInput(window);

        // BINDING TARGET FRAMEBUFFER
        fb.use(true);
        glClearColor(0.21,0.21,0.21,1);

        // RENDER //
        float time = glfwGetTime();
        shader.use();
        shader.uniformFloat(uColorUni,sin(time));
        setTextureAtLocation(GL_TEXTURE0,texture); shader.uniformInt(uTextureUni,0);

        model_matrix = glm::mat4(1.0);
        model_matrix           = glm::translate(model_matrix,glm::vec3(240.0f,200.0f,0.0f));
        model_matrix           = glm::rotate(model_matrix,(float)glm::radians((float)0),glm::vec3(0.0f,0.0f,1.0f));
        model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
        shader.uniformMat4(shader.getUniformId("model"),model_matrix);

        glBindTexture(GL_TEXTURE_2D,texture);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);

        fb2.use(true);

        model_matrix = glm::mat4(1.0);
        model_matrix           = glm::translate(model_matrix,glm::vec3(240.0f+PosX,100.0f,0.0f));
        model_matrix           = glm::rotate(model_matrix,(float)glm::radians((float)PosX),glm::vec3(0.0f,0.0f,1.0f));
        model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
        shader.uniformMat4(shader.getUniformId("model"),model_matrix);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);
        // END RENDER //

        // REAL FRAMEBUFFER RENDER
        bindDefaultFramebuffer(true);

        fb.fbShader.use();
        setTextureAtLocation(GL_TEXTURE0,fb.fbTexture,fb.fbShader.getUniformId("uTexture"));
        setTextureAtLocation(GL_TEXTURE1,fb2.fbTexture,fb.fbShader.getUniformId("uTextureTwo"));
        fb.draw();
        
        glfwSwapBuffers(window); // Render buffer to screen
    }


    glfwTerminate();
    return 0;
}