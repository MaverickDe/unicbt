/**
 * CBT Examination Server — Sample Node.js/Express Implementation
 * 
 * Run:  npm install express  &&  node server.js
 * Port: 8080 (default)
 * 
 * This is a reference implementation. Replace the in-memory
 * stores with a real database (PostgreSQL, SQLite, etc.) for production.
 */

const express = require('express');
const app     = express();
app.use(express.json());

// ─── In-memory stores (replace with DB) ─────────────────────────────────────

const students = {
    'CSC/2021/001': { name: 'Ada Okafor',    password: null },
    'CSC/2021/002': { name: 'Emeka Nwosu',   password: null },
    'EEE/2020/010': { name: 'Fatima Bello',  password: null },
};

const exams = {
    'CSC301-2024': {
        course:            'Data Structures & Algorithms',
        duration_seconds:  3600,        // 1 hour
        required_questions: 20,
        questions: [
            {
                id: 1, type: 'mcq', marks: 2,
                text: 'Which data structure uses LIFO order?',
                options: ['Queue', 'Stack', 'Linked List', 'Tree'],
                answer: 'Stack'
            },
            {
                id: 2, type: 'tf', marks: 1,
                text: 'A binary search tree always has O(log n) search time.',
                options: ['True', 'False'],
                answer: 'False'
            },
            {
                id: 3, type: 'mcq', marks: 2,
                text: 'What is the time complexity of quicksort in the average case?',
                options: ['O(n)', 'O(n log n)', 'O(n²)', 'O(log n)'],
                answer: 'O(n log n)'
            },
            {
                id: 4, type: 'short', marks: 3,
                text: 'Define a Hash Table and explain one collision resolution strategy.',
                options: [],
                answer: null  // manual grading
            },
            {
                id: 5, type: 'fillblank', marks: 2,
                text: 'The process of accessing elements in a stack is called ______.',
                options: [],
                answer: 'popping'
            },
            // Add more questions as needed...
        ]
    }
};

// Active sessions  { token -> { regNumber, examCode, startTime, savedAnswers, elapsed } }
const sessions = {};

// Supervisor codes (in production: generate per-exam per-student, store in DB)
const supervisorCodes = {
    resume: 'SUP-RESUME-2024',
    retake: 'SUP-RETAKE-2024',
    exit:   'SUP-EXIT-2024',
};

// ─── Helpers ─────────────────────────────────────────────────────────────────

function generateToken() {
    return Math.random().toString(36).substring(2) + Date.now().toString(36);
}

function stripAnswers(questions) {
    return questions.map(({ answer, ...rest }) => rest);
}

function scoreExam(examCode, answers) {
    const exam = exams[examCode];
    if (!exam) return { score: 0, total: 0 };

    let score = 0, total = 0;
    const breakdown = [];

    for (const q of exam.questions) {
        total += q.marks;
        const given = (answers[String(q.id)] || '').trim().toLowerCase();
        const correct = q.answer ? q.answer.toLowerCase() : null;
        const isCorrect = correct !== null && given === correct;
        if (isCorrect) score += q.marks;
        breakdown.push({ id: q.id, correct: isCorrect, marks_earned: isCorrect ? q.marks : 0 });
    }

    return { score, total, breakdown };
}

function gradeFromPercent(pct) {
    if (pct >= 70) return 'A';
    if (pct >= 60) return 'B';
    if (pct >= 50) return 'C';
    if (pct >= 45) return 'D';
    return 'F';
}

// ─── Routes ──────────────────────────────────────────────────────────────────

// Ping — used by client to verify server is reachable
app.get('/api/ping', (req, res) => {
    res.json({ success: true, message: 'CBT Server is online', timestamp: Date.now() });
});

// Validate student + start session
app.post('/api/exam/validate', (req, res) => {
    const { reg_number, exam_code } = req.body;

    if (!students[reg_number])
        return res.json({ success: false, message: 'Registration number not found.' });

    if (!exams[exam_code])
        return res.json({ success: false, message: 'Invalid examination code.' });

    // Check if already submitted
    const existing = Object.values(sessions).find(
        s => s.regNumber === reg_number && s.examCode === exam_code && s.submitted
    );
    if (existing)
        return res.json({ success: false, message: 'Exam already submitted. Use retake option.' });

    const token = generateToken();
    const exam  = exams[exam_code];
    const student = students[reg_number];

    sessions[token] = {
        regNumber: reg_number,
        examCode:  exam_code,
        startTime: Date.now(),
        savedAnswers: {},
        secondsElapsed: 0,
        submitted: false,
    };

    res.json({
        success: true,
        data: {
            session_token:      token,
            name:               student.name,
            course:             exam.course,
            duration_seconds:   exam.duration_seconds,
            required_questions: exam.required_questions,
        }
    });
});

// Resume exam (requires supervisor code)
app.post('/api/exam/resume', (req, res) => {
    const { reg_number, exam_code, resume_code } = req.body;

    if (resume_code !== supervisorCodes.resume)
        return res.json({ success: false, message: 'Invalid resume code. Contact your supervisor.' });

    const session = Object.values(sessions).find(
        s => s.regNumber === reg_number && s.examCode === exam_code && !s.submitted
    );

    if (!session)
        return res.json({ success: false, message: 'No active session found to resume.' });

    const token = Object.keys(sessions).find(
        k => sessions[k] === session
    );

    const exam    = exams[exam_code];
    const student = students[reg_number];

    // Convert savedAnswers keys to match client expectation
    const savedAnswersOut = {};
    for (const [qid, ans] of Object.entries(session.savedAnswers))
        savedAnswersOut[qid] = ans;

    res.json({
        success: true,
        data: {
            session_token:      token,
            name:               student.name,
            course:             exam.course,
            duration_seconds:   exam.duration_seconds,
            required_questions: exam.required_questions,
            seconds_elapsed:    session.secondsElapsed,
            saved_answers:      savedAnswersOut,
        }
    });
});

// Retake exam (requires supervisor code — resets session)
app.post('/api/exam/retake', (req, res) => {
    const { reg_number, exam_code, retake_code } = req.body;

    if (retake_code !== supervisorCodes.retake)
        return res.json({ success: false, message: 'Invalid retake code. Contact your supervisor.' });

    if (!students[reg_number])
        return res.json({ success: false, message: 'Registration number not found.' });

    if (!exams[exam_code])
        return res.json({ success: false, message: 'Invalid examination code.' });

    // Invalidate old sessions for this student+exam
    for (const [tok, s] of Object.entries(sessions)) {
        if (s.regNumber === reg_number && s.examCode === exam_code)
            delete sessions[tok];
    }

    // Create new session
    const token = generateToken();
    const exam  = exams[exam_code];
    const student = students[reg_number];

    sessions[token] = {
        regNumber: reg_number,
        examCode:  exam_code,
        startTime: Date.now(),
        savedAnswers: {},
        secondsElapsed: 0,
        submitted: false,
    };

    res.json({
        success: true,
        data: {
            session_token:      token,
            name:               student.name,
            course:             exam.course,
            duration_seconds:   exam.duration_seconds,
            required_questions: exam.required_questions,
        }
    });
});

// Get questions (auth required)
app.get('/api/exam/questions', (req, res) => {
    const token = (req.headers['authorization'] || '').replace('Bearer ', '');
    const session = sessions[token];

    if (!session)
        return res.json({ success: false, message: 'Invalid or expired session.' });

    const exam = exams[session.examCode];

    res.json({
        success: true,
        questions: stripAnswers(exam.questions),   // Never send answers to client
        meta: {
            total_questions:    exam.questions.length,
            required_questions: exam.required_questions,
            duration_seconds:   exam.duration_seconds,
        }
    });
});

// Save progress (auto-save endpoint — optional, call every 30s from client)
app.post('/api/exam/save', (req, res) => {
    const token = (req.headers['authorization'] || '').replace('Bearer ', '');
    const session = sessions[token];
    if (!session) return res.json({ success: false, message: 'Invalid session.' });

    const { answers, seconds_used } = req.body;
    session.savedAnswers   = answers || {};
    session.secondsElapsed = seconds_used || 0;
    res.json({ success: true });
});

// Submit exam
app.post('/api/exam/submit', (req, res) => {
    const token = (req.headers['authorization'] || '').replace('Bearer ', '');
    const session = sessions[token];

    if (!session)
        return res.json({ success: false, message: 'Invalid or expired session.' });
    if (session.submitted)
        return res.json({ success: false, message: 'Exam already submitted.' });

    const { answers, seconds_used } = req.body;
    session.submitted      = true;
    session.savedAnswers   = answers;
    session.secondsElapsed = seconds_used;
    session.submittedAt    = Date.now();

    const { score, total, breakdown } = scoreExam(session.examCode, answers);
    const pct   = total > 0 ? Math.round(score * 100 / total) : 0;
    const grade = gradeFromPercent(pct);

    res.json({
        success: true,
        data: {
            score,
            total_marks:  total,
            percentage:   pct,
            grade,
            passed:       pct >= 50,
            seconds_used: seconds_used,
            breakdown,
        }
    });
});

// ─── Start ───────────────────────────────────────────────────────────────────
const PORT = process.env.PORT || 8080;
app.listen(PORT, () => {
    console.log(`\n✅ CBT Server running on http://localhost:${PORT}`);
    console.log(`   Ping:      GET  /api/ping`);
    console.log(`   Validate:  POST /api/exam/validate`);
    console.log(`   Questions: GET  /api/exam/questions`);
    console.log(`   Submit:    POST /api/exam/submit`);
    console.log(`   Resume:    POST /api/exam/resume   (supervisor code: ${supervisorCodes.resume})`);
    console.log(`   Retake:    POST /api/exam/retake   (supervisor code: ${supervisorCodes.retake})`);
    console.log(`\n   Test credentials:`);
    console.log(`     Reg:       CSC/2021/001`);
    console.log(`     Exam Code: CSC301-2024\n`);
});
