#include "../include/GLFW/glfwpp.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <mutex>
#include <thread>
#include <vector>

namespace glfw {

// =============================================================================
// EventManager
// =============================================================================
class EventManager {
public:
  struct WindowPair {
    GLFWwindow*   m_glfw_window = nullptr;
    glfw::Window* m_window = nullptr;
    WindowPair(GLFWwindow* glfw_window, glfw::Window* window)
      : m_glfw_window(glfw_window), m_window(window) {}
  };

  void RegisterWindow(GLFWwindow* glfw_window, glfw::Window* window)
  {
    std::lock_guard<std::mutex> lock(m_window_mutex);
  
    auto it = std::find_if(std::begin(m_windows),
                           std::end(m_windows),
                           [glfw_window](const WindowPair& elem) -> bool {
                               return glfw_window == elem.m_glfw_window; });
    if (it == std::end(m_windows)) {
      m_windows.push_back(WindowPair(glfw_window, window));

      glfwSetWindowPosCallback(glfw_window, EventManager::WindowPosCallback);
      glfwSetWindowSizeCallback(glfw_window, EventManager::WindowSizeCallback);
      glfwSetWindowCloseCallback(glfw_window, EventManager::WindowCloseCallback);
      glfwSetWindowRefreshCallback(glfw_window, EventManager::WindowRefreshCallback);
      glfwSetWindowFocusCallback(glfw_window, EventManager::WindowFocusCallback);
      glfwSetWindowIconifyCallback(glfw_window, EventManager::WindowIconifyCallback);
      glfwSetWindowMaximizeCallback(glfw_window, EventManager::WindowMaximizeCallback);
      glfwSetFramebufferSizeCallback(glfw_window, EventManager::FramebufferSizeCallback);
      
      glfwSetKeyCallback(glfw_window, EventManager::KeyCallback);
      glfwSetCharCallback(glfw_window, EventManager::CharCallback);
      glfwSetCharModsCallback(glfw_window, EventManager::CharModsCallback);
      glfwSetMouseButtonCallback(glfw_window, EventManager::MouseButtonCallback);
      glfwSetCursorPosCallback(glfw_window, EventManager::CursorPosCallback);
      glfwSetCursorEnterCallback(glfw_window, EventManager::CursorEnterCallback);
      glfwSetScrollCallback(glfw_window, EventManager::ScrollCallback);
      glfwSetDropCallback(glfw_window, EventManager::DropCallback);
    }
  }

  void UnregisterWindow(GLFWwindow* glfw_window)
  {
    std::lock_guard<std::mutex> lock(m_window_mutex);

    m_windows.erase(std::remove_if(std::begin(m_windows), 
                                   std::end(m_windows), 
                                   [glfw_window](const WindowPair& elem) -> bool {
                                       return glfw_window == elem.m_glfw_window; }),
                    m_windows.end());
  }

  glfw::Window* FindWindow(GLFWwindow* glfw_window) {
    std::lock_guard<std::mutex> lock(m_window_mutex);
  
    auto it = std::find_if(std::begin(m_windows),
                           std::end(m_windows),
                           [glfw_window](const WindowPair& elem) -> bool {
                               return glfw_window == elem.m_glfw_window; });
      
    glfw::Window* result = (it != std::end(m_windows)) ? it->m_window : nullptr;
    return result;
  }

  static void   WindowPosCallback(GLFWwindow* glfw_window, int xpos, int ypos);
  static void   WindowSizeCallback(GLFWwindow* glfw_window, int width, int height);
  static void   WindowCloseCallback(GLFWwindow* glfw_window);
  static void   WindowRefreshCallback(GLFWwindow* glfw_window);
  static void   WindowFocusCallback(GLFWwindow* glfw_window, int focused);
  static void   WindowIconifyCallback(GLFWwindow* glfw_window, int iconified);
  static void   WindowMaximizeCallback(GLFWwindow* glfw_window, int iconified);
  static void   FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height);

  static void   KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods);
  static void   CharCallback(GLFWwindow* glfw_window, unsigned int codepoint);
  static void   CharModsCallback(GLFWwindow* glfw_window, unsigned int codepoint, int mods);
  static void   MouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods);
  static void   CursorPosCallback(GLFWwindow* glfw_window, double xpos, double ypos);
  static void   CursorEnterCallback(GLFWwindow* glfw_window, int entered);
  static void   ScrollCallback(GLFWwindow* glfw_window, double xoffset, double yoffset);
  static void   DropCallback(GLFWwindow* glfw_window, int count, const char** paths);

private:
  std::mutex              m_window_mutex;
  std::vector<WindowPair> m_windows;  
};

EventManager s_event_manager;

void EventManager::WindowPosCallback(GLFWwindow* glfw_window, int xpos, int ypos)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowPos(xpos, ypos);
  }
}

void EventManager::WindowSizeCallback(GLFWwindow* glfw_window, int width, int height)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->HandleWindowSize(width, height);
  }
}

void EventManager::WindowCloseCallback(GLFWwindow* glfw_window)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowClose();
  }
}

void EventManager::WindowRefreshCallback(GLFWwindow* glfw_window)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowRefresh();
  }
}

void EventManager::WindowFocusCallback(GLFWwindow* glfw_window, int focused)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowFocus(focused);
  }
}

void EventManager::WindowIconifyCallback(GLFWwindow* glfw_window, int iconified)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowIconify(iconified);
  }
}

void EventManager::WindowMaximizeCallback(GLFWwindow* glfw_window, int iconified)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->WindowMaximize(iconified);
  }
}

void EventManager::FramebufferSizeCallback(GLFWwindow* glfw_window, int width, int height)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->FramebufferSize(width, height);
  }
}

void EventManager::KeyCallback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->Key(key, scancode, action, mods);
  }
}

void EventManager::CharCallback(GLFWwindow* glfw_window, unsigned int codepoint)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->Char(codepoint);
  }
}

void EventManager::CharModsCallback(GLFWwindow* glfw_window, unsigned int codepoint, int mods)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->CharMods(codepoint, mods);
  }
}

void EventManager::MouseButtonCallback(GLFWwindow* glfw_window, int button, int action, int mods)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->MouseButton(button, action, mods);
  }
}

void EventManager::CursorPosCallback(GLFWwindow* glfw_window, double xpos, double ypos)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->CursorPos(xpos, ypos);
  }
}

void EventManager::CursorEnterCallback(GLFWwindow* glfw_window, int entered)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->CursorEnter(entered);
  }
}

void EventManager::ScrollCallback(GLFWwindow* glfw_window, double xoffset, double yoffset)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->Scroll(xoffset, yoffset);
  }
}

void EventManager::DropCallback(GLFWwindow* glfw_window, int count, const char** paths)
{
  glfw::Window* window = s_event_manager.FindWindow(glfw_window);
  if (window != nullptr) {
    window->Drop(count, paths);
  }
}

// =============================================================================
// Window
// =============================================================================
Window::Window(int width, int height, const std::string& title, GLFWmonitor* monitor, GLFWwindow* share)
  : m_width(width), m_height(height)
{
  if (! glfwInit()) {
    exit(EXIT_FAILURE);
  }

  m_glfw_window = glfwCreateWindow(width, height, title.c_str(), monitor, share);
  s_event_manager.RegisterWindow(m_glfw_window, this);

  glfwMakeContextCurrent(m_glfw_window);
}

Window::~Window()
{
  Close();
}

void Window::SetApplication(glfw::Application* application)
{
  m_application = application;
}

void Window::HandleWindowSize(int width, int height)
{
  m_width = width;
  m_height = height;
  WindowSize(width, height);
}

GLFWwindow* Window::GetGLFWwindow() const
{
  return m_glfw_window;
}

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
HWND Window::GetNativeWindowHandle() const
{
  HWND result = glfwGetWin32Window(m_glfw_window);
  return result;
}
#endif

int Window::GetWidth() const
{
  return m_width;
}

int Window::GetHeight() const
{
  return m_height;
}

bool Window::IsValid() const
{
  return m_glfw_window != nullptr;
}

void Window::Close()
{
  if (m_glfw_window != nullptr) {
    glfwDestroyWindow(m_glfw_window);
    s_event_manager.UnregisterWindow(m_glfw_window);
    m_glfw_window = nullptr;
    m_width = 0;
    m_height = 0;
  }
}

int Window::WindowShouldClose()
{
  if (! IsValid()) {
    return GLFW_TRUE;
  }

  return glfwWindowShouldClose(m_glfw_window);
}

void Window::SetWindowShouldClose(int value)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowShouldClose(m_glfw_window, value);
}

void Window::SetWindowTitle(const char* title)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowTitle(m_glfw_window, title);
}

void Window::SetWindowIcon(int count, const GLFWimage* images)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowIcon(m_glfw_window, count, images);
}

void Window::GetWindowPos(int* xpos, int* ypos)
{
  if (! IsValid()) {
    return;
  }

  glfwGetWindowPos(m_glfw_window, xpos, ypos);
}

void Window::SetWindowPos(int xpos, int ypos)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowPos(m_glfw_window, xpos, ypos);
}

void Window::GetWindowSize(int* width, int* height)
{
  if (! IsValid()) {
    return;
  }

  glfwGetWindowSize(m_glfw_window, width, height);
}

void Window::SetWindowSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowSizeLimits(m_glfw_window, minwidth, minheight, maxwidth, maxheight);
}

void Window::SetWindowAspectRatio(int numer, int denom)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowAspectRatio(m_glfw_window, numer, denom);
}

void Window::SetWindowSize(int width, int height)
{
  if (! IsValid()) {
    return;
  }

  m_width = width;
  m_height = height;
  glfwSetWindowSize(m_glfw_window, width, height);
}

void Window::GetFramebufferSize(int* width, int* height)
{
  if (! IsValid()) {
    return;
  }

  glfwGetFramebufferSize(m_glfw_window, width, height);
}

void Window::GetWindowFrameSize(int* left, int* top, int* right, int* bottom)
{
  if (! IsValid()) {
    return;
  }

  glfwGetWindowFrameSize(m_glfw_window, left, top, right, bottom);
}

void Window::IconifyWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwIconifyWindow(m_glfw_window);
}

void Window::RestoreWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwRestoreWindow(m_glfw_window);
}

void Window::MaximizeWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwMaximizeWindow(m_glfw_window);
}

void Window::ShowWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwShowWindow(m_glfw_window);
}

void Window::HideWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwHideWindow(m_glfw_window);
}

void Window::FocusWindow()
{
  if (! IsValid()) {
    return;
  }

  glfwFocusWindow(m_glfw_window);
}

GLFWmonitor* Window::GetWindowMonitor()
{
  if (! IsValid()) {
    return nullptr;
  }

  return glfwGetWindowMonitor(m_glfw_window);
}

void Window::SetWindowMonitor(GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refresh_rate)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowMonitor(m_glfw_window, monitor, xpos, ypos, width, height, refresh_rate);
}

int Window::GetWindowAttrib(int attrib)
{
  return glfwGetWindowAttrib(m_glfw_window, attrib);
}

void Window::SetWindowAttrib(int attrib, int value)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowAttrib(m_glfw_window, attrib, value);
}

void Window::SetWindowUserPointer(void* pointer)
{
  if (! IsValid()) {
    return;
  }

  glfwSetWindowUserPointer(m_glfw_window, pointer);
}

void* Window::GetWindowUserPointer()
{
  if (! IsValid()) {
    return nullptr;
  }

  return glfwGetWindowUserPointer(m_glfw_window);
}

void Window::PollEvents()
{
  if (! IsValid()) {
    return;
  }

  MakeContextCurrent();
  glfwPollEvents();
}

void Window::WaitEvents()
{
  if (! IsValid()) {
    return;
  }

  MakeContextCurrent();
  glfwWaitEvents();
}

void Window::WaitEventsTimeout(double timeout)
{
  if (! IsValid()) {
    return;
  }

  MakeContextCurrent();
  glfwWaitEventsTimeout(timeout);
}

void Window::PostEmptyEvent()
{
  if (! IsValid()) {
    return;
  }

  MakeContextCurrent();
  glfwPostEmptyEvent();
}

int Window::GetInputMode(int mode)
{
  if (! IsValid()) {
    return GLFW_FALSE;
  }

  return glfwGetInputMode(m_glfw_window, mode);
}

void Window::SetInputMode(int mode, int value)
{
  if (! IsValid()) {
    return;
  }

  glfwSetInputMode(m_glfw_window, mode, value);
}

int Window::GetKey(int key)
{
  if (! IsValid()) {
    return GLFW_KEY_UNKNOWN;
  }

  return glfwGetKey(m_glfw_window, key);
}

int Window::GetMouseButton(int button)
{
  if (! IsValid()) {
    return GLFW_MOUSE_BUTTON_UNKNOWN;
  }

  return glfwGetMouseButton(m_glfw_window, button);
}

void Window::GetCursorPos(double* xpos, double* ypos)
{
  if (! IsValid()) {
    return;
  }

  glfwGetCursorPos(m_glfw_window, xpos, ypos);
}

void Window::SetCursorPos(double xpos, double ypos)
{
  if (! IsValid()) {
    return;
  }

  glfwSetCursorPos(m_glfw_window, xpos, ypos);
}

void Window::SetCursor(GLFWcursor* cursor)
{
  if (! IsValid()) {
    return;
  }

  glfwSetCursor(m_glfw_window, cursor);
}

void Window::SetClipboardString(const char* string)
{
  if (! IsValid()) {
    return;
  }

  glfwSetClipboardString(m_glfw_window, string);
}

const char* Window::GetClipboardString()
{
  if (! IsValid()) {
    return nullptr;
  }

  return glfwGetClipboardString(m_glfw_window);
}

void Window::MakeContextCurrent() const
{
  if (! IsValid()) {
    return;
  }

  glfwMakeContextCurrent(m_glfw_window);
}

void Window::SwapBuffers() const
{
  if (! IsValid()) {
    return;
  }

  MakeContextCurrent();
  glfwSwapBuffers(m_glfw_window);
}

void Window::SwapInterval(int interval) const
{
  if (! IsValid()) {
    return;
  }

  glfwSwapInterval(interval);
}

// =============================================================================
// Application
// =============================================================================
Application::Application(bool owns_terminate)
  : m_owns_terminate(owns_terminate)
{
  if (! glfwInit()) {
    exit(EXIT_FAILURE);
  }

  glfwSetTime(0.0);
}

Application::Application(glfw::Window* window, bool owns_terminate)
  : m_owns_terminate(owns_terminate)
{
  AddWindow(window);
}

Application::~Application()
{
  for (auto& window : m_windows) { 
    window->Close();
    delete window;
    window = nullptr;
  }
  m_windows.clear();

  if (m_owns_terminate) {
    glfwTerminate();
  }
}

#if defined(GLFW_EXPOSE_NATIVE_WIN32)
HINSTANCE Application::GetNativeHandle()
{
  HINSTANCE result = ::GetModuleHandle(nullptr);
  return result;
}
#endif

void Application::AddWindow(glfw::Window* window)
{
  auto it = std::find_if(std::begin(m_windows),
                         std::end(m_windows),
                         [window](const glfw::Window* elem) -> bool {
                             return window == elem; });

  if (it == std::end(m_windows)) {
    m_windows.push_back(window);
  }
}

void Application::RemoveWindow(glfw::Window* window)
{
  m_windows.erase(std::remove_if(std::begin(m_windows), 
                                  std::end(m_windows), 
                                  [window](const glfw::Window* elem) -> bool {
                                      return window == elem; }),
                  m_windows.end());
}

bool Application::GetAutoPollEvents() const
{
  return m_auto_poll_events;
}

void Application::SetAutoPollEvents(bool value)
{
  m_auto_poll_events = value;
}

int Application::Execute()
{
  m_running = true;
  while (m_running) {
    EventLoop();

    if (! m_running) {
      break;
    }

    // Close or process events
    bool has_close = false;
    for (auto& window : m_windows) {
      if (window->WindowShouldClose()) {
        window->Close();
        has_close = true;
      }
      else {
        if (m_auto_poll_events) {
          window->PollEvents();
        }
      }
    }

    if (m_exit_on_last_window_closed) {
      bool all_closed = true;
      for (auto& window : m_windows) {
        if (window->IsValid()) {
          all_closed = false;
          break;
        }
      }
      m_running = all_closed ? false : true;
    }
  }

  return m_exit_code;
}

void Application::Exit(int exit_code)
{
  m_running = false;
  m_exit_code = exit_code;
}

} // namespace glfw