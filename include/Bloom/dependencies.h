#ifndef DEPS
#define DEPS

# define PI 3.14159265358979323846

#define SOLOUD_IMPLEMENTATION
#include "soloud/soloud.h" // SOUND
#include "soloud/soloud_wav.h" // WAV FILES	

#include "dearImGui/imgui.h" // UI
#include "dearImGui/imgui_impl_glfw.h"
#include "dearImGui/imgui_impl_opengl3.h"

#include <glad/glad.h> // OPENGL
#include <GLFW/glfw3.h>

#define GLM_FORCE_INTRINSICS
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

#include "SteamTools/steamtools.h" // STEAM TOOLS

#endif