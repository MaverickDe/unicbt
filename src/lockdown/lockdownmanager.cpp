#include "lockdownmanager.h"
#include <QWidget>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>





#include <QDesktopServices>
#include <QUrl>
#include <QProcess>

#ifdef Q_OS_MAC
#include <ApplicationServices/ApplicationServices.h>
#include <CoreGraphics/CoreGraphics.h>
#endif

#ifdef Q_OS_WIN
#include <windows.h>
#endif



// ─────────────────────────────────────────────────────────────────────────────
// WINDOWS — low level keyboard hook
// ─────────────────────────────────────────────────────────────────────────────
#ifdef PLATFORM_WINDOWS
  #include <windows.h>
  #include <winuser.h>

  static HHOOK g_kbHook = nullptr;

  static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
      if (nCode == HC_ACTION) {
          KBDLLHOOKSTRUCT* p = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
          if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN)                          return 1;
          if (p->vkCode == VK_TAB      && (GetAsyncKeyState(VK_MENU)    & 0x8000))   return 1;
          if (p->vkCode == VK_F4       && (GetAsyncKeyState(VK_MENU)    & 0x8000))   return 1;
          if (p->vkCode == VK_ESCAPE   && (GetAsyncKeyState(VK_CONTROL) & 0x8000))   return 1;
          if (p->vkCode == VK_SNAPSHOT)                                               return 1;
      }
      return CallNextHookEx(g_kbHook, nCode, wParam, lParam);
  }
#endif

// ─────────────────────────────────────────────────────────────────────────────
// MAC — CGEventTap (requires Accessibility permission)
// ─────────────────────────────────────────────────────────────────────────────
#ifdef Q_OS_MAC
  #include <ApplicationServices/ApplicationServices.h>
  #include <Carbon/Carbon.h>

  // Define the static members declared in the header
  CFMachPortRef      LockdownManager::s_eventTap      = nullptr;
  CFRunLoopSourceRef LockdownManager::s_runLoopSource  = nullptr;

//   CGEventRef LockdownManager::eventTapCallback(
//       CGEventTapProxy  proxy,
//       CGEventType      type,
//       CGEventRef       event,
//       void*            refcon)
//   {
//       Q_UNUSED(proxy)
//       Q_UNUSED(refcon)

//       // If the tap gets disabled by the OS (can happen if it's too slow),
//       // re-enable it immediately
//       if (type == kCGEventTapDisabledByTimeout ||
//           type == kCGEventTapDisabledByUserInput) {
//           if (s_eventTap) CGEventTapEnable(s_eventTap, true);
//           return event;
//       }

//       CGEventFlags flags   = CGEventGetFlags(event);
//       int64_t      keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

//       bool cmdHeld  = (flags & kCGEventFlagMaskCommand)  != 0;
//       bool ctrlHeld = (flags & kCGEventFlagMaskControl)  != 0;

//       // ── Block every dangerous combo ───────────────────────────────────────

//       // Cmd+Tab — app switcher
//       if (cmdHeld && keyCode == kVK_Tab)        return nullptr;

//       // Cmd+H — hide app
//       if (cmdHeld && keyCode == kVK_ANSI_H)     return nullptr;

//       // Cmd+M — minimize window
//       if (cmdHeld && keyCode == kVK_ANSI_M)     return nullptr;

//       // Cmd+W — close window
//       if (cmdHeld && keyCode == kVK_ANSI_W)     return nullptr;

//       // Cmd+Q — quit app
//       if (cmdHeld && keyCode == kVK_ANSI_Q)     return nullptr;

//       // Cmd+Space — Spotlight (could open another app)
//       if (cmdHeld && keyCode == kVK_Space)       return nullptr;

//       // Ctrl+Up / Ctrl+Down — Mission Control / App Expose
//       if (ctrlHeld && keyCode == kVK_UpArrow)   return nullptr;
//       if (ctrlHeld && keyCode == kVK_DownArrow)  return nullptr;

//       // Cmd+F3 — Show Desktop
//       if (cmdHeld && keyCode == kVK_F3)          return nullptr;

//       // F3 alone — Mission Control on some keyboard layouts
//       if (keyCode == kVK_F3)                     return nullptr;

//       // Let everything else through normally
//       return event;
//   }

//   void LockdownManager::installMacEventTap() {
//       // This is the mask of event types we want to intercept
//       CGEventMask mask = CGEventMaskBit(kCGEventKeyDown)     |
//                          CGEventMaskBit(kCGEventFlagsChanged);

//       s_eventTap = CGEventTapCreate(
//           kCGSessionEventTap,        // session-level: catches events before any app sees them
//           kCGHeadInsertEventTap,     // head of the chain: we get first look
//           kCGEventTapOptionDefault,  // we can modify or swallow (return nullptr) events
//           mask,
//           eventTapCallback,
//           nullptr                    // refcon — context pointer we don't need here
//       );

//       if (!s_eventTap) {
//           // Permission wasn't granted — inform clearly
//           qDebug() << "[LockdownManager] CGEventTap creation FAILED.";
//           qDebug() << "Grant Accessibility permission:";
//           qDebug() << "System Settings → Privacy & Security → Accessibility → enable CBTExam";

//           // Show a visible dialog so the exam runner knows lockdown is incomplete
//           QMessageBox::warning(
//               nullptr,
//               "Accessibility Permission Required",
//               "Full keyboard lockdown requires Accessibility access.\n\n"
//               "Please go to:\n"
//               "System Settings → Privacy & Security → Accessibility\n"
//               "and enable CBTExam, then restart the application.\n\n"
//               "The exam will continue with reduced lockdown (focus polling only)."
//           );
//           return;
//       }

//       // Wire the tap into the main run loop so it actually processes events
//       s_runLoopSource = CFMachPortCreateRunLoopSource(
//           kCFAllocatorDefault,
//           s_eventTap,
//           0
//       );

//       CFRunLoopAddSource(
//           CFRunLoopGetMain(),
//           s_runLoopSource,
//           kCFRunLoopCommonModes
//       );

//       CGEventTapEnable(s_eventTap, true);

//       qDebug() << "[LockdownManager] CGEventTap installed — Mac keyboard lockdown ACTIVE";
//   }

CGEventRef LockdownManager::eventTapCallback(
    CGEventTapProxy  proxy,
    CGEventType      type,
    CGEventRef       event,
    void*            refcon)
{
    Q_UNUSED(proxy)
    Q_UNUSED(refcon)

    if (type == kCGEventTapDisabledByTimeout || type == kCGEventTapDisabledByUserInput) {
        if (s_eventTap) CGEventTapEnable(s_eventTap, true);
        return event;
    }

    // ── Block Multi-Finger Trackpad Swipes / Mission Control Gestures ────────
    if (type == kCGEventScrollWheel) {
        // Check if swipe is accompanied by modifier keys or rapid multi-axis scroll
        CGEventFlags flags = CGEventGetFlags(event);
        bool ctrlHeld = (flags & kCGEventFlagMaskControl) != 0;
        bool cmdHeld  = (flags & kCGEventFlagMaskCommand) != 0;

        // Block swipe gestures triggered while holding Ctrl / Cmd
        if (ctrlHeld || cmdHeld) {
            return nullptr;
        }

        // Detect 3/4 finger gesture scrolls (macOS reports these with large vertical/horizontal deltas)
        int64_t isContinuous = CGEventGetIntegerValueField(event, kCGScrollWheelEventIsContinuous);
        int64_t pointDeltaY  = CGEventGetIntegerValueField(event, kCGScrollWheelEventPointDeltaAxis1);
        
        // Swiping down to minimize produces high Y delta continuous events
        if (isContinuous && (qAbs(pointDeltaY) > 50)) {
            return nullptr; // Swallow swipe gesture
        }
    }

    // ── Block Keyboard Shortcuts ─────────────────────────────────────────────
    if (type == kCGEventKeyDown || type == kCGEventFlagsChanged) {
        CGEventFlags flags   = CGEventGetFlags(event);
        int64_t      keyCode = CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

        bool cmdHeld  = (flags & kCGEventFlagMaskCommand)  != 0;
        bool ctrlHeld = (flags & kCGEventFlagMaskControl)  != 0;

        // Cmd+Tab — app switcher
        if (cmdHeld && keyCode == kVK_Tab)        return nullptr;
        // Cmd+H — hide app
        if (cmdHeld && keyCode == kVK_ANSI_H)     return nullptr;
        // Cmd+M — minimize window
        if (cmdHeld && keyCode == kVK_ANSI_M)     return nullptr;
        // Cmd+W — close window
        if (cmdHeld && keyCode == kVK_ANSI_W)     return nullptr;
        // Cmd+Q — quit app
        if (cmdHeld && keyCode == kVK_ANSI_Q)     return nullptr;
        // Cmd+Space — Spotlight
        if (cmdHeld && keyCode == kVK_Space)      return nullptr;

        // Ctrl+Up / Ctrl+Down — Mission Control / App Expose
        if (ctrlHeld && keyCode == kVK_UpArrow)   return nullptr;
        if (ctrlHeld && keyCode == kVK_DownArrow) return nullptr;
        if (ctrlHeld && keyCode == kVK_LeftArrow) return nullptr;  // Block space switching left
        if (ctrlHeld && keyCode == kVK_RightArrow) return nullptr; // Block space switching right

        // F3 / Cmd+F3 — Mission Control / Desktop
        if (keyCode == kVK_F3 || (cmdHeld && keyCode == kVK_F3)) return nullptr;
    }

    return event;
}


void LockdownManager::installMacEventTap() {
    // ── Extend Mask to Capture Trackpad Gestures & Scrolls ───────────────────
    CGEventMask mask = CGEventMaskBit(kCGEventKeyDown)         |
                       CGEventMaskBit(kCGEventFlagsChanged)    |
                       CGEventMaskBit(kCGEventScrollWheel)     | // Catches 2/3 finger trackpad swipes
                       CGEventMaskBit(kCGEventLeftMouseDown)   |
                       CGEventMaskBit(kCGEventOtherMouseDown);

    s_eventTap = CGEventTapCreate(
        kCGSessionEventTap,        // session-level: catches events before system
        kCGHeadInsertEventTap,     // head of chain
        kCGEventTapOptionDefault,  // allow swallow / return nullptr
        mask,
        eventTapCallback,
        nullptr
    );

    if (!s_eventTap) {
        qDebug() << "[LockdownManager] CGEventTap creation FAILED.";
        return;
    }

    s_runLoopSource = CFMachPortCreateRunLoopSource(
        kCFAllocatorDefault,
        s_eventTap,
        0
    );

    CFRunLoopAddSource(
        CFRunLoopGetMain(),
        s_runLoopSource,
        kCFRunLoopCommonModes
    );

    CGEventTapEnable(s_eventTap, true);

    // ── Force macOS into True System Kiosk Mode ──────────────────────────────
    // This explicitly disables macOS gestures, spaces, Dock, and menu bar
    // SetSystemUIMode(kSystemUiModeAllHidden, kUIOptionDisableAppleMenu | kUIOptionDisableProcessSwitch);
SetSystemUIMode(kUIModeAllHidden, kUIOptionDisableAppleMenu | kUIOptionDisableProcessSwitch);
    qDebug() << "[LockdownManager] CGEventTap installed + Kiosk UI Mode Active";
}

void LockdownManager::removeMacEventTap() {
    if (s_eventTap) {
        CGEventTapEnable(s_eventTap, false);
        CFRunLoopRemoveSource(
            CFRunLoopGetMain(),
            s_runLoopSource,
            kCFRunLoopCommonModes
        );
        CFRelease(s_runLoopSource);
        CFRelease(s_eventTap);
        s_eventTap      = nullptr;
        s_runLoopSource = nullptr;

        // Restore normal macOS UI mode & trackpad behavior
        // SetSystemUIMode(kSystemUiModeNormal, 0);
        SetSystemUIMode(kUIModeNormal, 0);

        qDebug() << "[LockdownManager] CGEventTap removed — UI & gestures restored";
    }
}

//   void LockdownManager::removeMacEventTap() {
//       if (s_eventTap) {
//           CGEventTapEnable(s_eventTap, false);
//           CFRunLoopRemoveSource(
//               CFRunLoopGetMain(),
//               s_runLoopSource,
//               kCFRunLoopCommonModes
//           );
//           CFRelease(s_runLoopSource);
//           CFRelease(s_eventTap);
//           s_eventTap      = nullptr;
//           s_runLoopSource = nullptr;
//           qDebug() << "[LockdownManager] CGEventTap removed — keyboard restored";
//       }
//   }
#endif

// ─────────────────────────────────────────────────────────────────────────────
// CONSTRUCTOR / DESTRUCTOR
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
// ENGAGE — called when exam questions load, not before
// ─────────────────────────────────────────────────────────────────────────────
void LockdownManager::engage(QWidget* mainWindow) {
    if (m_engaged) return;

    m_mainWindow     = mainWindow;
    m_engaged        = true;
    m_violationCount = 0;

    // OS-level key interception (Windows hook / Mac CGEventTap)
    applyPlatformLockdown(mainWindow);

    // Qt-level window flags: fullscreen, no title bar, always on top
    mainWindow->setWindowFlags(
        Qt::Window              |
        Qt::FramelessWindowHint |
        Qt::WindowStaysOnTopHint
    );
    mainWindow->showFullScreen();
    mainWindow->raise();
    mainWindow->activateWindow();

    // Watchdog timers — cross-platform safety net
    // 100ms focus check: very tight window for the student to do anything
    m_focusTimer->start(100);
    // 1s window state check: ensure fullscreen is maintained
    m_windowTimer->start(1000);

    qDebug() << "[LockdownManager] Lockdown ENGAGED";
}

// ─────────────────────────────────────────────────────────────────────────────
// DISENGAGE — called after exam submit or on result page
// ─────────────────────────────────────────────────────────────────────────────
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

    qDebug() << "[LockdownManager] Lockdown DISENGAGED";
}

// ─────────────────────────────────────────────────────────────────────────────
// WATCHDOG SLOTS — fire on timer, cross-platform
// ─────────────────────────────────────────────────────────────────────────────
void LockdownManager::onFocusCheck() {
    if (!m_engaged || !m_mainWindow) return;

    if (!m_mainWindow->isActiveWindow()) {
        m_violationCount++;
        forceForeground(m_mainWindow);
        emit focusViolationDetected(
            QString("Focus lost — attempt #%1").arg(m_violationCount)
        );
    }
}

void LockdownManager::onWindowCheck() {
    if (!m_engaged || !m_mainWindow) return;

    if (!m_mainWindow->isFullScreen()) {
        m_mainWindow->showFullScreen();
    }
    m_mainWindow->raise();
}

// ─────────────────────────────────────────────────────────────────────────────
// FORCE FOREGROUND — yanks the window back to front
// ─────────────────────────────────────────────────────────────────────────────
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

// ─────────────────────────────────────────────────────────────────────────────
// PLATFORM LOCKDOWN — apply/remove OS-level intercepts
// ─────────────────────────────────────────────────────────────────────────────
void LockdownManager::applyPlatformLockdown(QWidget* win) {
#ifdef PLATFORM_WINDOWS
    HWND hwnd = reinterpret_cast<HWND>(win->winId());
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    g_kbHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, nullptr, 0);
#endif

#ifdef Q_OS_MAC
    Q_UNUSED(win)
    installMacEventTap();
#endif

#ifdef PLATFORM_LINUX
    Q_UNUSED(win)
    // TODO: XGrabKeyboard for full Linux lockdown
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

#ifdef Q_OS_MAC
    removeMacEventTap();
#endif
}



bool LockdownManager::hasPermissions() const {
#ifdef Q_OS_MAC
    // 1. Check Accessibility Permissions (Required for key-blocking & global hooks)
    // AXIsProcessTrustedWithOptions with kAXTrustedCheckOptionPrompt set to false checks silently without prompting.
    CFStringRef keys[] = { kAXTrustedCheckOptionPrompt };
    CFTypeRef values[] = { kCFBooleanFalse };
    CFDictionaryRef options = CFDictionaryCreate(
        kCFAllocatorDefault,
        (const void**)keys,
        (const void**)values,
        1,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks
    );

    bool accessibilityGranted = AXIsProcessTrustedWithOptions(options);
    CFRelease(options);

    // 2. Check Screen Capture / Recording Permissions
    // CGPreflightScreenCaptureAccess() checks if screen capture permissions are granted (macOS 10.15+)
    // bool screenRecordingGranted = CGPreflightScreenCaptureAccess();

    return accessibilityGranted ;
    // && screenRecordingGranted;

#elif defined(Q_OS_WIN)
    // On Windows, check if the app is elevated / running as Administrator 
    // (Needed if your Lockdown Hook installs a WH_KEYBOARD_LL low-level hook)
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    // Return true on Windows (or isAdmin if you strictly enforce Admin execution)
    return true; 
#else
    return true;
#endif
}




void LockdownManager::requestPermissions() {
#ifdef Q_OS_MAC
    // 1. Trigger macOS Accessibility Prompt (Will show the OS dialog asking user to grant permission)
    CFStringRef keys[] = { kAXTrustedCheckOptionPrompt };
    CFTypeRef values[] = { kCFBooleanTrue }; // Passing TRUE triggers system dialog!
    CFDictionaryRef options = CFDictionaryCreate(
        kCFAllocatorDefault,
        (const void**)keys,
        (const void**)values,
        1,
        &kCFTypeDictionaryKeyCallBacks,
        &kCFTypeDictionaryValueCallBacks
    );

    AXIsProcessTrustedWithOptions(options);
    CFRelease(options);

    // 2. Trigger Screen Capture Authorization Prompt
    // CGRequestScreenCaptureAccess();

    // 3. Open macOS System Settings directly to Privacy & Security -> Accessibility
    // Deep links work on macOS Ventura (13.0) and newer:
    QDesktopServices::openUrl(QUrl("x-apple.systempreferences:com.apple.preference.security?Privacy_Accessibility"));

#elif defined(Q_OS_WIN)
    // On Windows, if administrative rights are required for low-level hooks,
    // prompt the user to restart the application as Administrator.
    // (Optional: You can trigger UAC elevation here if needed)
#endif
}