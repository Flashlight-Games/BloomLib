#ifndef DEPS
#define DEPS
#define NOMINMAX

# define PI 3.14159265358979323846

#include "soloud/soloud.h" // SOUND
#include "soloud/soloud_wav.h" // WAV FILES	

#include "dearImGui/imgui.h" // UI
#include "dearImGui/imgui_impl_glfw.h"
#include "dearImGui/imgui_impl_opengl3.h"

#include "JSON/json.hpp" // JSON

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#include <GLM/glm.hpp> // MATRICES
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include <FastNoise/FastNoise.h> // NOISE

#include <iostream>
#include <math.h>
#include <vector>
#include <string>
#include <time.h>
#include <chrono>
#include <thread>
#include <filesystem>

#include "shaders/shaders.h" // SHADERS
#include "text/text.h" // FONTS
#include "framebuffers/framebuffers.h" // FRAMEBUFFERS
#include "textures/textures.h" // TEXTURES
#include "sprite/sprite.h" // SPRITES
#include "sprite/spritesheet.h" // SPRITESHEETS
#include "sprite/sprite_subdivided.h" //SUBDIVIDEDSPRITES
#include "collision/collision.h" // COLLISION BOXES
#include "bones/bones.h" // BONE SYSTEM
#include "boneKinematics/boneKinematics.h" // BONE KINEMATICS
#include "shake/shake.h" // SCREEN SHAKE
#include "particles/particles.h" // PARTICLES
#include "framebuffers/chromatic_aberration.h" // CHROMATIC ABERRATION
#include "framebuffers/bloom.h" // BLOOM
#include "SteamTools/steamtools.h" // STEAM TOOLS
#include "waveFunctionCollapse/waveFunctionCollapse.h" // WAVE FUNCTION COLLAPSE

#endif