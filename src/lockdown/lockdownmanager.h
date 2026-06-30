#pragma once
#include <QObject>
#include <QTimer>
#include <QWindow>

class QWidget;

class LockdownManager : public QObject {
    Q_OBJECT
public:
    explicit LockdownManager(QObject* parent = nullptr);
    ~LockdownManager();

    void engage(QWidget* mainWindow);
    void disengage();
    bool isEngaged() const { return m_engaged; }

signals:
    void focusViolationDetected(const QString& reason);
    void windowRestored();

private slots:
    void onFocusCheck();
    void onWindowCheck();

private:
    void applyPlatformLockdown(QWidget* win);
    void removePlatformLockdown();
    void forceForeground(QWidget* win);

    QWidget*  m_mainWindow = nullptr;
    QTimer*   m_focusTimer = nullptr;
    QTimer*   m_windowTimer = nullptr;
    bool      m_engaged     = false;
    int       m_violationCount = 0;

#ifdef PLATFORM_WINDOWS
    void* m_hookHandle = nullptr;
    void  installWindowsHooks();
    void  removeWindowsHooks();
#endif
};
