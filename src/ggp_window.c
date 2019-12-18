#include "internal.h"

#include <assert.h>
#include <string.h>

// TODO(b/128539898): Temporarily call the versioned ABI functions directly, so
// that the event queue changes can go through. Switch to new API once the GGP
// headers are checked in.
extern GgpEventQueue GgpEventQueueCreate_v1(void);
extern void          GgpEventQueueDestroy_v1(GgpEventQueue queue);
extern GgpBool       GgpEventQueueProcessEvent_v1(GgpEventQueue queue,
                                                  uint32_t      wait_time_ms);

// UnregisterCallback(): Handle de-registration of callbacks. Currently unused.
static void UnregisterCallback(void* user_data) {}

// MouseInputCallback(): Callback for mouse input events.
static void MouseInputCallback(const GgpMouseInputEvent* event,
                               void*                     user_data)
{
  _GLFWwindow* window = (_GLFWwindow*)user_data;
  if (event->mouse != window->ggp.mouse.mouseObject) {
    return;
  }

  int mouse_button = 0;
  switch (event->type) {
    case kGgpMouseInputEventType_Wheel:
      _glfwInputScroll(window, event->scroll_x, event->scroll_y);
      break;

    case kGgpMouseInputEventType_ButtonDown:
    case kGgpMouseInputEventType_ButtonUp:
      switch (event->changed_button) {
        case kGgpMouseButton_Left:
          mouse_button = GLFW_MOUSE_BUTTON_LEFT;
          break;
        case kGgpMouseButton_Middle:
          mouse_button = GLFW_MOUSE_BUTTON_MIDDLE;
          break;
        case kGgpMouseButton_Right:
          mouse_button = GLFW_MOUSE_BUTTON_RIGHT;
          break;
        case kGgpMouseButton_Button4:
          mouse_button = GLFW_MOUSE_BUTTON_4;
          break;
        case kGgpMouseButton_Button5:
          mouse_button = GLFW_MOUSE_BUTTON_5;
          break;
      }

      window->ggp.mouse.left    = event->button_state_left;
      window->ggp.mouse.middle  = event->button_state_middle;
      window->ggp.mouse.right   = event->button_state_right;
      window->ggp.mouse.button4 = event->button_state_button4;
      window->ggp.mouse.button5 = event->button_state_button5;
      _glfwInputMouseClick(window, mouse_button,
                           (event->type == kGgpMouseInputEventType_ButtonDown)
                               ? GLFW_PRESS
                               : GLFW_RELEASE,
                           window->ggp.keyboard.mods);
      break;

    case kGgpMouseInputEventType_Move:
      if (event->coordinate_mode == kGgpMouseCoordinateMode_Absolute) {
        window->ggp.mouse.xpos = event->x;
        window->ggp.mouse.ypos = event->y;
        _glfwInputCursorPos(window, (double)window->ggp.mouse.xpos,
                            (double)window->ggp.mouse.ypos);
      } else if (event->coordinate_mode == kGgpMouseCoordinateMode_Relative) {
        window->ggp.mouse.xpos += event->x;  // TODO(cort): clamp to window?
        window->ggp.mouse.ypos += event->y;
        _glfwInputCursorPos(window, (double)window->ggp.mouse.xpos,
                            (double)window->ggp.mouse.ypos);
      }
      break;
  }
}

// MouseHotplugCallback(): Callback for mouse hotplug events.
static void MouseHotplugCallback(const GgpMouseHotplugEvent* event,
                                 void*                       user_data)
{
  _GLFWwindow* window = (_GLFWwindow*)user_data;

  const int      kMaxConnectedMice = 1;
  _GLFWmouseGgp* mice              = &(window->ggp.mouse);

  if (event->connected) {
    // Find first unused slot.
    int32_t slot = -1;
    for (int32_t i = 0; i < kMaxConnectedMice; ++i) {
      if (mice[i].mouseObject == kGgpInvalidReference) {
        slot = i;
        break;
      }
    }

    // Save mouse.
    if (slot != -1) {
      memset(&(mice[slot]), 0, sizeof(mice[slot]));
      mice[slot].mouseObject = event->mouse;
    }
  } else {
    // Find the slot that the mouse occupies.
    int32_t slot = -1;
    for (int32_t i = 0; i < kMaxConnectedMice; ++i) {
      if (mice[i].mouseObject == event->mouse) {
        slot = i;
        break;
      }
    }

    // Clear mouse.
    if (slot != -1) {
      mice[slot].mouseObject = kGgpInvalidReference;
    }
  }
}

// KeyboardInputCallback(): Callback for keyboard input events.
static void KeyboardInputCallback(const GgpKeyboardInputEvent* event,
                                  void*                        user_data)
{
  _GLFWwindow* window = (_GLFWwindow*)user_data;
  if (event->keyboard != window->ggp.keyboard.keyboardObject) {
    return;
  }

  GgpKeyCode keyCode     = event->key_code;
  GgpBool    newKeyState = event->type == kGgpKeyboardInputEventType_KeyDown;
  GgpBool    oldKeyState = window->ggp.keyboard.keyStates[keyCode];
  if (newKeyState != oldKeyState) {
    int key    = _glfw.ggp.keyCodes[keyCode];  // Ggp keycode -> GLFW keycode
    int action = (newKeyState == kGgpTrue) ? GLFW_PRESS : GLFW_RELEASE;
    int mods   = window->ggp.keyboard.mods;

    _glfwInputKey(window, key, (int)keyCode, action, mods);

    // Update cached keyboard state
    window->ggp.keyboard.keyStates[keyCode] = newKeyState;
    // update cached modifier bits
    switch (keyCode) {
      case kGgpKeyCode_LeftControl:
      case kGgpKeyCode_RightControl:
        if (newKeyState) {
          window->ggp.keyboard.mods |= GLFW_MOD_CONTROL;
        } else {
          window->ggp.keyboard.mods &= ~GLFW_MOD_CONTROL;
        }
        break;
      case kGgpKeyCode_LeftShift:
      case kGgpKeyCode_RightShift:
        if (newKeyState) {
          window->ggp.keyboard.mods |= GLFW_MOD_SHIFT;
        } else {
          window->ggp.keyboard.mods &= ~GLFW_MOD_SHIFT;
        }
        break;
      case kGgpKeyCode_LeftAlt:
      case kGgpKeyCode_RightAlt:
        if (newKeyState) {
          window->ggp.keyboard.mods |= GLFW_MOD_ALT;
        } else {
          window->ggp.keyboard.mods &= ~GLFW_MOD_ALT;
        }
        break;
      default:
        // not a modifer key
        break;
    }
  }
}

// SendTextKeypress(): Sends key press and release events for text input
// events.
static void SendTextKeypress(_GLFWwindow* window, GgpKeyCode keycode, int mods)
{
  _glfwInputKey(window, _glfw.ggp.keyCodes[keycode], (int)keycode, GLFW_PRESS,
                mods);
  _glfwInputKey(window, _glfw.ggp.keyCodes[keycode], (int)keycode,
                GLFW_RELEASE, mods);
};

// TextCallback(): Callback for text input events
static void TextCallback(const GgpTextEvent* event, void* user_data)
{
  _GLFWwindow* window = (_GLFWwindow*)user_data;

  if (event->action == kGgpTextAction_None) {
    // TODO(b/73267701): the input text is potentially a UTF-8 string coming
    // from an IME; for now I'm naively assuming it's a single ASCII char.
    assert(!(event->text[0] & 0x80) && event->text[1] == 0);
    GLFWbool isRegularTextInput = GLFW_TRUE;
    _glfwInputChar(window, event->text[0], 0, isRegularTextInput);
    return;
  }

  GgpBool has_ctrl =
      event->cursor_action_modifier == kGgpTextCursorActionModifier_ByWord ||
      event->cursor_action_modifier ==
          kGgpTextCursorActionModifier_SelectByWord;
  GgpBool has_shift =
      event->cursor_action_modifier == kGgpTextCursorActionModifier_Select ||
      event->cursor_action_modifier ==
          kGgpTextCursorActionModifier_SelectByWord;
  int mods =
      (has_ctrl ? GLFW_MOD_CONTROL : 0) | (has_shift ? GLFW_MOD_SHIFT : 0);

  switch (event->action) {
    case kGgpTextAction_Up:
      SendTextKeypress(window, kGgpKeyCode_Up, mods);
      break;
    case kGgpTextAction_Down:
      SendTextKeypress(window, kGgpKeyCode_Down, mods);
      break;
    case kGgpTextAction_Left:
      SendTextKeypress(window, kGgpKeyCode_Left, mods);
      break;
    case kGgpTextAction_Right:
      SendTextKeypress(window, kGgpKeyCode_Right, mods);
      break;
    case kGgpTextAction_PageUp:
      SendTextKeypress(window, kGgpKeyCode_PageUp, mods);
      break;
    case kGgpTextAction_PageDown:
      SendTextKeypress(window, kGgpKeyCode_PageDown, mods);
      break;
    case kGgpTextAction_Home:
      SendTextKeypress(window, kGgpKeyCode_Home, mods);
      break;
    case kGgpTextAction_End:
      SendTextKeypress(window, kGgpKeyCode_End, mods);
      break;
    case kGgpTextAction_Delete:
      SendTextKeypress(window, kGgpKeyCode_DeleteForward, 0);
      break;
    case kGgpTextAction_Backspace:
      SendTextKeypress(window, kGgpKeyCode_Backspace, 0);
      break;
    case kGgpTextAction_Next:
      SendTextKeypress(window, kGgpKeyCode_Tab, 0);
      break;
    case kGgpTextAction_Prev:
      SendTextKeypress(window, kGgpKeyCode_Tab, GLFW_MOD_SHIFT);
      break;
    case kGgpTextAction_Finish:
      SendTextKeypress(window, kGgpKeyCode_Enter, 0);
      break;
    case kGgpTextAction_Abort:
      SendTextKeypress(window, kGgpKeyCode_Escape, 0);
      break;
    case kGgpTextAction_Undo:
      SendTextKeypress(window, kGgpKeyCode_Z, GLFW_MOD_CONTROL);
      break;
    case kGgpTextAction_Redo:
      SendTextKeypress(window, kGgpKeyCode_Z,
                       GLFW_MOD_CONTROL | GLFW_MOD_SHIFT);
      break;
  };
}

// KeyboardHotplugCallback(): Callback for keyboard hotplug events.
static void KeyboardHotplugCallback(const GgpKeyboardHotplugEvent* event,
                                    void*                          user_data)
{
  _GLFWwindow* window = (_GLFWwindow*)user_data;

  const int         kMaxConnectedKeyboards = 1;
  _GLFWkeyboardGgp* keyboards              = &(window->ggp.keyboard);

  if (event->connected) {
    // Find first unused slot.
    int32_t slot = -1;
    for (int32_t i = 0; i < kMaxConnectedKeyboards; ++i) {
      if (keyboards[i].keyboardObject == kGgpInvalidReference) {
        slot = i;
        break;
      }
    }

    // Set keyboard.
    if (slot != -1) {
      memset(&(keyboards[slot]), 0, sizeof(keyboards[slot]));
      keyboards[slot].keyboardObject = event->keyboard;
    }
  } else {
    // Find the slot that the keyboard occupies
    int32_t slot = -1;
    for (int32_t i = 0; i < kMaxConnectedKeyboards; ++i) {
      if (keyboards[i].keyboardObject == event->keyboard) {
        slot = i;
        break;
      }
    }

    // Clear keyboard.
    if (slot != -1) {
      keyboards[slot].keyboardObject = kGgpInvalidReference;
    }
  }
}

static int createNativeWindow(_GLFWwindow*          window,
                              const _GLFWwndconfig* wndconfig)
{
  window->ggp.inputEventQueue = GgpEventQueueCreate_v1();

  window->ggp.keyboard.keyboardObject = kGgpInvalidReference;
  window->ggp.mouse.mouseObject       = kGgpInvalidReference;

  _glfw.ggp.mouseHotplugHandlerId = GgpAddMouseHotplugHandler(
      window->ggp.inputEventQueue, MouseHotplugCallback, (void*)window,
      UnregisterCallback, NULL);
  _glfw.ggp.keyboardHotplugHandlerId = GgpAddKeyboardHotplugHandler(
      window->ggp.inputEventQueue, KeyboardHotplugCallback, (void*)window,
      UnregisterCallback, NULL);
  _glfw.ggp.textEventHandlerId =
      GgpAddTextEventHandler(window->ggp.inputEventQueue, TextCallback,
                             (void*)window, UnregisterCallback);

  _glfw.ggp.mouseInputHandlerId =
      GgpAddMouseInputHandler(window->ggp.inputEventQueue, MouseInputCallback,
                              (void*)window, UnregisterCallback, NULL);
  _glfw.ggp.keyboardInputHandlerId = GgpAddKeyboardInputHandler(
      window->ggp.inputEventQueue, KeyboardInputCallback, (void*)window,
      UnregisterCallback, NULL);

  window->ggp.width  = wndconfig->width;
  window->ggp.height = wndconfig->height;

  return GLFW_TRUE;
}

static void destroyNativeWindow(_GLFWwindow* window)
{
  GgpRemoveMouseHotplugHandler(_glfw.ggp.mouseHotplugHandlerId, NULL);
  GgpRemoveKeyboardHotplugHandler(_glfw.ggp.keyboardHotplugHandlerId, NULL);
  GgpRemoveMouseInputHandler(_glfw.ggp.mouseInputHandlerId, NULL);
  GgpRemoveKeyboardInputHandler(_glfw.ggp.keyboardInputHandlerId, NULL);
  GgpRemoveTextEventHandler(_glfw.ggp.textEventHandlerId);
  if (window->ggp.inputEventQueue != kGgpInvalidReference) {
    GgpEventQueueDestroy_v1(window->ggp.inputEventQueue);
  }
}

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformCreateWindow(_GLFWwindow*          window,
                              const _GLFWwndconfig* wndconfig,
                              const _GLFWctxconfig* ctxconfig,
                              const _GLFWfbconfig*  fbconfig)
{
  if (!createNativeWindow(window, wndconfig)) {
    return GLFW_FALSE;
  }

  window->context.destroy = destroyNativeWindow;
  return GLFW_TRUE;
}

void _glfwPlatformDestroyWindow(_GLFWwindow* window)
{
  if (window->context.destroy) {
    window->context.destroy(window);
  }
}

void _glfwPlatformSetWindowTitle(_GLFWwindow* window, const char* title) {}

void _glfwPlatformSetWindowIcon(_GLFWwindow* window, int count,
                                const GLFWimage* images)
{
}

void _glfwPlatformSetWindowMonitor(_GLFWwindow* window, _GLFWmonitor* monitor,
                                   int xpos, int ypos, int width, int height,
                                   int refreshRate)
{
}

void _glfwPlatformGetWindowPos(_GLFWwindow* window, int* xpos, int* ypos)
{
  if (xpos) {
    *xpos = 0;
  }
  if (ypos) {
    *ypos = 0;
  }
}

void _glfwPlatformSetWindowPos(_GLFWwindow* window, int xpos, int ypos) {}

void _glfwPlatformGetWindowSize(_GLFWwindow* window, int* width, int* height)
{
  if (width) {
    *width = window->ggp.width;
  }
  if (height) {
    *height = window->ggp.height;
  }
}

void _glfwPlatformSetWindowSize(_GLFWwindow* window, int width, int height)
{
  if (window) {
    window->ggp.width  = width;
    window->ggp.height = height;
  }
}

void _glfwPlatformSetWindowSizeLimits(_GLFWwindow* window, int minwidth,
                                      int minheight, int maxwidth,
                                      int maxheight)
{
}

void _glfwPlatformSetWindowAspectRatio(_GLFWwindow* window, int n, int d) {}

void _glfwPlatformGetFramebufferSize(_GLFWwindow* window, int* width,
                                     int* height)
{
  if (width) {
    *width = window->ggp.width;
  }
  if (height) {
    *height = window->ggp.height;
  }
}

void _glfwPlatformGetWindowFrameSize(_GLFWwindow* window, int* left, int* top,
                                     int* right, int* bottom)
{
  if (left) {
    *left = 0;
  }
  if (top) {
    *top = 0;
  }
  if (right) {
    *right = window->ggp.width;
  }
  if (bottom) {
    *bottom = window->ggp.height;
  }
}

void _glfwPlatformIconifyWindow(_GLFWwindow* window) {}

void _glfwPlatformRestoreWindow(_GLFWwindow* window) {}

void _glfwPlatformMaximizeWindow(_GLFWwindow* window) {}

int _glfwPlatformWindowMaximized(_GLFWwindow* window)
{
  return (window != NULL) ? GLFW_TRUE : GLFW_FALSE;
}

void _glfwPlatformSetWindowResizable(_GLFWwindow* window, GLFWbool enabled) {}

void _glfwPlatformSetWindowDecorated(_GLFWwindow* window, GLFWbool enabled) {}

void _glfwPlatformSetWindowFloating(_GLFWwindow* window, GLFWbool enabled) {}

void _glfwPlatformShowWindow(_GLFWwindow* window) {}

void _glfwPlatformUnhideWindow(_GLFWwindow* window) {}

void _glfwPlatformHideWindow(_GLFWwindow* window) {}

void _glfwPlatformFocusWindow(_GLFWwindow* window) {}

int _glfwPlatformWindowFocused(_GLFWwindow* window) { return GLFW_TRUE; }

int _glfwPlatformWindowIconified(_GLFWwindow* window) { return GLFW_FALSE; }

int _glfwPlatformWindowVisible(_GLFWwindow* window)
{
  return (window != NULL) ? GLFW_TRUE : GLFW_FALSE;
}

void _glfwPlatformPollEvents(void)
{
  _GLFWwindow* window = _glfw.windowListHead;
  while (window) {
    while (GgpEventQueueProcessEvent_v1(window->ggp.inputEventQueue, 0)) {
    }  // empty loop

    // Advance to next window
    window = window->next;
  }
}

void _glfwPlatformWaitEvents(void)
{
  // @TODO: Add Me!
}

void _glfwPlatformWaitEventsTimeout(double timeout)
{
  // @TODO: Add Me!
}

void _glfwPlatformPostEmptyEvent(void)
{
  // @TODO: Add Me!
}

void _glfwPlatformGetCursorPos(_GLFWwindow* window, double* xpos, double* ypos)
{
  *xpos = (double)window->ggp.mouse.xpos;
  *ypos = (double)window->ggp.mouse.ypos;
}

void _glfwPlatformSetCursorPos(_GLFWwindow* window, double x, double y)
{
  // @TODO: Add Me!
}

void _glfwPlatformSetCursorMode(_GLFWwindow* window, int mode)
{
  // @TODO: Add Me!
}

int _glfwPlatformCreateCursor(_GLFWcursor* cursor, const GLFWimage* image,
                              int xhot, int yhot)
{
  return GLFW_TRUE;
}

int _glfwPlatformCreateStandardCursor(_GLFWcursor* cursor, int shape)
{
  return GLFW_TRUE;
}

void _glfwPlatformDestroyCursor(_GLFWcursor* cursor)
{
  // @TODO: Add Me!
}

void _glfwPlatformSetCursor(_GLFWwindow* window, _GLFWcursor* cursor)
{
  // @TODO: Add Me!
}

void _glfwPlatformSetClipboardString(_GLFWwindow* window, const char* string)
{
}

const char* _glfwPlatformGetClipboardString(_GLFWwindow* window)
{
  return NULL;
}

const char* _glfwPlatformGetKeyName(int key, int scancode) { return ""; }

int _glfwPlatformGetKeyScancode(int key) { return -1; }

void _glfwPlatformGetRequiredInstanceExtensions(char** extensions)
{
  if (!_glfw.vk.KHR_surface || !_glfw.vk.GGP_stream_descriptor_surface) return;

  extensions[0] = "VK_KHR_surface";
  extensions[1] = "VK_GGP_stream_descriptor_surface";
}

int _glfwPlatformGetPhysicalDevicePresentationSupport(VkInstance instance,
                                                      VkPhysicalDevice device,
                                                      uint32_t queuefamily)
{
  return GLFW_TRUE;
}

VkResult _glfwPlatformCreateWindowSurface(
    VkInstance instance, _GLFWwindow* window,
    const VkAllocationCallbacks* allocator, VkSurfaceKHR* surface)
{
  PFN_vkCreateStreamDescriptorSurfaceGGP vkCreateStreamDescriptorSurfaceGGP;
  vkCreateStreamDescriptorSurfaceGGP =
      (PFN_vkCreateStreamDescriptorSurfaceGGP)vkGetInstanceProcAddr(
          instance, "vkCreateStreamDescriptorSurfaceGGP");
  if (!vkCreateStreamDescriptorSurfaceGGP) {
    _glfwInputError(GLFW_API_UNAVAILABLE,
                    "Ggp: Vulkan instance missing "
                    "VK_GGP_stream_descriptor_surface extension");
    return VK_ERROR_EXTENSION_NOT_PRESENT;
  }

  VkStreamDescriptorSurfaceCreateInfoGGP sci = {
      VK_STRUCTURE_TYPE_STREAM_DESCRIPTOR_SURFACE_CREATE_INFO_GGP};
  sci.streamDescriptor = kGgpPrimaryStreamDescriptor;

  VkResult err =
      vkCreateStreamDescriptorSurfaceGGP(instance, &sci, allocator, surface);
  if (err) {
    _glfwInputError(GLFW_PLATFORM_ERROR,
                    "Ggp: Failed to create Vulkan surface: %s",
                    _glfwGetVulkanResultString(err));
  }

  return err;
}
