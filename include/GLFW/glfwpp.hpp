#ifndef _glfwpp_hpp_
#define _glfwpp_hpp_

#if ! defined(__cplusplus)
 #error "glfwpp requires C++"
#endif

#include "glfw3.h"

#include <memory>
#include <vector>

namespace glfw {

class Application;
class EventManager;

//! \class Window
//!
//!
class Window {
public:
  Window(int width, int height, const char* title, GLFWmonitor* monitor = nullptr, GLFWwindow* share = nullptr);
  virtual ~Window();

  GLFWwindow*   GetGLFWwindow() const;
  int           GetWidth() const;
  int           GetHeight() const;

  int           WindowShouldClose();
  void          SetWindowShouldClose(int value);
  void          SetWindowTitle(const char* title);
  void          SetWindowIcon(int count, const GLFWimage* images);
  void          GetWindowPos(int* xpos, int* ypos);
  void          SetWindowPos(int xpos, int ypos);
  void          GetWindowSize(int* width, int* height);
  void          SetWindowSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight);
  void          SetWindowAspectRatio(int numer, int denom);
  void          SetWindowSize(int width, int height);
  void          GetFramebufferSize(int* width, int* height);
  void          GetWindowFrameSize(int* left, int* top, int* right, int* bottom);
  void          IconifyWindow();
  void          RestoreWindow();
  void          MaximizeWindow();
  void          ShowWindow();
  void          HideWindow();
  void          FocusWindow();
  GLFWmonitor*  GetWindowMonitor();
  void          SetWindowMonitor(GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate);
  int           GetWindowAttrib(int attrib);
  void          SetWindowAttrib(int attrib, int value);
  void          SetWindowUserPointer(void* pointer);
  void*         GetWindowUserPointer();

  virtual void  WindowPos(int xpos, int ypos) {}
  virtual void  WindowSize(int width, int height) {}
  virtual void  WindowClose() {}
  virtual void  WindowRefresh() {}
  virtual void  WindowFocus(int focused) {}
  virtual void  WindowIconify(int iconified) {}
  virtual void  WindowMaximize(int iconified) {}
  virtual void  FramebufferSize(int width, int height) {}

  int           GetInputMode(int mode);
  void          SetInputMode(int mode, int value);
  int           GetKey(int key);
  int           GetMouseButton(int button);
  void          GetCursorPos(double* xpos, double* ypos);
  void          SetCursorPos(double xpos, double ypos);
  void          SetCursor(GLFWcursor* cursor);
  void          SetClipboardString(const char* string);
  const char*   GetClipboardString();

  virtual void  Key(int key, int scancode, int action, int mods) {}
  virtual void  Char(unsigned int codepoint) {}
  virtual void  CharMods(unsigned int codepoint, int mods) {}
  virtual void  MouseButton(int button, int action, int mods) {}
  virtual void  CursorPos(double xpos, double ypos) {}
  virtual void  CursorEnter(int entered) {}
  virtual void  Scroll(double xoffset, double yoffset) {}
  virtual void  Drop(int count, const char** paths) {}

  void          MakeContextCurrent() const;
  void          SwapBuffers() const;
  void          SwapInterval(int interval) const;

private:
  void SetApplication(glfw::Application* application);
  void HandleWindowSize(int width, int height);

private:
  glfw::Application*  m_application = nullptr;
  int                 m_width = 0;
  int                 m_height = 0;
  GLFWwindow*         m_glfw_window = nullptr;
  friend class glfw::Application;
  friend class glfw::EventManager;
};

//! \class Application
//!
//!
class Application {
public:
  Application(bool owns_terminate = true);
  Application(glfw::Window* window, bool owns_terminate = false);
  Application(std::unique_ptr<glfw::Window> window, bool owns_terminate = true);
  virtual ~Application();

  void          AddWindow(glfw::Window* window);
  void          AddWindow(std::unique_ptr<glfw::Window>& window);
  void          RemoveWindow(glfw::Window* window);
  glfw::Window* GetCurrentWindow() const;
  
  bool          GetAutoPollEvents() const;
  void          SetAutoPollEvents(bool value);
  void          PollEvents();

  virtual void  Execute();
  virtual void  Exit();
  virtual void  EventLoop() {}

private:
  bool m_running = false;
  bool m_exit_on_last_window_closed = true;
  bool m_owns_terminate = false;
  bool m_auto_poll_events = true;

  std::vector<glfw::Window*>                  m_windows;
  glfw::Window*                               m_current_window = nullptr;
  std::vector<std::unique_ptr<glfw::Window>>  m_owned_windows;
};

} // namespace glfw

#endif // _glfwpp_hpp_