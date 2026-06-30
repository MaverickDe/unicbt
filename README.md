# CBT Examination Portal — Qt/C++ Client + Node.js Server

A fully locked-down Computer-Based Testing system built with **Qt 6 / C++17** and a sample **Node.js/Express** server.

---

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                   Qt/C++ Client App                     │
│                                                         │
│  OnboardPage → BioPage → ExamPage → ResultPage          │
│       ↓            ↓         ↓                          │
│   LockdownManager (OS-level lockdown via hooks)         │
│   ApiClient      (HTTP to localhost server)             │
└─────────────────────────────────────────────────────────┘
            ↕  HTTP (localhost only)
┌─────────────────────────────────────────────────────────┐
│               Node.js/Express Server                    │
│   /api/ping  /api/exam/validate  /api/exam/questions    │
│   /api/exam/submit  /api/exam/resume  /api/exam/retake  │
└─────────────────────────────────────────────────────────┘
```

---

## File Structure

```
cbt_exam/
├── CMakeLists.txt
├── README.md
├── server/
│   └── server.js                 ← Node.js exam server
└── src/
    ├── main.cpp
    ├── mainwindow.h / .cpp       ← Orchestrator + close/key guards
    ├── lockdown/
    │   ├── lockdownmanager.h
    │   └── lockdownmanager.cpp   ← OS-level lockdown (Win hooks + polling)
    ├── network/
    │   ├── apiclient.h
    │   └── apiclient.cpp         ← All HTTP communication
    ├── models/
    │   ├── question.h            ← Question struct + JSON parsing
    │   ├── examsession.h         ← Full session state
    │   └── examsession.cpp
    ├── pages/
    │   ├── onboardpage.h / .cpp  ← URL entry + ping
    │   ├── biopage.h / .cpp      ← Reg no., exam code, resume/retake
    │   ├── exampage.h / .cpp     ← Exam UI + timer + navigation
    │   └── resultpage.h / .cpp   ← Instant result display
    └── widgets/
        ├── timerwidget.h / .cpp          ← Countdown timer with warnings
        ├── questionwidget.h / .cpp       ← All question types
        ├── notificationpopup.h / .cpp    ← Toast notifications
        └── supervisormodal.h / .cpp      ← Supervisor code dialog
```

---

## Building the Client

### Prerequisites
- Qt 6.4+ (QtCore, QtWidgets, QtNetwork, QtGui)
- CMake 3.16+
- C++17 compiler (MSVC / GCC / Clang)
- On Linux: `libx11-dev` (optional, for X11 key grab)

### Windows (MSVC)
```bat
mkdir build && cd build
cmake .. -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:/Qt/6.x.x/msvc2019_64"
cmake --build . --config Release
```

### Linux (GCC)
```bash
mkdir build && cd build
cmake .. -DCMAKE_PREFIX_PATH="/opt/Qt/6.x.x/gcc_64"
make -j$(nproc)
```

---

## Running the Server

```bash
cd server
npm install express
node server.js
# Server starts at http://localhost:8080
```

### Default test credentials
| Field       | Value           |
|-------------|-----------------|
| Reg Number  | `CSC/2021/001`  |
| Exam Code   | `CSC301-2024`   |

### Supervisor codes (change in production!)
| Action | Code              |
|--------|-------------------|
| Resume | `SUP-RESUME-2024` |
| Retake | `SUP-RETAKE-2024` |

---

## Exam Flow

```
Onboard (URL input + ping)
    ↓ ping OK → LOCKDOWN ENGAGED
Bio Page
    ├── Start Exam     → validate credentials → fetch questions → ExamPage
    ├── Resume Exam    → supervisor code → restore session → ExamPage
    └── Retake Exam    → supervisor code → new session → ExamPage

ExamPage (FULLSCREEN LOCKDOWN)
    ├── Question navigator (colour-coded: answered/flagged/unanswered)
    ├── Timer (warnings at 5min + 1min, auto-submit on expire)
    ├── All question types: MCQ, True/False, Short Answer, Fill-in-Blank
    ├── Flag for review
    ├── Submit button (enabled only after required_questions answered)
    └── Submit modal (shows answered vs unanswered count)
        ↓
ResultPage (lockdown released)
    ├── Grade circle (A/B/C/D/F)
    ├── Score, percentage, pass/fail
    ├── Time used, answers count
    └── "New Exam / Next Candidate" → back to Onboard
```

---

## Lockdown Mechanisms

### Windows
| Mechanism | Method |
|-----------|--------|
| Alt+Tab   | Low-level keyboard hook (`WH_KEYBOARD_LL`) |
| Win key   | Same hook, blocks `VK_LWIN` / `VK_RWIN` |
| Alt+F4    | Blocks in hook |
| Ctrl+Esc  | Blocks in hook |
| PrintScreen | Blocks in hook |
| Minimise  | `changeEvent` restores fullscreen |
| Close     | `closeEvent` ignores, re-raises window |
| Always on top | `SetWindowPos(HWND_TOPMOST)` |
| Focus polling | 500ms timer, `SetForegroundWindow` on loss |

### Linux
| Mechanism | Method |
|-----------|--------|
| Minimise  | `changeEvent` restores fullscreen |
| Close     | `closeEvent` ignores |
| Always on top | `Qt::WindowStaysOnTopHint` |
| Focus polling | 500ms timer, `raise()` + `activateWindow()` |

> **Note:** On Linux, full keyboard grab (`XGrabKeyboard`) can be added for even stronger lockdown, but requires the app to run as the display owner. The current implementation handles the most common escape vectors.

> **Note:** On Windows, blocking Task Manager requires the app to run as Administrator. To enforce this, embed a manifest requesting elevation or run via Group Policy.

---

## Question Types (server `type` field)

| Server value | Type         | UI         |
|--------------|--------------|------------|
| `mcq`        | Multiple choice | Radio buttons (A/B/C/D) |
| `tf`         | True/False   | Radio buttons (True/False) |
| `short`      | Short answer | Text input field |
| `fillblank`  | Fill in blank| Text input field |

---

## API Reference

All endpoints return `{ success: bool, data/message }`.

### `GET /api/ping`
No auth. Returns `{ success: true }`.

### `POST /api/exam/validate`
```json
{ "reg_number": "CSC/2021/001", "exam_code": "CSC301-2024" }
```
Returns session token, candidate name, course, duration.

### `GET /api/exam/questions`
Header: `Authorization: Bearer <token>`
Returns question list (without answers) and meta.

### `POST /api/exam/resume`
```json
{ "reg_number": "...", "exam_code": "...", "resume_code": "SUP-RESUME-2024" }
```
Returns session + saved answers + seconds elapsed.

### `POST /api/exam/retake`
```json
{ "reg_number": "...", "exam_code": "...", "retake_code": "SUP-RETAKE-2024" }
```
Clears previous session and returns fresh session token.

### `POST /api/exam/submit`
Header: `Authorization: Bearer <token>`
```json
{ "answers": { "1": "Stack", "2": "False" }, "seconds_used": 1842 }
```
Returns score, grade, pass/fail, breakdown.

---

## Security Notes

- **Answers never leave the server.** Questions are stripped of `answer` field before being sent to the client.
- **Session tokens** are random strings — use UUID v4 in production.
- **Supervisor codes** should be randomly generated per-exam and stored server-side, not hardcoded.
- Run the server on a **dedicated LAN** with no internet access for full isolation.
- For maximum lockdown on Windows, sign the application and run it as a **restricted Windows user** with no task manager access.

---

## Production Checklist

- [ ] Replace in-memory stores with PostgreSQL/SQLite
- [ ] Generate per-exam supervisor codes (stored in DB)
- [ ] Add HTTPS (even on LAN, use self-signed cert)
- [ ] Add exam scheduling (time window enforcement on server)
- [ ] Log all focus violations server-side
- [ ] Auto-save answers every 30s (`/api/exam/save` endpoint provided)
- [ ] Windows: embed UAC manifest requesting elevation
- [ ] Linux: consider `XGrabKeyboard` for tighter lockdown
- [ ] Add invigilator admin panel (optional)
