#ifndef DEPS
#define DEPS

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
#include <vector>
#include <string>

#include "shaders/shaders.h" // SHADERS
#include "text/text.h" // FONTS
#include "framebuffers/framebuffers.h" // FRAMEBUFFERS
#include "textures/textures.h" // TEXTURES
#include "sprite/sprite.h" // SPRITES
#include "sprite/spritesheet.h" // SPRITES
#include "collision/collision.h" // COLLISION BOXES
#include "bones/bones.h" // BONE SYSTEM
#include "boneKinematics/boneKinematics.h" // BONE KINEMATICS

#endif