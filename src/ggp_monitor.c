
#include "internal.h"

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

void _glfwPlatformGetMonitorPos(_GLFWmonitor* monitor, int* xpos, int* ypos) {}

GLFWvidmode* _glfwPlatformGetVideoModes(_GLFWmonitor* monitor, int* found)
{
  return NULL;
}

void _glfwPlatformGetVideoMode(_GLFWmonitor* monitor, GLFWvidmode* mode) {}

void _glfwPlatformGetGammaRamp(_GLFWmonitor* monitor, GLFWgammaramp* ramp) {}

void _glfwPlatformSetGammaRamp(_GLFWmonitor*        monitor,
                               const GLFWgammaramp* ramp)
{
}
