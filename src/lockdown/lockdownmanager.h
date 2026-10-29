// #pragma once
// #include <QObject>
// #include <QTimer>
// #include <QWindow>

// class QWidget;

// // ── Mac: pull in CoreFoundation types BEFORE they are referenced ──────────
// // Must be in the header (not just .cpp) because moc processes this header
// // and needs to see CFMachPortRef / CFRunLoopSourceRef type definitions.
// #ifdef Q_OS_MAC
//   #include <CoreFoundation/CoreFoundation.h>
//   #include <ApplicationServices/ApplicationServices.h>
// #endif

// class LockdownManager : public QObject {
//     Q_OBJECT
// public:
//     explicit LockdownManager(QObject* parent = nullptr);
//     ~LockdownManager();

//     void engage(QWidget* mainWindow);
//     void disengage();
//     bool isEngaged() const { return m_engaged; }

// signals:
//     void focusViolationDetected(const QString& reason);

// private slots:
//     void onFocusCheck();
//     void onWindowCheck();

// private:
//     void applyPlatformLockdown(QWidget* win);
//     void removePlatformLockdown();
//     void forceForeground(QWidget* win);

//     QWidget*  m_mainWindow     = nullptr;
//     QTimer*   m_focusTimer     = nullptr;
//     QTimer*   m_windowTimer    = nullptr;
//     bool      m_engaged        = false;
//     int       m_violationCount = 0;

// #ifdef PLATFORM_WINDOWS
//     void installWindowsHooks();
//     void removeWindowsHooks();
// #endif

// #ifdef Q_OS_MAC
//     void installMacEventTap();
//     void removeMacEventTap();

//     // static: CGEventTap requires a plain C function pointer (no hidden `this`)
//     static CGEventRef eventTapCallback(CGEventTapProxy proxy,
//                                        CGEventType     type,
//                                        CGEventRef      event,
//                                        void*           refcon);

//     // static: only one event tap should ever exist per process
//     static CFMachPortRef       s_eventTap;
//     static CFRunLoopSourceRef  s_runLoopSource;
// #endif
// };




#pragma once
#include <QObject>
#include <QTimer>
#include <QWindow>

class QWidget;

// ── Mac: pull in CoreFoundation types BEFORE they are referenced ──────────
// Must be in the header (not just .cpp) because moc processes this header
// and needs to see CFMachPortRef / CFRunLoopSourceRef type definitions.
#ifdef Q_OS_MAC
  #include <CoreFoundation/CoreFoundation.h>
  #include <ApplicationServices/ApplicationServices.h>
#endif

class LockdownManager : public QObject {
    Q_OBJECT
public:
    explicit LockdownManager(QObject* parent = nullptr);
    ~LockdownManager();

    // ─── Permission Check & Request API ──────────────────────────────────────
    /**
     * @brief Checks if required OS-level lockdown permissions (e.g., Accessibility & 
     *        Screen Capture on macOS) are granted before starting the exam.
     */
    bool hasPermissions() const;

    /**
     * @brief Prompts the user and opens System Settings/Preferences so missing 
     *        permissions can be granted without initiating lockdown.
     */
    void requestPermissions();

    // ─── Lockdown Control ────────────────────────────────────────────────────
    void engage(QWidget* mainWindow);
    void disengage();
    bool isEngaged() const { return m_engaged; }

signals:
    void focusViolationDetected(const QString& reason);

private slots:
    void onFocusCheck();
    void onWindowCheck();

private:
    void applyPlatformLockdown(QWidget* win);
    void removePlatformLockdown();
    void forceForeground(QWidget* win);

    QWidget*  m_mainWindow     = nullptr;
    QTimer*   m_focusTimer     = nullptr;
    QTimer*   m_windowTimer    = nullptr;
    bool      m_engaged        = false;
    int       m_violationCount = 0;

#ifdef PLATFORM_WINDOWS
    void installWindowsHooks();
    void removeWindowsHooks();
#endif

#ifdef Q_OS_MAC
    void installMacEventTap();
    void removeMacEventTap();

    // static: CGEventTap requires a plain C function pointer (no hidden `this`)
    static CGEventRef eventTapCallback(CGEventTapProxy proxy,
                                       CGEventType     type,
                                       CGEventRef      event,
                                       void*           refcon);

    // static: only one event tap should ever exist per process
    static CFMachPortRef       s_eventTap;
    static CFRunLoopSourceRef  s_runLoopSource;
#endif
};