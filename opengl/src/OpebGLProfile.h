#pragma once

#include <GLFW/glfw3.h>

#include <stdexcept>

enum class OPENGL_PROFILE { ANY, CORE, COMPAT };

inline unsigned int GetProfileType(OPENGL_PROFILE profile)
{
    switch (profile)
    {
    case OPENGL_PROFILE::ANY:      return GLFW_OPENGL_ANY_PROFILE;
    case OPENGL_PROFILE::CORE:     return GLFW_OPENGL_CORE_PROFILE;
    case OPENGL_PROFILE::COMPAT:   return GLFW_OPENGL_COMPAT_PROFILE;
    }
    throw std::runtime_error("Unknown OpenGL profile");
}