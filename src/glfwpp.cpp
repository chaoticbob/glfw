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
Window::Window(int width, int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share)
  : m_width(width), m_height(height)
{
  m_glfw_window = glfwCreateWindow(width, height, title, monitor, share);
  s_event_manager.RegisterWindow(m_glfw_window, this);

  glfwMakeContextCurrent(m_glfw_window);
}

Window::~Window()
{
  glfwDestroyWindow(m_glfw_window);
  s_event_manager.UnregisterWindow(m_glfw_window);
  m_glfw_window = nullptr;
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

int Window::GetWidth() const
{
  return m_width;
}

int Window::GetHeight() const
{
  return m_height;
}

int Window::WindowShouldClose()
{
  return glfwWindowShouldClose(m_glfw_window);
}

void Window::SetWindowShouldClose(int value)
{
  glfwSetWindowShouldClose(m_glfw_window, value);
}

void Window::SetWindowTitle(const char* title)
{
  glfwSetWindowTitle(m_glfw_window, title);
}

void Window::SetWindowIcon(int count, const GLFWimage* images)
{
  glfwSetWindowIcon(m_glfw_window, count, images);
}

void Window::GetWindowPos(int* xpos, int* ypos)
{
  glfwGetWindowPos(m_glfw_window, xpos, ypos);
}

void Window::SetWindowPos(int xpos, int ypos)
{
  glfwSetWindowPos(m_glfw_window, xpos, ypos);
}

void Window::GetWindowSize(int* width, int* height)
{
  glfwGetWindowSize(m_glfw_window, width, height);
}

void Window::SetWindowSizeLimits(int minwidth, int minheight, int maxwidth, int maxheight)
{
  glfwSetWindowSizeLimits(m_glfw_window, minwidth, minheight, maxwidth, maxheight);
}

void Window::SetWindowAspectRatio(int numer, int denom)
{
  glfwSetWindowAspectRatio(m_glfw_window, numer, denom);
}

void Window::SetWindowSize(int width, int height)
{
  m_width = width;
  m_height = height;
  glfwSetWindowSize(m_glfw_window, width, height);
}

void Window::GetFramebufferSize(int* width, int* height)
{
  glfwGetFramebufferSize(m_glfw_window, width, height);
}

void Window::GetWindowFrameSize(int* left, int* top, int* right, int* bottom)
{
  glfwGetWindowFrameSize(m_glfw_window, left, top, right, bottom);
}

void Window::IconifyWindow()
{
  glfwIconifyWindow(m_glfw_window);
}

void Window::RestoreWindow()
{
  glfwRestoreWindow(m_glfw_window);
}

void Window::MaximizeWindow()
{
  glfwMaximizeWindow(m_glfw_window);
}

void Window::ShowWindow()
{
  glfwShowWindow(m_glfw_window);
}

void Window::HideWindow()
{
  glfwHideWindow(m_glfw_window);
}

void Window::FocusWindow()
{
  glfwFocusWindow(m_glfw_window);
}

GLFWmonitor* Window::GetWindowMonitor()
{
  return glfwGetWindowMonitor(m_glfw_window);
}

void Window::SetWindowMonitor(GLFWmonitor* monitor, int xpos, int ypos, int width, int height, int refreshRate)
{
  glfwSetWindowMonitor(m_glfw_window, monitor, xpos, ypos, width, height, refreshRate);
}

int Window::GetWindowAttrib(int attrib)
{
  return glfwGetWindowAttrib(m_glfw_window, attrib);
}

void Window::SetWindowAttrib(int attrib, int value)
{
  glfwSetWindowAttrib(m_glfw_window, attrib, value);
}

void Window::SetWindowUserPointer(void* pointer)
{
    return glfwSetWindowUserPointer(m_glfw_window, pointer);
}

void* Window::GetWindowUserPointer()
{
  return glfwGetWindowUserPointer(m_glfw_window);
}

int Window::GetInputMode(int mode)
{
  return glfwGetInputMode(m_glfw_window, mode);
}

void Window::SetInputMode(int mode, int value)
{
  glfwSetInputMode(m_glfw_window, mode, value);
}

int Window::GetKey(int key)
{
  return glfwGetKey(m_glfw_window, key);
}

int Window::GetMouseButton(int button)
{
  return glfwGetMouseButton(m_glfw_window, button);
}

void Window::GetCursorPos(double* xpos, double* ypos)
{
  glfwGetCursorPos(m_glfw_window, xpos, ypos);
}

void Window::SetCursorPos(double xpos, double ypos)
{
  glfwSetCursorPos(m_glfw_window, xpos, ypos);
}

void Window::SetCursor(GLFWcursor* cursor)
{
  glfwSetCursor(m_glfw_window, cursor);
}

void Window::SetClipboardString(const char* string)
{
  glfwSetClipboardString(m_glfw_window, string);
}

const char* Window::GetClipboardString()
{
  return glfwGetClipboardString(m_glfw_window);
}

void Window::MakeContextCurrent() const
{
  glfwMakeContextCurrent(m_glfw_window);
}

void Window::SwapBuffers() const
{
  glfwSwapBuffers(m_glfw_window);
}

void Window::SwapInterval(int interval) const
{
  glfwSwapInterval(interval);
}

// =============================================================================
// Application
// =============================================================================
Application::Application(bool owns_terminate)
  : m_owns_terminate(owns_terminate)
{
  if (m_owns_terminate) {
    if (! glfwInit()) {
      exit(EXIT_FAILURE);
    }

    glfwSetTime(0.0);
  }
}

Application::Application(glfw::Window* window, bool owns_terminate)
  : m_owns_terminate(owns_terminate)
{
  AddWindow(window);
  m_current_window = m_windows.back();
}

Application::Application(std::unique_ptr<glfw::Window> window, bool owns_terminate)
  : m_owns_terminate(owns_terminate)
{
  AddWindow(window);
  m_current_window = m_windows.back();
}

Application::~Application()
{
  if (m_owns_terminate) {
    glfwTerminate();
  }
}

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

void Application::AddWindow(std::unique_ptr<glfw::Window>& window)
{
  auto it = std::find_if(std::begin(m_windows),
                         std::end(m_windows),
                         [&window](const glfw::Window* elem) -> bool {
                             return window.get() == elem; });

  if (it == std::end(m_windows)) {
    m_windows.push_back(window.get());
    m_owned_windows.push_back(std::move(window));
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

glfw::Window* Application::GetCurrentWindow() const
{
  return m_current_window;
}

bool Application::GetAutoPollEvents() const
{
  return m_auto_poll_events;
}

void Application::SetAutoPollEvents(bool value)
{
  m_auto_poll_events = value;
}

void Application::PollEvents()
{
  glfwPollEvents();
}

int Application::Execute()
{
  m_running = true;
  while (m_running) {
    EventLoop();

    if (m_auto_poll_events && m_running) {
      PollEvents();
    }

    if (m_windows.empty() && m_exit_on_last_window_closed) {
      m_running = false;
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