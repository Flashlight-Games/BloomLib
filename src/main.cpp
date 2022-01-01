#include "dearImGui/imgui.h" // UI
#include "dearImGui/imgui_impl_glfw.h"
#include "dearImGui/imgui_impl_opengl3.h"

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>


#include <iostream>
#include <math.h>
#include <string>

#include "shaders/shaders.h" // SHADERS
#include "text/text.h" // FONTS
#include "framebuffers/framebuffers.h" // FRAMEBUFFERS
#include "textures/textures.h" // TEXTURES



#include <chrono> // Time

int offsetX = 0;
bool locked = false;


void framebuffer_size_callback(GLFWwindow* window, int width, int height) { glViewport(0, 0, width, height); } // Resize viewport with window
void processInput(GLFWwindow* window) { 
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS) 
        {glfwSetWindowShouldClose(window,true);} 
    else if(glfwGetKey(window,GLFW_KEY_O) == GLFW_PRESS)
        {offsetX++;} 
    locked = glfwGetKey(window,GLFW_KEY_L) == GLFW_PRESS;
    } // Close window

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
    uint16_t pixel_windowWidth = 64; uint16_t pixel_windowHeight = 48;
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

    float nearClip = -1000;
    float farClip = 1000;
    glm::mat4 pixel_projection_matrix = glm::ortho(0.0f, (float)pixel_windowWidth, 0.0f, (float)pixel_windowHeight, nearClip, farClip); // ORTHO PROJECTION, window size you want to render at
    glm::mat4 window_projection_matrix = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, nearClip, farClip); // ORTHO PROJECTION, window size you want to render at

    glm::mat4 view_matrix = glm::mat4(1.0f);
    view_matrix = glm::translate(view_matrix, glm::vec3(0.0f, -1.0f, 0.0f)); 

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE); // Enable Double Sided Polygons
    glDisable(GL_MULTISAMPLE); // ENABLE ANTI-ALIASING
    // glfwSwapInterval(0); // DISABLE VSYNC
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window,true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // PROCESSING //
    uint32_t vao;
    glGenVertexArrays(1,&vao); // VAO can store all vertex attrib pointer configs, so we dont have to specify them for each object.
    glBindVertexArray(vao);

    float multiplier = 0.05;
    float verts[] = {
        // POSITION    COLOR    Texture
        -160*multiplier, -160*multiplier,    1,1,1,      0,0,
        160*multiplier, -160*multiplier,     1,1,1,      1,0,
        160*multiplier, 160*multiplier,      1,1,1,      1,1,
        -160*multiplier,160*multiplier,       1,1,1,      0,1
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

    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,7*sizeof(float),((void*)(5*sizeof(float)))); // Texture
    glEnableVertexAttribArray(2);

    // Shader shader = Shader("res/shaders/testing_textures/textureshader_vertex.glsl","res/shaders/testing_textures/textureshader_fragment.glsl");
    Bloom::Shader shader = Bloom::Shader("res/shaders/testing_textures/textureshader_vertex.glsl","res/shaders/testing_textures/textureshader_fragment.glsl");
    shader.use();
    float uColorUni = shader.getUniformId("uColor");
    float uTextureUni = shader.getUniformId("uTexture");

    Bloom::Texture2D testing_texture = Bloom::Texture2D("res/textures/detailTest16Border.png",false);


    // EXAMPLE MODEL MATRIX
    glm::mat4 model_matrix = glm::mat4(1.0);
    model_matrix           = glm::translate(model_matrix,glm::vec3(8.0f,100.0f,0.0f));
    model_matrix           = glm::rotate(model_matrix,(float)glm::radians((float)0),glm::vec3(0.0f,0.0f,1.0f));
    model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
    shader.uniformMat4(shader.getUniformId("model"),model_matrix);

    shader.uniformMat4(shader.getUniformId("view"),view_matrix);
    shader.uniformMat4(shader.getUniformId("projection"),pixel_projection_matrix);

    Bloom::Framebuffer fb_pixels = Bloom::Framebuffer(pixel_windowWidth,pixel_windowHeight,false);
    fb_pixels.setShader("src/shaders/framebuffer/framebuffer_fragment.glsl");
    fb_pixels.fbShader.uniformInt(fb_pixels.fbShader.getUniformId("uTexture"),fb_pixels.getTexture().getID());


    int boxPosX = 0;
    int boxPosY = 0;
    float time = 0;

    glClearColor(0.21,0.21,0.21,1);

    const char* fontPath = "res/fonts/Roboto-Thin.ttf"; //A
    Bloom::Text fontTesting = Bloom::Text(fontPath,128);

    auto beginTime = std::chrono::system_clock::now();
    auto currentTime = beginTime;
    std::chrono::duration<double> render_duration;
    std::chrono::duration<double> transform_duration;
    std::chrono::duration<double> font_duration;
    std::chrono::duration<double> imGui_duration;
    std::chrono::duration<double> frameBuffer_duration;
    std::chrono::duration<double> bufferSwap_duration;
    while(!glfwWindowShouldClose(window)) // GAME LOOP
    {
        beginTime = std::chrono::system_clock::now(); // Check begin time
        currentTime = std::chrono::system_clock::now();
        glfwPollEvents(); processInput(window);

        // BINDING FRAMEBUFFER
        fb_pixels.use(true);
        fb_pixels.fbShader.use();

        // RENDER //
        if(!locked)
            time = glfwGetTime();
        shader.use();
        shader.uniformFloat(uColorUni,1);
        setTextureAtLocation(GL_TEXTURE0,testing_texture.getID()); shader.uniformInt(uTextureUni,0);

        

        model_matrix = glm::mat4(1.0);
        model_matrix           = glm::translate(model_matrix,glm::vec3(8.0f,100.0f,0.0f));
        model_matrix           = glm::rotate(model_matrix,(float)glm::radians((float)0),glm::vec3(0.0f,0.0f,1.0f));
        model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
        shader.uniformMat4(shader.getUniformId("model"),model_matrix);

        testing_texture.bind();
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);

        model_matrix = glm::mat4(1.0);
        model_matrix           = glm::translate(model_matrix,glm::vec3((float)boxPosX+offsetX,boxPosY,0.0f));
        model_matrix           = glm::rotate(model_matrix,(float)sin(time),glm::vec3(0.0f,0.0f,1.0f));
        model_matrix           = glm::scale(model_matrix,glm::vec3(1.0f,1.0f,1.0f));
        shader.uniformMat4(shader.getUniformId("model"),model_matrix);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),GL_UNSIGNED_INT,0);

        transform_duration = std::chrono::system_clock::now()-currentTime;
        currentTime = std::chrono::system_clock::now();
        // END RENDER //

        // SCREEN FRAMEBUFFER RENDER
        bindDefaultFramebuffer(true,windowWidth,windowHeight);
        fb_pixels.draw();
        
        frameBuffer_duration = std::chrono::system_clock::now()-currentTime;
        currentTime = std::chrono::system_clock::now();

        // Text
        fontTesting.Print(glm::vec2(0*windowWidth/pixel_windowWidth,boxPosY*windowHeight/pixel_windowHeight),"Waawwaw",glm::vec2(windowWidth,windowHeight),glm::vec4(0,1,0,1));
        font_duration = std::chrono::system_clock::now()-currentTime;
        currentTime = std::chrono::system_clock::now();

        // IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Demo window");
                ImGui::TextColored(ImVec4(1,0.5,0,1),"Hello World! %s",std::to_string(((int)time)).c_str());

                ImGui::SliderInt("Box PosX",&boxPosX,0,pixel_windowWidth);
                ImGui::SliderInt("Box PosY",&boxPosY,0,pixel_windowHeight);
                ImGui::ProgressBar(transform_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Transform");
                ImGui::ProgressBar(frameBuffer_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Framebuffer");
                ImGui::ProgressBar(imGui_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"ImGui");
                ImGui::ProgressBar(font_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"Font");

                ImGui::ProgressBar(bufferSwap_duration.count() / render_duration.count(),ImVec2(ImGui::GetWindowSize()[0]-30,30),"BufferSwap");
                
                ImGui::Image((void*)(intptr_t)fb_pixels.getTexture().getID(), ImVec2(fb_pixels.getTexture().getSize()[0],fb_pixels.getTexture().getSize()[1]));
                ImGui::Image((void*)(intptr_t)fontTesting.getTexture().getID(), ImVec2(fontTesting.getTexture().getSize()[0],fontTesting.getTexture().getSize()[1]));
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        imGui_duration = std::chrono::system_clock::now()-currentTime;
        currentTime = std::chrono::system_clock::now();

        // Rendering pipeline end
        glfwSwapBuffers(window); // Render buffer to screen

        bufferSwap_duration = std::chrono::system_clock::now()-currentTime;
        render_duration = std::chrono::system_clock::now()-beginTime;
        currentTime = std::chrono::system_clock::now();
        
    }


    glfwTerminate();
    return 0;
}