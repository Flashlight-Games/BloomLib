#ifndef ENGINE
#define ENGINE

#include "../src/dependencies.h"

namespace Game{
    class Engine{
        public:
        Engine(bool init){if(init){Init();};};

        // EVENTS
        int Init();
        int Begin();

        // LOOPS
        int GameLoop();
        int UILoop();

        // BACKEND
        void InputLoop(GLFWwindow* window); 
        int EndLoop();
        
        private:
        glm::mat4 window_projection_matrix;                                 // PROJECTION MATRIX FOR WINDOW
        glm::mat4 pixel_projection_matrix;                                  // PROJECTION MATRIX FOR PIXELS
        glm::mat4 view_matrix;                                              // CAMERA VIEW MATRIX

        uint16_t windowWidth = 800; uint16_t windowHeight = 600;            // WINDOW SIZE
        uint16_t pixel_windowWidth = 64; uint16_t pixel_windowHeight = 48;  // PIXEL SIZE

        Bloom::Framebuffer* fb_pixels;                                      // PIXEL FRAMEBUFFER
        GLFWwindow* window;                                                 // APPLICATION WINDOW
        int fps = 0;                                                        // FRAMES PER SECOND
        float time = 0;                                                     // APP TIME
        bool isLocked = false;                                              // TIME IS LOCKED

        // GAME DATA
        Bloom::Sprite* BoxA;
        Bloom::Sprite* BoxB;

        PhysX::collisionBox* CBoxA;
        PhysX::collisionBox* CBoxB;

        Bloom::Bone* BoneA;
        Bloom::Bone* BoneB;

        int ABoxPos[2] = {0,0};
        int BBoxPos[2] = {0,0};
        int Rots[2] = {0,0};

        float aHighlight = 0;
    };

    void ResizeLoop(GLFWwindow* window, int width, int height); // GLFW RESIZING
}

#endif