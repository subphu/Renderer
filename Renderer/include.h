#pragma once

#include <vulkan/vulkan.h>

#define GLM_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#ifdef _WIN32
#include <Windows.h>
#elif __unix__
#include <unistd.h>
#endif

#include <stdexcept>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include <stack>
#include <math.h>
#include <chrono>
#include <filesystem>
