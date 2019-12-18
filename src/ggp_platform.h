
#ifndef _glfw3_ggp_platform_h_
#define _glfw3_ggp_platform_h_

#include <dlfcn.h>
#include <ggp_c/ggp.h>
#include <pthread.h>
#include <stdbool.h>

#define VKAPI_ATTR
#define VKAPI_CALL
#define VKAPI_PTR

#define _GLFW_PLATFORM_WINDOW_STATE _GLFWwindowGgp ggp

#define _GLFW_PLATFORM_CONTEXT_STATE
#define _GLFW_PLATFORM_MONITOR_STATE
#define _GLFW_PLATFORM_CURSOR_STATE
#define _GLFW_PLATFORM_LIBRARY_WINDOW_STATE
#define _GLFW_PLATFORM_LIBRARY_CONTEXT_STATE _GLFWlibraryGgp ggp
#define _GLFW_EGL_CONTEXT_STATE
#define _GLFW_EGL_LIBRARY_CONTEXT_STATE

#define VK_GGP_stream_descriptor_surface 1

typedef VkFlags VkStreamDescriptorSurfaceCreateFlagsGGP;

typedef struct VkStreamDescriptorSurfaceCreateInfoGGP {
    VkStructureType                            sType;
    const void*                                pNext;
    VkStreamDescriptorSurfaceCreateFlagsGGP    flags;
    GgpStreamDescriptor                        streamDescriptor;
} VkStreamDescriptorSurfaceCreateInfoGGP;

typedef VkResult (VKAPI_PTR *PFN_vkCreateStreamDescriptorSurfaceGGP)(VkInstance instance, const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);

#include "ggp_joystick.h"
#include "osmesa_context.h"
#include "posix_time.h"
#include "posix_thread.h"

#if defined(_GLFW_WIN32)
#define _glfw_dlopen(name) LoadLibraryA(name)
#define _glfw_dlclose(handle) FreeLibrary((HMODULE)handle)
#define _glfw_dlsym(handle, name) GetProcAddress((HMODULE)handle, name)
#else
#define _glfw_dlopen(name) dlopen(name, RTLD_LAZY | RTLD_LOCAL)
#define _glfw_dlclose(handle) dlclose(handle)
#define _glfw_dlsym(handle, name) dlsym(handle, name)
#endif

typedef struct _GLFWmouseGgp {
  GgpMouse mouseObject;
  int      xpos;
  int      ypos;
  GgpBool  left;
  GgpBool  middle;
  GgpBool  right;
  GgpBool button4;
  GgpBool button5;
} _GLFWmouseGgp;

typedef struct _GLFWkeyboardGgp {
  GgpKeyboard keyboardObject;
  GgpBool     keyStates[kGgpKeyCode_KeyCodeMax + 1];
  int mods;  // cached modifer bits (GLFW_MOD_*) for currently pressed modifier
             // keys
} _GLFWkeyboardGgp;

typedef struct _GLFWwindowGgp {
  GgpEventQueue    inputEventQueue;
  _GLFWmouseGgp    mouse;
  _GLFWkeyboardGgp keyboard;

  int width;
  int height;
} _GLFWwindowGgp;

typedef struct _GLFWlibraryGgp {
  // Streamer events will run on a separate thread
  pthread_t         streamerThreadId;
  GgpEventQueue     streamerEventQueue;
  GgpEventHandlerID streamStateChangedHandle;
  GgpEventHandlerID mouseHotplugHandlerId;
  GgpEventHandlerID keyboardHotplugHandlerId;
  GgpEventHandlerID textEventHandlerId;
  GgpEventHandlerID mouseInputHandlerId;
  GgpEventHandlerID keyboardInputHandlerId;
  bool              quit;

  // ...for everything else...
  int16_t keyCodes[kGgpKeyCode_KeyCodeMax + 1];
} _GLFWlibraryGgp;

#endif  // _glfw3_ggp_platform_h_
