#include "lockdownmanager.h"
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QTimer>

#ifdef PLATFORM_WINDOWS
  #include <windows.h>
  #include <winuser.h>

  // Low-level keyboard hook to block Alt+Tab, Win key, Alt+F4 etc.
  static HHOOK g_kbHook = nullptr;
  static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
      if (nCode == HC_ACTION) {
          KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
          // Block Windows keys
          if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN) return 1;
          // Block Alt+Tab
          if (p->vkCode == VK_TAB && (GetAsyncKeyState(VK_MENU) & 0x8000)) return 1;
          // Block Alt+F4
          if (p->vkCode == VK_F4  && (GetAsyncKeyState(VK_MENU) & 0x8000)) return 1;
          // Block Ctrl+Esc (Start menu)
          if (p->vkCode == VK_ESCAPE && (GetAsyncKeyState(VK_CONTROL) & 0x8000)) return 1;
          // Block PrintScreen
          if (p->vkCode == VK_SNAPSHOT) return 1;
      }
      return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
  }
#endif

#ifdef PLATFORM_LINUX
  #ifdef HAVE_X11
    #include <X11/Xlib.h>
    #include <X11/keysym.h>
  #endif
#endif

LockdownManager::LockdownManager(QObject* parent)
    : QObject(parent)
{
    m_focusTimer  = new QTimer(this);
    m_windowTimer = new QTimer(this);

    connect(m_focusTimer,  &QTimer::timeout, this, &LockdownManager::onFocusCheck);
    connect(m_windowTimer, &QTimer::timeout, this, &LockdownManager::onWindowCheck);
}

LockdownManager::~LockdownManager() {
    disengage();
}

void LockdownManager::engage(QWidget* mainWindow) {
    if (m_engaged) return;
    m_mainWindow = mainWindow;
    m_engaged    = true;
    m_violationCount = 0;

    applyPlatformLockdown(mainWindow);

    // Show fullscreen, topmost, no frame
    mainWindow->setWindowFlags(
        Qt::Window |
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint
    );
    mainWindow->showFullScreen();
    mainWindow->raise();
    mainWindow->activateWindow();

    // Poll every 500ms for focus violations
    m_focusTimer->start(500);
    // Poll every 1s to ensure window is still topmost
    m_windowTimer->start(1000);
}

void LockdownManager::disengage() {
    if (!m_engaged) return;
    m_engaged = false;

    m_focusTimer->stop();
    m_windowTimer->stop();

    removePlatformLockdown();

    if (m_mainWindow) {
        m_mainWindow->setWindowFlags(Qt::Window);
        m_mainWindow->showNormal();
    }
}

void LockdownManager::onFocusCheck() {
    if (!m_engaged || !m_mainWindow) return;

    if (!m_mainWindow->isActiveWindow()) {
        m_violationCount++;
        forceForeground(m_mainWindow);
        emit focusViolationDetected(
            QString("Focus lost (%1 violation(s))").arg(m_violationCount)
        );
    }
}

void LockdownManager::onWindowCheck() {
    if (!m_engaged || !m_mainWindow) return;
    // Ensure still fullscreen + on top
    if (!m_mainWindow->isFullScreen()) {
        m_mainWindow->showFullScreen();
    }
    m_mainWindow->raise();
}

void LockdownManager::forceForeground(QWidget* win) {
#ifdef PLATFORM_WINDOWS
    HWND hwnd = reinterpret_cast<HWND>(win->winId());
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    ShowWindow(hwnd, SW_SHOW);
#endif
    win->raise();
    win->activateWindow();
}

void LockdownManager::applyPlatformLockdown(QWidget* win) {
#ifdef PLATFORM_WINDOWS
    // Set topmost via WinAPI
    HWND hwnd = reinterpret_cast<HWND>(win->winId());
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Install low-level keyboard hook
    g_kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);

    // Block task manager shortcut (requires elevated privileges in practice)
    // SystemParametersInfo(SPI_SETSCREENSAVEACTIVE, FALSE, nullptr, 0);
#endif

#ifdef PLATFORM_LINUX
    // On Linux/X11 we rely on Qt flags + the polling mechanism
    // Full grab of keyboard can be done via XGrabKeyboard but requires careful release
    Q_UNUSED(win)
#endif
}

void LockdownManager::removePlatformLockdown() {
#ifdef PLATFORM_WINDOWS
    if (g_kbHook) {
        UnhookWindowsHookEx(g_kbHook);
        g_kbHook = nullptr;
    }
    if (m_mainWindow) {
        HWND hwnd = reinterpret_cast<HWND>(m_mainWindow->winId());
        SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
#endif
}
