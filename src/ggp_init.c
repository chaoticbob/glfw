#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

static void streamStateChanged(const GgpStreamStateChangedEvent* event,
                               void*                             user_data)
{
  // Exit the application if the client disconnects to exit.
  if (event->new_state == kGgpStreamStateChanged_Exited) {
    _GLFWlibrary* glfwLibrary = (_GLFWlibrary*)user_data;

    glfwLibrary->ggp.quit = true;

    // There should only ever be one window for Ggp so it
    // should be safe iter through the windows list.
    _GLFWwindow* window = glfwLibrary->windowListHead;
    while (window) {
      _glfwInputWindowCloseRequest(window);
      window = window->next;
    }
  }
}

static void unregisterCallBack(void* user_data) {}

static void* streamerEventLoopGgp(void* unused)
{
  pthread_setname_np(pthread_self(), "glfwEventLoop");
  while (!_glfw.ggp.quit) {
    GgpEventQueueProcess(_glfw.ggp.streamerEventQueue,
                         /*wait_time_us=*/1000);
  }

  pthread_exit(NULL);
}

static void initializeKeyCodes(short int* keyCodes)
{
  for (uint32_t i = 0; i < kGgpKeyCode_KeyCodeMax; ++i) {
    keyCodes[i] = GLFW_KEY_UNKNOWN;
  }

  keyCodes[kGgpKeyCode_Spacebar]     = GLFW_KEY_SPACE;
  keyCodes[kGgpKeyCode_Quote]        = GLFW_KEY_APOSTROPHE;
  keyCodes[kGgpKeyCode_Comma]        = GLFW_KEY_COMMA;
  keyCodes[kGgpKeyCode_Minus]        = GLFW_KEY_MINUS;
  keyCodes[kGgpKeyCode_Period]       = GLFW_KEY_PERIOD;
  keyCodes[kGgpKeyCode_ForwardSlash] = GLFW_KEY_SLASH;
  keyCodes[kGgpKeyCode_1]            = GLFW_KEY_0;
  keyCodes[kGgpKeyCode_2]            = GLFW_KEY_1;
  keyCodes[kGgpKeyCode_3]            = GLFW_KEY_2;
  keyCodes[kGgpKeyCode_4]            = GLFW_KEY_3;
  keyCodes[kGgpKeyCode_5]            = GLFW_KEY_4;
  keyCodes[kGgpKeyCode_6]            = GLFW_KEY_5;
  keyCodes[kGgpKeyCode_7]            = GLFW_KEY_6;
  keyCodes[kGgpKeyCode_8]            = GLFW_KEY_7;
  keyCodes[kGgpKeyCode_9]            = GLFW_KEY_8;
  keyCodes[kGgpKeyCode_0]            = GLFW_KEY_9;
  keyCodes[kGgpKeyCode_Semicolon]    = GLFW_KEY_SEMICOLON;
  keyCodes[kGgpKeyCode_Equal]        = GLFW_KEY_EQUAL;
  keyCodes[kGgpKeyCode_A]            = GLFW_KEY_A;
  keyCodes[kGgpKeyCode_B]            = GLFW_KEY_B;
  keyCodes[kGgpKeyCode_C]            = GLFW_KEY_C;
  keyCodes[kGgpKeyCode_D]            = GLFW_KEY_D;
  keyCodes[kGgpKeyCode_E]            = GLFW_KEY_E;
  keyCodes[kGgpKeyCode_F]            = GLFW_KEY_F;
  keyCodes[kGgpKeyCode_G]            = GLFW_KEY_G;
  keyCodes[kGgpKeyCode_H]            = GLFW_KEY_H;
  keyCodes[kGgpKeyCode_I]            = GLFW_KEY_I;
  keyCodes[kGgpKeyCode_J]            = GLFW_KEY_J;
  keyCodes[kGgpKeyCode_K]            = GLFW_KEY_K;
  keyCodes[kGgpKeyCode_L]            = GLFW_KEY_L;
  keyCodes[kGgpKeyCode_M]            = GLFW_KEY_M;
  keyCodes[kGgpKeyCode_N]            = GLFW_KEY_N;
  keyCodes[kGgpKeyCode_O]            = GLFW_KEY_O;
  keyCodes[kGgpKeyCode_P]            = GLFW_KEY_P;
  keyCodes[kGgpKeyCode_Q]            = GLFW_KEY_Q;
  keyCodes[kGgpKeyCode_R]            = GLFW_KEY_R;
  keyCodes[kGgpKeyCode_S]            = GLFW_KEY_S;
  keyCodes[kGgpKeyCode_T]            = GLFW_KEY_T;
  keyCodes[kGgpKeyCode_U]            = GLFW_KEY_U;
  keyCodes[kGgpKeyCode_V]            = GLFW_KEY_V;
  keyCodes[kGgpKeyCode_W]            = GLFW_KEY_W;
  keyCodes[kGgpKeyCode_X]            = GLFW_KEY_X;
  keyCodes[kGgpKeyCode_Y]            = GLFW_KEY_Y;
  keyCodes[kGgpKeyCode_Z]            = GLFW_KEY_Z;
  keyCodes[kGgpKeyCode_LeftBrace]    = GLFW_KEY_LEFT_BRACKET;
  keyCodes[kGgpKeyCode_BackSlash]    = GLFW_KEY_BACKSLASH;
  keyCodes[kGgpKeyCode_RightBrace]   = GLFW_KEY_RIGHT_BRACKET;
  keyCodes[kGgpKeyCode_GraveAccent]  = GLFW_KEY_GRAVE_ACCENT;
  keyCodes[kGgpKeyCode_Escape]       = GLFW_KEY_ESCAPE;
  keyCodes[kGgpKeyCode_Enter]        = GLFW_KEY_ENTER;
  keyCodes[kGgpKeyCode_Tab]          = GLFW_KEY_TAB;
  keyCodes[kGgpKeyCode_Backspace]    = GLFW_KEY_BACKSPACE;
  keyCodes[kGgpKeyCode_Insert]       = GLFW_KEY_INSERT;
  keyCodes[kGgpKeyCode_DeleteForward] =
      GLFW_KEY_DELETE;  // NOTE: kGgpKeyCode_Delete is an alias for
                        // kGgpKeyCode_Backspace!
  keyCodes[kGgpKeyCode_Right]          = GLFW_KEY_RIGHT;
  keyCodes[kGgpKeyCode_Left]           = GLFW_KEY_LEFT;
  keyCodes[kGgpKeyCode_Down]           = GLFW_KEY_DOWN;
  keyCodes[kGgpKeyCode_Up]             = GLFW_KEY_UP;
  keyCodes[kGgpKeyCode_PageUp]         = GLFW_KEY_PAGE_UP;
  keyCodes[kGgpKeyCode_PageDown]       = GLFW_KEY_PAGE_DOWN;
  keyCodes[kGgpKeyCode_Home]           = GLFW_KEY_HOME;
  keyCodes[kGgpKeyCode_End]            = GLFW_KEY_END;
  keyCodes[kGgpKeyCode_CapsLock]       = GLFW_KEY_CAPS_LOCK;
  keyCodes[kGgpKeyCode_ScrollLock]     = GLFW_KEY_SCROLL_LOCK;
  keyCodes[kGgpKeyCode_KeypadNumLock]  = GLFW_KEY_NUM_LOCK;
  keyCodes[kGgpKeyCode_PrintScreen]    = GLFW_KEY_PRINT_SCREEN;
  keyCodes[kGgpKeyCode_Pause]          = GLFW_KEY_PAUSE;
  keyCodes[kGgpKeyCode_F1]             = GLFW_KEY_F1;
  keyCodes[kGgpKeyCode_F2]             = GLFW_KEY_F2;
  keyCodes[kGgpKeyCode_F3]             = GLFW_KEY_F3;
  keyCodes[kGgpKeyCode_F4]             = GLFW_KEY_F4;
  keyCodes[kGgpKeyCode_F5]             = GLFW_KEY_F5;
  keyCodes[kGgpKeyCode_F6]             = GLFW_KEY_F6;
  keyCodes[kGgpKeyCode_F7]             = GLFW_KEY_F7;
  keyCodes[kGgpKeyCode_F8]             = GLFW_KEY_F8;
  keyCodes[kGgpKeyCode_F9]             = GLFW_KEY_F9;
  keyCodes[kGgpKeyCode_F10]            = GLFW_KEY_F10;
  keyCodes[kGgpKeyCode_F11]            = GLFW_KEY_F11;
  keyCodes[kGgpKeyCode_F12]            = GLFW_KEY_F12;
  keyCodes[kGgpKeyCode_F13]            = GLFW_KEY_F13;
  keyCodes[kGgpKeyCode_F14]            = GLFW_KEY_F14;
  keyCodes[kGgpKeyCode_F15]            = GLFW_KEY_F15;
  keyCodes[kGgpKeyCode_F16]            = GLFW_KEY_F16;
  keyCodes[kGgpKeyCode_F17]            = GLFW_KEY_F17;
  keyCodes[kGgpKeyCode_F18]            = GLFW_KEY_F18;
  keyCodes[kGgpKeyCode_F19]            = GLFW_KEY_F19;
  keyCodes[kGgpKeyCode_F20]            = GLFW_KEY_F20;
  keyCodes[kGgpKeyCode_F21]            = GLFW_KEY_F21;
  keyCodes[kGgpKeyCode_F22]            = GLFW_KEY_F22;
  keyCodes[kGgpKeyCode_F23]            = GLFW_KEY_F23;
  keyCodes[kGgpKeyCode_F24]            = GLFW_KEY_F24;
  keyCodes[kGgpKeyCode_Keypad0]        = GLFW_KEY_KP_0;
  keyCodes[kGgpKeyCode_Keypad1]        = GLFW_KEY_KP_1;
  keyCodes[kGgpKeyCode_Keypad2]        = GLFW_KEY_KP_2;
  keyCodes[kGgpKeyCode_Keypad3]        = GLFW_KEY_KP_3;
  keyCodes[kGgpKeyCode_Keypad4]        = GLFW_KEY_KP_4;
  keyCodes[kGgpKeyCode_Keypad5]        = GLFW_KEY_KP_5;
  keyCodes[kGgpKeyCode_Keypad6]        = GLFW_KEY_KP_6;
  keyCodes[kGgpKeyCode_Keypad7]        = GLFW_KEY_KP_7;
  keyCodes[kGgpKeyCode_Keypad8]        = GLFW_KEY_KP_8;
  keyCodes[kGgpKeyCode_Keypad9]        = GLFW_KEY_KP_9;
  keyCodes[kGgpKeyCode_KeypadPeriod]   = GLFW_KEY_KP_DECIMAL;
  keyCodes[kGgpKeyCode_KeypadDivide]   = GLFW_KEY_KP_DIVIDE;
  keyCodes[kGgpKeyCode_KeypadMultiply] = GLFW_KEY_KP_MULTIPLY;
  keyCodes[kGgpKeyCode_KeypadSubtract] = GLFW_KEY_KP_SUBTRACT;
  keyCodes[kGgpKeyCode_KeypadAdd]      = GLFW_KEY_KP_ADD;
  keyCodes[kGgpKeyCode_KeypadEnter]    = GLFW_KEY_KP_ENTER;
  keyCodes[kGgpKeyCode_KeypadEqual]    = GLFW_KEY_KP_EQUAL;
  keyCodes[kGgpKeyCode_LeftShift]      = GLFW_KEY_LEFT_SHIFT;
  keyCodes[kGgpKeyCode_LeftControl]    = GLFW_KEY_LEFT_CONTROL;
  keyCodes[kGgpKeyCode_LeftAlt]        = GLFW_KEY_LEFT_ALT;
  keyCodes[kGgpKeyCode_RightShift]     = GLFW_KEY_RIGHT_SHIFT;
  keyCodes[kGgpKeyCode_RightControl]   = GLFW_KEY_RIGHT_CONTROL;
  keyCodes[kGgpKeyCode_RightAlt]       = GLFW_KEY_RIGHT_ALT;
  keyCodes[kGgpKeyCode_Menu]           = GLFW_KEY_MENU;
}

//////////////////////////////////////////////////////////////////////////
//////                       GLFW platform API                      //////
//////////////////////////////////////////////////////////////////////////

int _glfwPlatformInit(void)
{
  memset(&(_glfw.ggp), 0, sizeof(_glfw.ggp));

  initializeKeyCodes(_glfw.ggp.keyCodes);

  GgpInitialize(NULL);

  _glfw.ggp.streamerEventQueue = GgpEventQueueCreate();

  GgpStatus status;
  _glfw.ggp.streamStateChangedHandle = GgpAddStreamStateChangedHandler(
      _glfw.ggp.streamerEventQueue, streamStateChanged, &_glfw,
      unregisterCallBack, &status);
  if (status.status_code != kGgpStatusCode_Ok) {
    exit(EXIT_FAILURE);
  }

  _glfw.ggp.quit = false;

  // Create thread for streamer events
  {
    int threadCreateResult = pthread_create(&(_glfw.ggp.streamerThreadId),
                                            NULL, streamerEventLoopGgp, NULL);
    if (threadCreateResult != 0) {
      exit(EXIT_FAILURE);
    }
  }

  _glfwInitTimerPOSIX();

  return GLFW_TRUE;
}

void _glfwPlatformTerminate(void)
{
  GgpStopStream();

  // Wait for a StreamStoppedEvent to be received.
  if (_glfw.ggp.streamerThreadId != 0) {
    void* threadResult = NULL;
    pthread_join(_glfw.ggp.streamerThreadId, &threadResult);
  }

  GgpRemoveStreamStateChangedHandler(_glfw.ggp.streamStateChangedHandle, NULL);

  GgpEventQueueDestroy(_glfw.ggp.streamerEventQueue, NULL);
}

const char* _glfwPlatformGetVersionString(void)
{
  return _GLFW_VERSION_NUMBER " Ggp";
}
