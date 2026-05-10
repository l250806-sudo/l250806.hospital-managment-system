


#define _CRT_SECURE_NO_WARNINGS
#include <SFML/Graphics.hpp>
#include <ctime>
#include <cstdio>
#include<fstream>
#include "string.h"
#include "exception.h"
#include "storage.h"
#include "patient.h"
#include "doctor.h"
#include "Admin.h"
#include "appointment.h"
#include "Bill.h"
#include "prescription.h"
#include "filehandler.h"
#include "Validator.h"
using namespace std;
using namespace sf;

//constants for window size
const int WIN_W = 1100;
const int WIN_H = 700;
//colors
const Color BG(10, 18, 35);
const Color PANEL(18, 30, 55);
const Color TEAL(32, 201, 181);
const Color TEAL_D(20, 150, 135);
const Color AMBER(251, 191, 36);
const Color DANGER(220, 60, 60);
const Color SUCCESS(52, 211, 153);
const Color TXT(220, 235, 255);
const Color TXT2(140, 160, 195);
const Color BTN_N(32, 201, 181);
const Color BTN_H(50, 220, 200);
const Color BTN_DN(38, 55, 90);
const Color BTN_DH(55, 75, 115);
const Color RED_N(200, 50, 50);
const Color RED_H(230, 70, 70);



//states
FileHandler           fh;
Storage<patient>      gPatients;
Storage<doctor>       gDoctors;
Storage<appointment>  gAppointments;
Storage<bill>         gBills;
Storage<prescription> gPrescriptions;
Admin* gAdmin = nullptr;

Font gFont;  // loaded once, shared everywhere

// currently logged-in user IDs (-1 = not logged in)
int gPatientID = -1;
int gDoctorID = -1;

//constrol which screen is drawn
enum Screen
{
    SCR_LOGIN,
    SCR_PATIENT_DASHBOARD,
    SCR_DOCTOR_DASHBOARD,
    SCR_ADMIN_DASHBOARD,
    SCR_BOOK_APPOINTMENT,
    SCR_CANCEL_APPOINTMENT,
    SCR_VIEW_APPOINTMENTS,
    SCR_VIEW_RECORDS,
    SCR_VIEW_BILLS,
    SCR_PAY_BILL,
    SCR_TOPUP,
    SCR_DOC_TODAY,
    SCR_DOC_MARK,
    SCR_DOC_PRESCRIPTION,
    SCR_DOC_HISTORY,
    SCR_ADMIN_ADD_DOCTOR,
    SCR_ADMIN_REMOVE_DOCTOR,
    SCR_ADMIN_VIEW_PATIENTS,
    SCR_ADMIN_VIEW_DOCTORS,
    SCR_ADMIN_VIEW_APPTS,
    SCR_ADMIN_UNPAID_BILLS,
    SCR_ADMIN_DISCHARGE,
    SCR_ADMIN_SECURITY_LOG,
    SCR_ADMIN_DAILY_REPORT
};

Screen gScreen = SCR_LOGIN;


// helper function-draw text as a position
static void drawText(RenderWindow& w, const char* str,
    float x, float y, unsigned sz = 18,
    Color c = Color(220, 235, 255))
{
    Text t;
    t.setFont(gFont);
    t.setString(str);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    t.setPosition(x, y);
    w.draw(t);
}

// draw text centred on x
static void drawTextCentre(RenderWindow& w, const char* str,
    float cx, float y, unsigned sz, Color c)
{
    Text t;
    t.setFont(gFont);
    t.setString(str);
    t.setCharacterSize(sz);
    t.setFillColor(c);
    FloatRect r = t.getLocalBounds();
    t.setPosition(cx - r.width / 2.f, y);
    w.draw(t);
}

//draw a butto return true if clicked 
static bool drawButton(RenderWindow& w, const Event& ev,
    float x, float y, float bw, float bh,
    const char* label,
    Color cN = Color(32, 201, 181),
    Color cH = Color(50, 220, 200),
    Color txtC = Color(10, 18, 35))
{
    Vector2i mp = Mouse::getPosition(w);
    bool hovered = (mp.x >= x && mp.x <= x + bw && mp.y >= y && mp.y <= y + bh);

    RectangleShape box(Vector2f(bw, bh));
    box.setPosition(x, y);
    box.setFillColor(hovered ? cH : cN);
    w.draw(box);

    Text t;
    t.setFont(gFont);
    t.setString(label);
    t.setCharacterSize(15);
    t.setFillColor(txtC);
    FloatRect r = t.getLocalBounds();
    t.setPosition(x + (bw - r.width) / 2.f - r.left,
        y + (bh - r.height) / 2.f - r.top);
    w.draw(t);

    // detect click
    if (ev.type == Event::MouseButtonReleased &&
        ev.mouseButton.button == Mouse::Left)
    {
        if (mp.x >= x && mp.x <= x + bw && mp.y >= y && mp.y <= y + bh)
            return true;
    }
    return false;
}

//draw panel
static void drawPanel(RenderWindow& w, float x, float y, float pw, float ph,
    Color c = Color(18, 30, 55))
{
    RectangleShape p(Vector2f(pw, ph));
    p.setPosition(x, y);
    p.setFillColor(c);
    w.draw(p);
}

//header bar
static void drawHeader(RenderWindow& w, const char* title,
    const char* subtitle = "")
{
    drawPanel(w, 0, 0, WIN_W, 64, Color(13, 22, 45));
    drawText(w, "MediCore", 20, 18, 22, TEAL);
    drawTextCentre(w, title, WIN_W / 2.f, 20, 20, TXT);
    if (myStrLen(subtitle) > 0)
        drawTextCentre(w, subtitle, WIN_W / 2.f, 42, 14, TXT2);
}

//------------------get todays date------------//
static void getTodayStr(char* buf)
{
    time_t now = time(nullptr);
    struct tm* t = localtime(&now);
    char d[3], m[3], y[5];
    intToCharArr(t->tm_mday, d);
    intToCharArr(t->tm_mon + 1, m);
    intToCharArr(t->tm_year + 1900, y);
    // pad
    char dp[3], mp[3];
    if (myStrLen(d) == 1)
    { dp[0] = '0'; dp[1] = d[0]; dp[2] = '\0'; }
    else myStrCopy(dp, d, 3);
    if (myStrLen(m) == 1)
    { mp[0] = '0'; mp[1] = m[0]; mp[2] = '\0'; }
    else myStrCopy(mp, m, 3);
    buf[0] = '\0';
    myStrCat(buf, dp, 12); myStrCat(buf, "-", 12);
    myStrCat(buf, mp, 12); myStrCat(buf, "-", 12);
    myStrCat(buf, y, 12);
}

// ============================================================
//  TEXTBOX helper struct (no class - just local state)
// ============================================================
struct TBox
{
    char buf[256];
    int  len;
    int  maxLen;
    bool focused;
    bool password;
    float x, y, w, h;

    void init(float x_, float y_, float w_, float h_,
        int mLen = 80, bool pw = false)
    {
        x = x_; y = y_; w = w_; h = h_;
        buf[0] = '\0'; len = 0; maxLen = mLen;
        focused = false; password = pw;
    }

    void handleEvent(const Event& ev, const RenderWindow& win)
    {
        // click to focus
        if (ev.type == Event::MouseButtonPressed &&
            ev.mouseButton.button == Mouse::Left)
        {
            int mx = ev.mouseButton.x, my = ev.mouseButton.y;
            focused = (mx >= x && mx <= x + w && my >= y && my <= y + h);
        }
        if (!focused) return;
        if (ev.type == Event::TextEntered)
        {
            Uint32 c = ev.text.unicode;
            if (c == '\b' && len > 0)
                buf[--len] = '\0';
            else if (c >= 32 && c < 127 && len < maxLen - 1)
            {
                buf[len++] = (char)c;
                buf[len] = '\0';
            }
        }
    }

    void draw(RenderWindow& w_, const char* placeholder = "") const
    {
        // box background
        RectangleShape box(Vector2f(w, h));
        box.setPosition(x, y);
        box.setFillColor(Color(14, 24, 46));
        box.setOutlineThickness(focused ? 2.f : 1.f);
        box.setOutlineColor(focused ? TEAL : Color(40, 60, 100));
        w_.draw(box);

        // display text (masked if password)
        char disp[256];
        if (password)
        {
            for (int i = 0; i < len; i++) disp[i] = '*';
            disp[len] = '\0';
        }
        else
        {
            myStrCopy(disp, buf, 256);
        }

        if (len == 0 && !focused && myStrLen(placeholder) > 0)
        {
            Text pt;
            pt.setFont(gFont);
            pt.setString(placeholder);
            pt.setCharacterSize(14);
            pt.setFillColor(Color(70, 90, 120));
            pt.setPosition(x + 10, y + (h - 16) / 2.f);
            w_.draw(pt);
        }
        else
        {
            Text dt;
            dt.setFont(gFont);
            dt.setString(disp);
            dt.setCharacterSize(14);
            dt.setFillColor(TXT);
            dt.setPosition(x + 10, y + (h - 16) / 2.f);
            w_.draw(dt);
        }
    }

    void clear() { buf[0] = '\0'; len = 0; }
};


// Sort appointment indices by date ascending
static void sortApptByDateAsc(appointment* appts, int* idx, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
        {
            // compare dates DD-MM-YYYY as YYYYMMDD for correct order
            const char* d1 = appts[idx[j]].getDate();
            const char* d2 = appts[idx[j + 1]].getDate();
            // build numeric YYYYMMDD
            auto toNum = [](const char* d) -> int {
                if (myStrLen(d) < 10) return 0;
                int day = (d[0] - '0') * 10 + (d[1] - '0');
                int mon = (d[3] - '0') * 10 + (d[4] - '0');
                int year = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
                return year * 10000 + mon * 100 + day;
                };
            if (toNum(d1) > toNum(d2))
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }
        }
}

// Sort prescription indices by date desceding
static void sortRxByDateDesc(prescription* rx, int* idx, int n)
{
    for (int i = 0; i < n - 1; i++)
        for (int j = 0; j < n - i - 1; j++)
        {
            const char* d1 = rx[idx[j]].getDate();
            const char* d2 = rx[idx[j + 1]].getDate();
            auto toNum = [](const char* d) -> int {
                if (myStrLen(d) < 10) return 0;
                int day = (d[0] - '0') * 10 + (d[1] - '0');
                int mon = (d[3] - '0') * 10 + (d[4] - '0');
                int year = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
                return year * 10000 + mon * 100 + day;
                };
            if (toNum(d1) < toNum(d2))  // descending
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }
        }
}

// ============================================================
//  SCROLLABLE LIST - simple scroll state for list views
// ============================================================
struct ListView
{
    char rows[200][512]; // up to 200 rows, 512 chars each
    Color rowCol[200];
    int   count;
    int   scroll;  // first visible row

    void clear() { count = 0; scroll = 0; }
    bool addRow(const char* txt, Color c = Color(220, 235, 255))
    {
        if (count >= 200) return false;
        myStrCopy(rows[count], txt, 512);
        rowCol[count] = c;
        count++;
        return true;
    }

    void handleScroll(const Event& ev)
    {
        if (ev.type == Event::MouseWheelScrolled)
        {
            scroll -= (int)ev.mouseWheelScroll.delta;
            if (scroll < 0) scroll = 0;
            int maxS = count - 14; // 14 visible rows typically
            if (maxS < 0) maxS = 0;
            if (scroll > maxS) scroll = maxS;
        }
    }

    void draw(RenderWindow& w, float x, float y, float lw, float lh,
        const char* header = "") const
    {
        drawPanel(w, x, y, lw, lh, Color(18, 30, 55));

        float rowH = 28.f;
        float curY = y;

        // header
        if (myStrLen(header) > 0)
        {
            drawPanel(w, x, curY, lw, rowH, Color(13, 22, 45));
            drawText(w, header, x + 10, curY + 6, 13, TEAL);
            curY += rowH;
        }

        int visible = (int)((lh - (myStrLen(header) > 0 ? rowH : 0)) / rowH);
        for (int i = 0; i < visible; i++)
        {
            int ri = scroll + i;
            if (ri >= count) break;

            // alternating rows
            if (ri % 2 == 0)
            {
                RectangleShape bg(Vector2f(lw, rowH - 1));
                bg.setPosition(x, curY);
                bg.setFillColor(Color(22, 36, 63));
                w.draw(bg);
            }

            drawText(w, rows[ri], x + 8, curY + 6, 13, rowCol[ri]);
            curY += rowH;
        }

        // scrollbar hint
        if (count > visible)
        {
            char hint[32]; hint[0] = '\0';
            char cur[8], tot[8];
            intToCharArr(scroll + 1, cur);
            intToCharArr(count, tot);
            myStrCat(hint, cur, 32); myStrCat(hint, "/", 32); myStrCat(hint, tot, 32);
            drawText(w, hint, x + lw - 60, y + 4, 11, TXT2);
        }
    }
};

// =================message bar show error or success meassages=================


struct MsgBar
{
    char  text[256];
    bool  isError;
    Clock timer;
    bool  active;

    void set(const char* msg, bool err = false)
    {
        myStrCopy(text, msg, 256);
        isError = err;
        active = true;
        timer.restart();
    }
    void clear() { active = false; text[0] = '\0'; }

    void draw(RenderWindow& w) const
    {
        if (!active) return;
        Color bg = isError ? Color(180, 30, 30, 220) : Color(20, 120, 90, 220);
        drawPanel(w, 0, WIN_H - 50, WIN_W, 50, bg);
        drawTextCentre(w, text, WIN_W / 2.f, WIN_H - 36, 15, Color::White);
    }

    void update()
    {
        if (active && timer.getElapsedTime().asSeconds() > 4.f)
            active = false;
    }
};


//  ============   SCREEN: LOGIN   ============

struct LoginState
{
    TBox idBox, passBox;
    int  role;        // 0=Patient 1=Doctor 2=Admin
    int  attempts;
    bool locked;
    MsgBar msg;

    void init()
    {
        role = 0; attempts = 0; locked = false;
        idBox.init(WIN_W / 2.f - 150, 280, 300, 40, 20);
        passBox.init(WIN_W / 2.f - 150, 350, 300, 40, 50, true);
        msg.clear();
    }

    void handleEvent(const Event& ev, RenderWindow& win, Screen& screen)
    {
        if (locked) return;
        idBox.handleEvent(ev, win);
        passBox.handleEvent(ev, win);

        // role buttons handled in draw()
    }

    void tryLogin(Screen& screen, MsgBar& msg_)
    {
        if (locked) { msg_.set("Account locked. Contact admin.", true); return; }

        int enteredId = 0;
        for (int i = 0; idBox.buf[i]; i++)
            if (idBox.buf[i] >= '0' && idBox.buf[i] <= '9')
                enteredId = enteredId * 10 + (idBox.buf[i] - '0');

        const char* roleStr = (role == 0) ? "Patient" : (role == 1) ? "Doctor" : "Admin";
        bool ok = false;

        if (role == 0) // Patient
        {
            patient* p = gPatients.findByID(enteredId);
            if (p && myStrEq(p->getPassword(), passBox.buf))
            {
                gPatientID = enteredId;
                ok = true;
                screen = SCR_PATIENT_DASHBOARD;
            }
        }
        else if (role == 1) // Doctor
        {
            doctor* d = gDoctors.findByID(enteredId);
            if (d && myStrEq(d->getPassword(), passBox.buf))
            {
                gDoctorID = enteredId;
                ok = true;
                screen = SCR_DOCTOR_DASHBOARD;
            }
        }
        else // Admin
        {
            if (gAdmin && gAdmin->getId() == enteredId &&
                myStrEq(gAdmin->getPassword(), passBox.buf))
            {
                ok = true;
                screen = SCR_ADMIN_DASHBOARD;
            }
        }

        if (!ok)
        {
            attempts++;
            fh.logSecurityEvent(roleStr, enteredId, "FAILED");
            if (attempts >= 3)
            {
                locked = true;
                fh.logSecurityEvent(roleStr, enteredId, "LOCKED");
                msg_.set("Account locked. Contact admin.", true);
            }
            else
            {
                msg_.set("Invalid ID or password. Try again.", true);
            }
        }
    }

    void draw(RenderWindow& w, const Event& ev, Screen& screen)
    {
        // background
        w.clear(BG);
        drawPanel(w, 0, 0, WIN_W, WIN_H, BG);

        // title
        drawTextCentre(w, "MediCore Hospital Management System",
            WIN_W / 2.f, 80, 26, TEAL);
        drawTextCentre(w, "Login to continue",
            WIN_W / 2.f, 118, 16, TXT2);

        // role selector
        const char* roles[3] = { "Patient","Doctor","Admin" };
        for (int i = 0; i < 3; i++)
        {
            float bx = WIN_W / 2.f - 225 + i * 155;
            Color nc = (role == i) ? TEAL : BTN_DN;
            Color hc = (role == i) ? TEAL_D : BTN_DH;
            Color tc = (role == i) ? Color(10, 18, 35) : TXT2;
            if (drawButton(w, ev, bx, 190, 140, 38, roles[i], nc, hc, tc))
                role = i;
        }

        // ID field
        drawText(w, "Enter ID:", WIN_W / 2.f - 150, 256, 14, TXT2);
        idBox.draw(w, "e.g. 1");

        // Password field
        drawText(w, "Password:", WIN_W / 2.f - 150, 326, 14, TXT2);
        passBox.draw(w, "Enter password");

        // Login button
        if (drawButton(w, ev, WIN_W / 2.f - 100, 415, 200, 44, "LOGIN",
            TEAL, BTN_H, Color(10, 18, 35)) && !locked)
        {
            tryLogin(screen, msg);
        }

        msg.draw(w);
        msg.update();
    }
};

//  ============   PATIENT DASHBOARD   ============

// All patient actions share some sub-state declared here

struct BookState
{
    char spec[80];
    int  doctorId;
    char date[12];
    char slot[8];
    int  step;   // 0=spec 1=docId 2=date 3=slot
    TBox specBox, docIdBox, dateBox, slotBox;
    MsgBar msg;
    ListView docList;
    bool showDocs;

    void init()
    {
        step = 0; doctorId = -1; showDocs = false;
        spec[0] = '\0'; date[0] = '\0'; slot[0] = '\0';
        specBox.init(200, 200, 300, 38, 50);
        docIdBox.init(200, 300, 200, 38, 10);
        dateBox.init(200, 400, 200, 38, 12);
        slotBox.init(200, 480, 120, 38, 6);
        msg.clear();
        docList.clear();
    }

    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        specBox.handleEvent(ev, w);
        docIdBox.handleEvent(ev, w);
        dateBox.handleEvent(ev, w);
        slotBox.handleEvent(ev, w);
        docList.handleScroll(ev);
    }
};

struct CancelState
{
    TBox apptIdBox;
    ListView list;
    MsgBar msg;
    void init()
    {
        apptIdBox.init(200, 450, 200, 38, 10);
        list.clear(); msg.clear();
    }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        apptIdBox.handleEvent(ev, w); list.handleScroll(ev);
    }
};

struct BillState
{
    TBox billIdBox;
    ListView list;
    MsgBar msg;
    float outstanding;
    void init()
    {
        billIdBox.init(200, 530, 200, 38, 10);
        list.clear(); msg.clear(); outstanding = 0;
    }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        billIdBox.handleEvent(ev, w); list.handleScroll(ev);
    }
};

struct TopUpState
{
    TBox amtBox;
    MsgBar msg;
    int attempts;
    void init() { amtBox.init(WIN_W / 2.f - 100, 320, 200, 40, 12); msg.clear(); attempts = 0; }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        amtBox.handleEvent(ev, w);
    }
};

// Doctor sub-states
struct DocMarkState
{
    TBox apptIdBox;
    ListView list;
    MsgBar msg;
    void init() { apptIdBox.init(200, 480, 200, 38, 10); list.clear(); msg.clear(); }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        apptIdBox.handleEvent(ev, w); list.handleScroll(ev);
    }
};

struct PrescriptionState
{
    TBox apptIdBox;
    TBox medBox;
    TBox notesBox;
    MsgBar msg;
    int step; // 0=apptId 1=medicines 2=notes
    void init()
    {
        step = 0;
        apptIdBox.init(200, 220, 200, 38, 10);
        medBox.init(200, 320, 600, 38, 240);
        notesBox.init(200, 400, 600, 38, 150);
        msg.clear();
    }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        apptIdBox.handleEvent(ev, w); medBox.handleEvent(ev, w); notesBox.handleEvent(ev, w);
    }
};

struct DocHistoryState
{
    TBox patIdBox;
    ListView list;
    MsgBar msg;
    void init() { patIdBox.init(200, 220, 200, 38, 10); list.clear(); msg.clear(); }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        patIdBox.handleEvent(ev, w); list.handleScroll(ev);
    }
};

// Admin sub-states
struct AddDoctorState
{
    TBox nameBox, specBox, contBox, passBox, feeBox;
    MsgBar msg;
    void init()
    {
        nameBox.init(350, 170, 300, 36, 52);
        specBox.init(350, 225, 300, 36, 52);
        contBox.init(350, 280, 300, 36, 13);
        passBox.init(350, 335, 300, 36, 52, true);
        feeBox.init(350, 390, 200, 36, 12);
        msg.clear();
    }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        nameBox.handleEvent(ev, w); specBox.handleEvent(ev, w);
        contBox.handleEvent(ev, w); passBox.handleEvent(ev, w);
        feeBox.handleEvent(ev, w);
    }
};

struct RemoveDoctorState
{
    TBox docIdBox;
    ListView list;
    MsgBar msg;
    void init() { docIdBox.init(200, 500, 200, 38, 10); list.clear(); msg.clear(); }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        docIdBox.handleEvent(ev, w); list.handleScroll(ev);
    }
};

struct DischargeState
{
    TBox patIdBox;
    MsgBar msg;
    void init() { patIdBox.init(WIN_W / 2.f - 100, 300, 200, 38, 10); msg.clear(); }
    void handleEvent(const Event& ev, const RenderWindow& w)
    {
        patIdBox.handleEvent(ev, w);
    }
};


//  Persistent sub-state objects (one of each kind)

LoginState      loginSt;
BookState       bookSt;
CancelState     cancelSt;
BillState       billSt;
TopUpState      topUpSt;
DocMarkState    docMarkSt;
PrescriptionState prescSt;
DocHistoryState docHistSt;
AddDoctorState  addDocSt;
RemoveDoctorState removeDocSt;
DischargeState  dischargeSt;

ListView   gList;   // generic reusable list for view screens
MsgBar     gMsg;    // generic reusable message bar


//  HELPER: build a single row string for a list

static void buildApptRow(const appointment& a, char* out, int maxLen,
    const char* docName = nullptr,
    const char* patName = nullptr)
{
    out[0] = '\0';
    char idBuf[8];
    intToCharArr(a.getAppointmentID(), idBuf);
    myStrCat(out, "ID:", maxLen); myStrCat(out, idBuf, maxLen);
    myStrCat(out, "  ", maxLen);
    if (patName) { myStrCat(out, "Pat:", maxLen); myStrCat(out, patName, maxLen); myStrCat(out, "  ", maxLen); }
    if (docName) { myStrCat(out, "Dr:", maxLen);  myStrCat(out, docName, maxLen); myStrCat(out, "  ", maxLen); }
    myStrCat(out, a.getDate(), maxLen); myStrCat(out, "  ", maxLen);
    myStrCat(out, a.getTimeSlot(), maxLen); myStrCat(out, "  ", maxLen);
    myStrCat(out, a.getStatus(), maxLen);
}

// ============================================================
//  PATIENT: BOOK APPOINTMENT logic
// ============================================================
static void doBookAppointment(BookState& bs, Screen& screen)
{
    // Step 1: search by specialization
    if (bs.step == 0)
    {
        // user pressed Search button
        myStrCopy(bs.spec, bs.specBox.buf, 80);
        bs.docList.clear();
        bool found = false;
        for (int i = 0; i < gDoctors.size(); i++)
        {
            doctor& d = gDoctors.getAll()[i];
            if (myStrEqCI(d.getSpecialization(), bs.spec))
            {
                char row[256]; row[0] = '\0';
                char idB[8], feeB[20];
                intToCharArr(d.getId(), idB);
                floatToCharArr(d.getFee(), feeB);
                myStrCat(row, "ID:", 256); myStrCat(row, idB, 256);
                myStrCat(row, "  ", 256); myStrCat(row, d.getName(), 256);
                myStrCat(row, "  Fee:PKR ", 256); myStrCat(row, feeB, 256);
                bs.docList.addRow(row);
                found = true;
            }
        }
        if (!found)
        {
            bs.docList.addRow("No doctors available for that specialization.", AMBER);
        }
        bs.showDocs = true;
        bs.step = 1;
    }
    else if (bs.step == 1)
    {
        // user entered doctor ID
        int did = 0;
        for (int i = 0; bs.docIdBox.buf[i]; i++)
            if (bs.docIdBox.buf[i] >= '0' && bs.docIdBox.buf[i] <= '9')
                did = did * 10 + (bs.docIdBox.buf[i] - '0');
        if (!gDoctors.findByID(did))
        {
            bs.msg.set("Doctor not found.", true);
            return;
        }
        bs.doctorId = did;
        bs.step = 2;
    }
    else if (bs.step == 2)
    {
        // user entered date
        myStrCopy(bs.date, bs.dateBox.buf, 12);
        if (!Validator::validateDate(bs.date))
        {
            bs.msg.set("Invalid date. Use DD-MM-YYYY, current year or later.", true);
            return;
        }
        bs.step = 3;
    }
    else if (bs.step == 3)
    {
        // user entered slot
        myStrCopy(bs.slot, bs.slotBox.buf, 8);
        if (!Validator::validateTimeSlot(bs.slot))
        {
            bs.msg.set("Invalid time slot. Use 09:00, 10:00 ... 16:00", true);
            return;
        }

        // check if slot is already taken
        for (int i = 0; i < gAppointments.size(); i++)
        {
            appointment& a = gAppointments.getAll()[i];
            if (a.getDoctorID() == bs.doctorId &&
                myStrEq(a.getDate(), bs.date) &&
                myStrEq(a.getTimeSlot(), bs.slot) &&
                a.getStatus()[0] != 'c')
            {
                try { throw SlotUnavailableException(); }
                catch (SlotUnavailableException& ex)
                {
                    bs.msg.set(ex.what(), true);
                    return;
                }
            }
        }

        // check patient balance
        patient* p = gPatients.findByID(gPatientID);
        doctor* d = gDoctors.findByID(bs.doctorId);
        if (!p || !d) return;

        if (p->getBalance() < d->getFee())
        {
            try { throw InsufficientFundsException(); }
            catch (InsufficientFundsException& ex)
            {
                bs.msg.set(ex.what(), true);
                return;
            }
        }

        // deduct fee using -= operator
        *p -= d->getFee();
        fh.updatePatient(*p);

        // get today's date for bill
        char today[12]; getTodayStr(today);

        // generate new appointment ID
        int newAId = fh.getNextID("appointments.txt", 0);
        appointment newA(newAId, gPatientID, bs.doctorId,
            bs.date, bs.slot, "pending");
        gAppointments.add(newA);
        fh.addAppointment(newA);

        // generate bill
        int newBId = fh.getNextID("bills.txt", 0);
        bill newB(newBId, gPatientID, newAId, d->getFee(), "unpaid", today);
        gBills.add(newB);
        fh.addBill(newB);

        char success[128]; success[0] = '\0';
        myStrCat(success, "Appointment booked! ID:", 128);
        char idBuf[8]; intToCharArr(newAId, idBuf);
        myStrCat(success, idBuf, 128);
        bs.msg.set(success, false);
        bs.step = 0;
        bs.showDocs = false;
        bs.docList.clear();
        bs.specBox.clear(); bs.docIdBox.clear();
        bs.dateBox.clear(); bs.slotBox.clear();
    }
}

// ============================================================
//  PATIENT: CANCEL APPOINTMENT logic
// ============================================================
static void doCancelAppointment(CancelState& cs)
{
    int apptId = 0;
    for (int i = 0; cs.apptIdBox.buf[i]; i++)
        if (cs.apptIdBox.buf[i] >= '0' && cs.apptIdBox.buf[i] <= '9')
            apptId = apptId * 10 + (cs.apptIdBox.buf[i] - '0');

    appointment* a = gAppointments.findByID(apptId);
    if (!a || a->getPatientID() != gPatientID || !myStrEq(a->getStatus(), "pending"))
    {
        cs.msg.set("Invalid appointment ID or not yours / not pending.", true);
        return;
    }

    // find fee to refund
    doctor* d = gDoctors.findByID(a->getDoctorID());
    float fee = d ? d->getFee() : 0.f;

    // cancel the appointment
    a->setStatus("cancelled");
    fh.updateAppointment(*a);

    // refund using += operator
    patient* p = gPatients.findByID(gPatientID);
    if (p)
    {
        *p += fee;
        fh.updatePatient(*p);
    }

    // cancel the bill
    for (int i = 0; i < gBills.size(); i++)
    {
        bill& b = gBills.getAll()[i];
        if (b.getAppointId() == apptId)
        {
            b.setStatus("cancelled");
            fh.updateBill(b);
            break;
        }
    }

    char ok[80]; ok[0] = '\0';
    myStrCat(ok, "Cancelled. PKR ", 80);
    char feeBuf[20]; floatToCharArr(fee, feeBuf);
    myStrCat(ok, feeBuf, 80);
    myStrCat(ok, " refunded.", 80);
    cs.msg.set(ok, false);

    // refresh list
    cs.list.clear();
    for (int i = 0; i < gAppointments.size(); i++)
    {
        appointment& ap = gAppointments.getAll()[i];
        if (ap.getPatientID() == gPatientID && myStrEq(ap.getStatus(), "pending"))
        {
            char row[256];
            doctor* doc = gDoctors.findByID(ap.getDoctorID());
            const char* dn = doc ? doc->getName() : "Unknown";
            buildApptRow(ap, row, 256, dn, nullptr);
            cs.list.addRow(row);
        }
    }
    cs.apptIdBox.clear();
}

// ============================================================
//  PATIENT: PAY BILL logic
// ============================================================
static void doPayBill(BillState& bs)
{
    int billId = 0;
    for (int i = 0; bs.billIdBox.buf[i]; i++)
        if (bs.billIdBox.buf[i] >= '0' && bs.billIdBox.buf[i] <= '9')
            billId = billId * 10 + (bs.billIdBox.buf[i] - '0');

    bill* b = gBills.findByID(billId);
    if (!b || b->getPatientId() != gPatientID || !myStrEq(b->getStatus(), "unpaid"))
    {
        bs.msg.set("Invalid Bill ID or bill is not yours/already paid.", true);
        return;
    }

    patient* p = gPatients.findByID(gPatientID);
    if (!p) return;

    if (p->getBalance() < b->getAmount())
    {
        try { throw InsufficientFundsException(); }
        catch (InsufficientFundsException& ex)
        {
            bs.msg.set(ex.what(), true);
            return;
        }
    }

    *p -= b->getAmount();
    fh.updatePatient(*p);
    b->setStatus("paid");
    fh.updateBill(*b);

    char ok[80]; ok[0] = '\0';
    myStrCat(ok, "Bill paid! Remaining: PKR ", 80);
    char balBuf[20]; floatToCharArr(p->getBalance(), balBuf);
    myStrCat(ok, balBuf, 80);
    bs.msg.set(ok, false);
    bs.billIdBox.clear();

    // refresh list
    bs.list.clear(); bs.outstanding = 0;
    for (int i = 0; i < gBills.size(); i++)
    {
        bill& bl = gBills.getAll()[i];
        if (bl.getPatientId() == gPatientID && myStrEq(bl.getStatus(), "unpaid"))
        {
            char row[256]; row[0] = '\0';
            char idB[8], amtB[20];
            intToCharArr(bl.getBillId(), idB);
            floatToCharArr(bl.getAmount(), amtB);
            myStrCat(row, "BillID:", 256); myStrCat(row, idB, 256);
            myStrCat(row, "  PKR:", 256);  myStrCat(row, amtB, 256);
            myStrCat(row, "  ", 256);      myStrCat(row, bl.getDate(), 256);
            bs.list.addRow(row);
            bs.outstanding += bl.getAmount();
        }
    }
}

// ============================================================
//  BUILD LISTS  (called when entering a screen)
// ============================================================
static void buildViewApptList(ListView& lv)
{
    lv.clear();
    // gather indices for this patient
    int idx[100]; int cnt = 0;
    for (int i = 0; i < gAppointments.size() && cnt < 100; i++)
        if (gAppointments.getAll()[i].getPatientID() == gPatientID)
            idx[cnt++] = i;

    sortApptByDateAsc(gAppointments.getAll(), idx, cnt);

    for (int i = 0; i < cnt; i++)
    {
        appointment& a = gAppointments.getAll()[idx[i]];
        doctor* d = gDoctors.findByID(a.getDoctorID());
        const char* dn = d ? d->getName() : "Unknown";
        const char* ds = d ? d->getSpecialization() : "";
        char row[512]; row[0] = '\0';
        char idB[8]; intToCharArr(a.getAppointmentID(), idB);
        myStrCat(row, "ID:", 512); myStrCat(row, idB, 512);
        myStrCat(row, "  Dr:", 512); myStrCat(row, dn, 512);
        myStrCat(row, "  [", 512); myStrCat(row, ds, 512); myStrCat(row, "]", 512);
        myStrCat(row, "  ", 512); myStrCat(row, a.getDate(), 512);
        myStrCat(row, "  ", 512); myStrCat(row, a.getTimeSlot(), 512);
        myStrCat(row, "  ", 512); myStrCat(row, a.getStatus(), 512);
        Color rc = myStrEq(a.getStatus(), "completed") ? SUCCESS :
            myStrEq(a.getStatus(), "cancelled") ? TXT2 :
            myStrEq(a.getStatus(), "noshow") ? DANGER : TXT;
        lv.addRow(row, rc);
    }
    if (lv.count == 0) lv.addRow("No appointments found.", TXT2);
}

static void buildViewRecordsList(ListView& lv)
{
    lv.clear();
    int idx[100]; int cnt = 0;
    for (int i = 0; i < gPrescriptions.size() && cnt < 100; i++)
        if (gPrescriptions.getAll()[i].getPatientId() == gPatientID)
            idx[cnt++] = i;

    sortRxByDateDesc(gPrescriptions.getAll(), idx, cnt);

    for (int i = 0; i < cnt; i++)
    {
        prescription& rx = gPrescriptions.getAll()[idx[i]];
        doctor* d = gDoctors.findByID(rx.getDoctorId());
        const char* dn = d ? d->getName() : "Unknown";
        char row[512]; row[0] = '\0';
        myStrCat(row, rx.getDate(), 512);
        myStrCat(row, "  Dr:", 512); myStrCat(row, dn, 512);
        myStrCat(row, "  Meds: ", 512); myStrCat(row, rx.getMedicine(), 512);
        lv.addRow(row);
        // notes on next row
        char notesRow[512]; notesRow[0] = '\0';
        myStrCat(notesRow, "   Notes: ", 512); myStrCat(notesRow, rx.getNotes(), 512);
        lv.addRow(notesRow, TXT2);
    }
    if (lv.count == 0) lv.addRow("No medical records found.", TXT2);
}

static void buildBillsList(BillState& bs)
{
    bs.list.clear(); bs.outstanding = 0;
    for (int i = 0; i < gBills.size(); i++)
    {
        bill& b = gBills.getAll()[i];
        if (b.getPatientId() != gPatientID) continue;
        char row[256]; row[0] = '\0';
        char bId[8], apptId[8], amtBuf[20];
        intToCharArr(b.getBillId(), bId);
        intToCharArr(b.getAppointId(), apptId);
        floatToCharArr(b.getAmount(), amtBuf);
        myStrCat(row, "Bill:", 256); myStrCat(row, bId, 256);
        myStrCat(row, "  Appt:", 256); myStrCat(row, apptId, 256);
        myStrCat(row, "  PKR:", 256);  myStrCat(row, amtBuf, 256);
        myStrCat(row, "  ", 256);      myStrCat(row, b.getStatus(), 256);
        myStrCat(row, "  ", 256);      myStrCat(row, b.getDate(), 256);
        Color rc = myStrEq(b.getStatus(), "paid") ? SUCCESS :
            myStrEq(b.getStatus(), "cancelled") ? TXT2 : AMBER;
        bs.list.addRow(row, rc);
        if (myStrEq(b.getStatus(), "unpaid")) bs.outstanding += b.getAmount();
    }
    if (bs.list.count == 0) bs.list.addRow("No bills found.", TXT2);
}

static void buildCancelList(CancelState& cs)
{
    cs.list.clear();
    for (int i = 0; i < gAppointments.size(); i++)
    {
        appointment& a = gAppointments.getAll()[i];
        if (a.getPatientID() == gPatientID && myStrEq(a.getStatus(), "pending"))
        {
            doctor* d = gDoctors.findByID(a.getDoctorID());
            const char* dn = d ? d->getName() : "Unknown";
            char row[256]; buildApptRow(a, row, 256, dn, nullptr);
            cs.list.addRow(row);
        }
    }
    if (cs.list.count == 0) cs.list.addRow("No pending appointments.", TXT2);
}

static void buildDocTodayList(ListView& lv)
{
    lv.clear();
    char today[12]; getTodayStr(today);
    // gather and sort by time slot
    int idx[100]; int cnt = 0;
    for (int i = 0; i < gAppointments.size() && cnt < 100; i++)
    {
        appointment& a = gAppointments.getAll()[i];
        if (a.getDoctorID() == gDoctorID && myStrEq(a.getDate(), today))
            idx[cnt++] = i;
    }
    // sort by time slot ascending (simple: slot string is already sortable)
    for (int i = 0; i < cnt - 1; i++)
        for (int j = 0; j < cnt - i - 1; j++)
        {
            const char* t1 = gAppointments.getAll()[idx[j]].getTimeSlot();
            const char* t2 = gAppointments.getAll()[idx[j + 1]].getTimeSlot();
            // compare as strings - they are HH:MM so lexicographic = chronological
            int k = 0;
            while (t1[k] && t2[k] && t1[k] == t2[k]) k++;
            if (t1[k] > t2[k])
            {
                int t = idx[j]; idx[j] = idx[j + 1]; idx[j + 1] = t;
            }
        }

    for (int i = 0; i < cnt; i++)
    {
        appointment& a = gAppointments.getAll()[idx[i]];
        patient* p = gPatients.findByID(a.getPatientID());
        const char* pn = p ? p->getName() : "Unknown";
        char row[256];
        buildApptRow(a, row, 256, nullptr, pn);
        Color rc = myStrEq(a.getStatus(), "completed") ? SUCCESS :
            myStrEq(a.getStatus(), "noshow") ? DANGER : TXT;
        lv.addRow(row, rc);
    }
    if (lv.count == 0) lv.addRow("No appointments scheduled for today.", TXT2);
}

// ============================================================
//  MAIN
// ============================================================
int main()
{
    // --- Load font ---
    // try the bundled Astro.ttf first, fallback to system font
    if (!gFont.loadFromFile("Astro.ttf"))
        gFont.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");

    // --- Load all data from files ---
    gPatients = fh.loadPatients();
    gDoctors = fh.loadDoctors();
    gAppointments = fh.loadAppointments();
    gPrescriptions = fh.loadPrescriptions();
    gBills = fh.loadBills();
    gAdmin = fh.loadAdmin();

    // --- Create window ---
    RenderWindow window(VideoMode(WIN_W, WIN_H),
        "MediCore Hospital Management System",
        Style::Close | Style::Titlebar);
    window.setFramerateLimit(60);

    // --- Initialise all sub-states ---
    loginSt.init();
    bookSt.init();
    cancelSt.init();
    billSt.init();
    topUpSt.init();
    docMarkSt.init();
    prescSt.init();
    docHistSt.init();
    addDocSt.init();
    removeDocSt.init();
    dischargeSt.init();

    Event ev;

    // ============================================================
    //  MAIN LOOP
    // ============================================================
    while (window.isOpen())
    {
        // default event so we don't have stale clicks
        ev.type = Event::Count; // neutral

        while (window.pollEvent(ev))
        {
            if (ev.type == Event::Closed)
                window.close();
        }

        window.clear(BG);

        // --------------------------------------------------------
        //  SCREEN DISPATCH
        // --------------------------------------------------------
        switch (gScreen)
        {
            // ====================================================
            //  LOGIN
            // ====================================================
        case SCR_LOGIN:
        {
            loginSt.handleEvent(ev, window, gScreen);
            loginSt.draw(window, ev, gScreen);
            break;
        }

        // ====================================================
        //  PATIENT DASHBOARD
        // ====================================================
        case SCR_PATIENT_DASHBOARD:
        {
            patient* p = gPatients.findByID(gPatientID);
            if (!p) { gScreen = SCR_LOGIN; break; }

            drawHeader(window, "Patient Dashboard", p->getName());

            // balance
            char balBuf[40]; balBuf[0] = '\0';
            myStrCat(balBuf, "Balance: PKR ", 40);
            char bal[20]; floatToCharArr(p->getBalance(), bal);
            myStrCat(balBuf, bal, 40);
            drawText(window, balBuf, 20, 70, 14, AMBER);

            // menu buttons
            const char* labels[8] = {
                "Book Appointment", "Cancel Appointment", "View Appointments",
                "View Medical Records", "View Bills", "Pay Bill",
                "Top Up Balance", "Logout"
            };
            float bx = 80, by = 130, bw = 220, bh = 50, gap = 14;
            for (int i = 0; i < 8; i++)
            {
                float x = bx + (i % 4) * (bw + gap);
                float y = by + (i / 4) * (bh + gap);
                Color nc = (i == 7) ? RED_N : BTN_N;
                Color hc = (i == 7) ? RED_H : BTN_H;
                if (drawButton(window, ev, x, y, bw, bh, labels[i], nc, hc))
                {
                    switch (i)
                    {
                    case 0: bookSt.init();   gScreen = SCR_BOOK_APPOINTMENT;   break;
                    case 1: cancelSt.init(); buildCancelList(cancelSt);
                        gScreen = SCR_CANCEL_APPOINTMENT;  break;
                    case 2: buildViewApptList(gList);
                        gScreen = SCR_VIEW_APPOINTMENTS;   break;
                    case 3: buildViewRecordsList(gList);
                        gScreen = SCR_VIEW_RECORDS;        break;
                    case 4: buildBillsList(billSt);
                        gScreen = SCR_VIEW_BILLS;          break;
                    case 5: buildBillsList(billSt);
                        billSt.list.clear();
                        // show only unpaid
                        for (int j = 0; j < gBills.size(); j++) {
                            bill& b = gBills.getAll()[j];
                            if (b.getPatientId() == gPatientID && myStrEq(b.getStatus(), "unpaid")) {
                                char row[256]; row[0] = '\0';
                                char bId[8], amtB[20];
                                intToCharArr(b.getBillId(), bId);
                                floatToCharArr(b.getAmount(), amtB);
                                myStrCat(row, "BillID:", 256); myStrCat(row, bId, 256);
                                myStrCat(row, "  PKR:", 256); myStrCat(row, amtB, 256);
                                myStrCat(row, "  ", 256); myStrCat(row, b.getDate(), 256);
                                billSt.list.addRow(row, AMBER);
                            }
                        }
                        if (billSt.list.count == 0)billSt.list.addRow("No unpaid bills.", TXT2);
                        gScreen = SCR_PAY_BILL;            break;
                    case 6: topUpSt.init(); gScreen = SCR_TOPUP;              break;
                    case 7: gPatientID = -1;  gScreen = SCR_LOGIN;
                        loginSt.init(); break;
                    }
                }
            }
            break;
        }

        // ====================================================
        //  BOOK APPOINTMENT
        // ====================================================
        case SCR_BOOK_APPOINTMENT:
        {
            drawHeader(window, "Book Appointment");
            bookSt.handleEvent(ev, window);

            float lx = 60;
            drawText(window, "Step 1: Enter Specialization", lx, 80, 15, TXT2);
            drawText(window, "Specialization:", lx, 168, 14, TXT2);
            bookSt.specBox.draw(window, "e.g. Cardiology");

            if (drawButton(window, ev, 520, 195, 140, 36, "Search", TEAL, BTN_H))
            {
                bookSt.step = 0;
                doBookAppointment(bookSt, gScreen);
            }

            if (bookSt.showDocs)
            {
                bookSt.docList.draw(window, lx, 250, 900, 180, "Available Doctors:");

                drawText(window, "Step 2: Enter Doctor ID", lx, 440, 15, TXT2);
                bookSt.docIdBox.draw(window, "Doctor ID");
                if (drawButton(window, ev, 420, 437, 120, 36, "Next", TEAL, BTN_H))
                {
                    bookSt.step = 1; doBookAppointment(bookSt, gScreen);
                }

                if (bookSt.step >= 2)
                {
                    drawText(window, "Step 3: Enter Date (DD-MM-YYYY)", lx, 490, 15, TXT2);
                    bookSt.dateBox.draw(window, "DD-MM-YYYY");
                    if (drawButton(window, ev, 420, 487, 120, 36, "Next", TEAL, BTN_H))
                    {
                        bookSt.step = 2; doBookAppointment(bookSt, gScreen);
                    }
                }
                if (bookSt.step >= 3)
                {
                    drawText(window, "Step 4: Enter Time Slot", lx, 545, 15, TXT2);
                    bookSt.slotBox.draw(window, "09:00");
                    // show available slots
                    const char* slots[8] = { "09:00","10:00","11:00","12:00","13:00","14:00","15:00","16:00" };
                    char sline[256]; sline[0] = '\0'; myStrCat(sline, "Available: ", 256);
                    for (int i = 0; i < 8; i++)
                    {
                        bool taken = false;
                        for (int j = 0; j < gAppointments.size(); j++)
                        {
                            appointment& a = gAppointments.getAll()[j];
                            if (a.getDoctorID() == bookSt.doctorId &&
                                myStrEq(a.getDate(), bookSt.date) &&
                                myStrEq(a.getTimeSlot(), slots[i]) &&
                                a.getStatus()[0] != 'c')
                            {
                                taken = true; break;
                            }
                        }
                        if (!taken) { myStrCat(sline, slots[i], 256); myStrCat(sline, " ", 256); }
                    }
                    drawText(window, sline, lx, 572, 13, SUCCESS);
                    if (drawButton(window, ev, 340, 542, 140, 36, "Confirm Book", SUCCESS, Color(70, 230, 170)))
                    {
                        bookSt.step = 3; doBookAppointment(bookSt, gScreen);
                    }
                }
            }

            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;

            bookSt.msg.draw(window);
            bookSt.msg.update();
            break;
        }

        // ====================================================
        //  CANCEL APPOINTMENT
        // ====================================================
        case SCR_CANCEL_APPOINTMENT:
        {
            drawHeader(window, "Cancel Appointment");
            cancelSt.handleEvent(ev, window);

            cancelSt.list.draw(window, 60, 80, 980, 350, "Your Pending Appointments:");

            drawText(window, "Enter Appointment ID to cancel:", 60, 450, 14, TXT2);
            cancelSt.apptIdBox.draw(window, "Appointment ID");
            if (drawButton(window, ev, 420, 447, 160, 36, "Cancel Appointment", DANGER, RED_H, TXT))
                doCancelAppointment(cancelSt);

            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;

            cancelSt.msg.draw(window); cancelSt.msg.update();
            break;
        }

        // ====================================================
        //  VIEW APPOINTMENTS
        // ====================================================
        case SCR_VIEW_APPOINTMENTS:
        {
            drawHeader(window, "My Appointments (sorted by date)");
            gList.handleScroll(ev);
            gList.draw(window, 40, 80, 1020, 540,
                "ID  |  Doctor  |  Specialization  |  Date  |  Slot  |  Status");
            if (drawButton(window, ev, 20, 635, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;
            break;
        }

        // ====================================================
        //  VIEW MEDICAL RECORDS
        // ====================================================
        case SCR_VIEW_RECORDS:
        {
            drawHeader(window, "My Medical Records (most recent first)");
            gList.handleScroll(ev);
            gList.draw(window, 40, 80, 1020, 540,
                "Date  |  Doctor  |  Medicines  |  Notes");
            if (drawButton(window, ev, 20, 635, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;
            break;
        }

        // ====================================================
        //  VIEW BILLS
        // ====================================================
        case SCR_VIEW_BILLS:
        {
            drawHeader(window, "My Bills");
            billSt.list.handleScroll(ev);
            billSt.list.draw(window, 40, 80, 1020, 500, "BillID | ApptID | Amount | Status | Date");
            char outBuf[64]; outBuf[0] = '\0';
            myStrCat(outBuf, "Total Outstanding: PKR ", 64);
            char ob[20]; floatToCharArr(billSt.outstanding, ob);
            myStrCat(outBuf, ob, 64);
            drawText(window, outBuf, 40, 595, 15, AMBER);
            if (drawButton(window, ev, 20, 635, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;
            break;
        }

        // ====================================================
        //  PAY BILL
        // ====================================================
        case SCR_PAY_BILL:
        {
            drawHeader(window, "Pay Bill");
            billSt.handleEvent(ev, window);
            billSt.list.draw(window, 40, 80, 1020, 430, "Unpaid Bills: BillID | Amount | Date");
            drawText(window, "Enter Bill ID to pay:", 40, 525, 14, TXT2);
            billSt.billIdBox.draw(window, "Bill ID");
            if (drawButton(window, ev, 420, 522, 120, 36, "Pay", SUCCESS, Color(70, 230, 170)))
                doPayBill(billSt);
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;
            billSt.msg.draw(window); billSt.msg.update();
            break;
        }

        // ====================================================
        //  TOP UP BALANCE
        // ====================================================
        case SCR_TOPUP:
        {
            drawHeader(window, "Top Up Balance");
            topUpSt.handleEvent(ev, window);

            patient* p = gPatients.findByID(gPatientID);
            if (p)
            {
                char balLine[64]; balLine[0] = '\0';
                myStrCat(balLine, "Current Balance: PKR ", 64);
                char bal[20]; floatToCharArr(p->getBalance(), bal);
                myStrCat(balLine, bal, 64);
                drawTextCentre(window, balLine, WIN_W / 2.f, 250, 18, AMBER);
            }
            drawTextCentre(window, "Enter amount to add (PKR):", WIN_W / 2.f, 295, 14, TXT2);
            topUpSt.amtBox.draw(window, "e.g. 1000");

            if (drawButton(window, ev, WIN_W / 2.f - 80, 380, 160, 44, "Add Balance", TEAL, BTN_H))
            {
                float amt = 0;
                const char* ab = topUpSt.amtBox.buf;
                bool hasDot = false;
                float frac = 0; float div = 1;
                for (int i = 0; ab[i]; i++)
                {
                    if (ab[i] == '.') { hasDot = true; continue; }
                    if (ab[i] < '0' || ab[i]>'9') { amt = -1; break; }
                    if (!hasDot) amt = amt * 10 + (ab[i] - '0');
                    else { div *= 10; frac += (ab[i] - '0') / div; }
                }
                amt += frac;

                try
                {
                    if (amt <= 0) throw InvalidInputException("Amount must be greater than 0.");
                    patient* p2 = gPatients.findByID(gPatientID);
                    if (p2)
                    {
                        *p2 += amt;
                        fh.updatePatient(*p2);
                        char ok[80]; ok[0] = '\0';
                        myStrCat(ok, "Balance updated! New: PKR ", 80);
                        char nb[20]; floatToCharArr(p2->getBalance(), nb);
                        myStrCat(ok, nb, 80);
                        topUpSt.msg.set(ok, false);
                        topUpSt.amtBox.clear();
                    }
                }
                catch (InvalidInputException& ex)
                {
                    topUpSt.attempts++;
                    topUpSt.msg.set(ex.what(), true);
                    if (topUpSt.attempts >= 3)
                        gScreen = SCR_PATIENT_DASHBOARD;
                }
            }
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_PATIENT_DASHBOARD;
            topUpSt.msg.draw(window); topUpSt.msg.update();
            break;
        }

        // ====================================================
        //  DOCTOR DASHBOARD
        // ====================================================
        case SCR_DOCTOR_DASHBOARD:
        {
            doctor* d = gDoctors.findByID(gDoctorID);
            if (!d) { gScreen = SCR_LOGIN; break; }

            char sub[120]; sub[0] = '\0';
            myStrCat(sub, "Dr. ", 120); myStrCat(sub, d->getName(), 120);
            myStrCat(sub, "  |  Spec: ", 120);
            myStrCat(sub, d->getSpecialization(), 120);
            drawHeader(window, "Doctor Dashboard", sub);

            const char* labels[6] = {
                "View Today's Appts","Mark Complete","Mark No-Show",
                "Write Prescription","Patient History","Logout"
            };
            float bw = 220, bh = 52, gap = 14;
            float startX = (WIN_W - (3 * (bw + gap) - gap)) / 2.f;
            for (int i = 0; i < 6; i++)
            {
                float x = startX + (i % 3) * (bw + gap);
                float y = 160 + (i / 3) * (bh + gap);
                Color nc = (i == 5) ? RED_N : BTN_DN;
                Color hc = (i == 5) ? RED_H : BTN_DH;
                if (drawButton(window, ev, x, y, bw, bh, labels[i], nc, hc, TXT))
                {
                    switch (i)
                    {
                    case 0: buildDocTodayList(gList); gScreen = SCR_DOC_TODAY; break;
                    case 1: docMarkSt.init(); buildDocTodayList(docMarkSt.list);
                        gScreen = SCR_DOC_MARK; break;
                    case 2: docMarkSt.init(); buildDocTodayList(docMarkSt.list);
                        gScreen = SCR_DOC_MARK; break;
                    case 3: prescSt.init(); gScreen = SCR_DOC_PRESCRIPTION; break;
                    case 4: docHistSt.init(); gScreen = SCR_DOC_HISTORY; break;
                    case 5: gDoctorID = -1; gScreen = SCR_LOGIN; loginSt.init(); break;
                    }
                }
            }
            break;
        }

        // ===================================================
        //  DOCTOR: VIEW TODAY
        // ===================================================
        case SCR_DOC_TODAY:
        {
            char today[12]; getTodayStr(today);
            char hdr[64]; hdr[0] = '\0';
            myStrCat(hdr, "Today's Appointments  (", 64);
            myStrCat(hdr, today, 64); myStrCat(hdr, ")", 64);
            drawHeader(window, hdr);
            gList.handleScroll(ev);
            gList.draw(window, 40, 80, 1020, 540,
                "ID | Patient | Date | Slot | Status");
            if (drawButton(window, ev, 20, 635, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_DOCTOR_DASHBOARD;
            break;
        }

        // ====================================================
        //  DOCTOR: MARK COMPLETE / NO-SHOW
        // ====================================================
        case SCR_DOC_MARK:
        {
            drawHeader(window, "Mark Appointment");
            docMarkSt.handleEvent(ev, window);
            docMarkSt.list.draw(window, 40, 80, 1020, 340, "Today's Appointments:");
            drawText(window, "Enter Appointment ID:", 40, 438, 14, TXT2);
            docMarkSt.apptIdBox.draw(window, "Appointment ID");

            auto doMark = [&](const char* newStatus)
                {
                    int apptId = 0;
                    for (int i = 0; docMarkSt.apptIdBox.buf[i]; i++)
                        if (docMarkSt.apptIdBox.buf[i] >= '0' && docMarkSt.apptIdBox.buf[i] <= '9')
                            apptId = apptId * 10 + (docMarkSt.apptIdBox.buf[i] - '0');

                    char today[12]; getTodayStr(today);
                    appointment* a = gAppointments.findByID(apptId);
                    if (!a || a->getDoctorID() != gDoctorID ||
                        !myStrEq(a->getStatus(), "pending") ||
                        !myStrEq(a->getDate(), today))
                    {
                        docMarkSt.msg.set("Invalid ID or not yours/pending/today.", true);
                        return;
                    }
                    a->setStatus(newStatus);
                    fh.updateAppointment(*a);

                    if (myStrEq(newStatus, "noshow"))
                    {
                        // cancel the bill...no refund
                        for (int i = 0; i < gBills.size(); i++) {
                            bill& b = gBills.getAll()[i];
                            if (b.getAppointId() == apptId)
                            {
                                b.setStatus("cancelled"); fh.updateBill(b); break;
                            }
                        }
                    }

                    char ok[64]; ok[0] = '\0';
                    myStrCat(ok, "Marked as ", 64); myStrCat(ok, newStatus, 64);
                    docMarkSt.msg.set(ok, false);
                    buildDocTodayList(docMarkSt.list);
                    docMarkSt.apptIdBox.clear();
                };

            if (drawButton(window, ev, 420, 435, 160, 36, "Mark Complete", SUCCESS, Color(70, 230, 170)))
                doMark("completed");
            if (drawButton(window, ev, 600, 435, 160, 36, "Mark No-Show", DANGER, RED_H, TXT))
                doMark("noshow");
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_DOCTOR_DASHBOARD;
            docMarkSt.msg.draw(window); docMarkSt.msg.update();
            break;
        }

        // ====================================================
        //  DOCTOR: WRITE PRESCRIPTION
        // ====================================================
        case SCR_DOC_PRESCRIPTION:
        {
            drawHeader(window, "Write Prescription");
            prescSt.handleEvent(ev, window);

            drawText(window, "Appointment ID (must be completed by you):", 60, 170, 14, TXT2);
            prescSt.apptIdBox.draw(window, "Appointment ID");

            drawText(window, "Medicines (e.g. Paracetamol 500mg;Amoxicillin 250mg):", 60, 270, 14, TXT2);
            prescSt.medBox.draw(window, "Medicine Dosage;Medicine2 Dosage...");

            drawText(window, "Notes (max 150 chars):", 60, 350, 14, TXT2);
            prescSt.notesBox.draw(window, "e.g. Take after meals");

            if (drawButton(window, ev, 60, 455, 180, 42, "Save Prescription", SUCCESS, Color(70, 230, 170)))
            {
                // validate appointment
                int apptId = 0;
                for (int i = 0; prescSt.apptIdBox.buf[i]; i++)
                    if (prescSt.apptIdBox.buf[i] >= '0' && prescSt.apptIdBox.buf[i] <= '9')
                        apptId = apptId * 10 + (prescSt.apptIdBox.buf[i] - '0');

                appointment* a = gAppointments.findByID(apptId);
                if (!a || a->getDoctorID() != gDoctorID || !myStrEq(a->getStatus(), "completed"))
                {
                    prescSt.msg.set("Appointment not found or not completed by you.", true);
                }
                else
                {
                    // check if prescription already exists
                    bool already = false;
                    for (int i = 0; i < gPrescriptions.size(); i++)
                        if (gPrescriptions.getAll()[i].getAppointmentId() == apptId)
                        {
                            already = true; break;
                        }

                    if (already)
                    {
                        prescSt.msg.set("Prescription already written for this appointment.", true);
                    }
                    else
                    {
                        char today[12]; getTodayStr(today);
                        int newId = fh.getNextID("prescriptions.txt", 0);
                        prescription rx(newId, apptId, a->getPatientID(), gDoctorID,
                            today, prescSt.medBox.buf, prescSt.notesBox.buf);
                        gPrescriptions.add(rx);
                        fh.addPrescription(rx);
                        prescSt.msg.set("Prescription saved successfully.", false);
                        prescSt.apptIdBox.clear();
                        prescSt.medBox.clear();
                        prescSt.notesBox.clear();
                    }
                }
            }
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_DOCTOR_DASHBOARD;
            prescSt.msg.draw(window); prescSt.msg.update();
            break;
        }

        // ====================================================
        //  DOCTOR: PATIENT HISTORY
        // ====================================================
        case SCR_DOC_HISTORY:
        {
            drawHeader(window, "Patient Medical History");
            docHistSt.handleEvent(ev, window);
            drawText(window, "Enter Patient ID:", 60, 170, 14, TXT2);
            docHistSt.patIdBox.draw(window, "Patient ID");

            if (drawButton(window, ev, 320, 167, 120, 38, "Load History", TEAL, BTN_H))
            {
                int pid = 0;
                for (int i = 0; docHistSt.patIdBox.buf[i]; i++)
                    if (docHistSt.patIdBox.buf[i] >= '0' && docHistSt.patIdBox.buf[i] <= '9')
                        pid = pid * 10 + (docHistSt.patIdBox.buf[i] - '0');

                // patient must exist AND have at least one completed appt with this doctor
                bool ok = false;
                if (gPatients.findByID(pid))
                {
                    for (int i = 0; i < gAppointments.size(); i++)
                    {
                        appointment& a = gAppointments.getAll()[i];
                        if (a.getPatientID() == pid && a.getDoctorID() == gDoctorID &&
                            myStrEq(a.getStatus(), "completed"))
                        {
                            ok = true; break;
                        }
                    }
                }
                docHistSt.list.clear();
                if (!ok)
                {
                    docHistSt.msg.set("Access denied. Patient not your patient.", true);
                }
                else
                {
                    // gather prescriptions by this doctor for this patient, sort desc
                    int idx[100]; int cnt = 0;
                    for (int i = 0; i < gPrescriptions.size() && cnt < 100; i++)
                    {
                        prescription& rx = gPrescriptions.getAll()[i];
                        if (rx.getPatientId() == pid && rx.getDoctorId() == gDoctorID)
                            idx[cnt++] = i;
                    }
                    sortRxByDateDesc(gPrescriptions.getAll(), idx, cnt);
                    for (int i = 0; i < cnt; i++)
                    {
                        prescription& rx = gPrescriptions.getAll()[idx[i]];
                        char row[512]; row[0] = '\0';
                        myStrCat(row, rx.getDate(), 512);
                        myStrCat(row, "  Meds: ", 512); myStrCat(row, rx.getMedicine(), 512);
                        docHistSt.list.addRow(row);
                        char nr[512]; nr[0] = '\0';
                        myStrCat(nr, "  Notes: ", 512); myStrCat(nr, rx.getNotes(), 512);
                        docHistSt.list.addRow(nr, TXT2);
                    }
                    if (docHistSt.list.count == 0)
                        docHistSt.list.addRow("No prescriptions for this patient.", TXT2);
                }
            }

            docHistSt.list.draw(window, 40, 225, 1020, 360, "Prescriptions (most recent first):");
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_DOCTOR_DASHBOARD;
            docHistSt.msg.draw(window); docHistSt.msg.update();
            break;
        }

        // ====================================================
        //  ADMIN DASHBOARD
        // ====================================================
        case SCR_ADMIN_DASHBOARD:
        {
            drawHeader(window, "Admin Panel - MediCore");
            const char* labels[10] = {
                "Add Doctor","Remove Doctor","View All Patients",
                "View All Doctors","View All Appointments","View Unpaid Bills",
                "Discharge Patient","View Security Log","Daily Report","Logout"
            };
            float bw = 200, bh = 48, gap = 12;
            float sx = (WIN_W - (5 * (bw + gap) - gap)) / 2.f;
            for (int i = 0; i < 10; i++)
            {
                float x = sx + (i % 5) * (bw + gap);
                float y = 120 + (i / 5) * (bh + gap);
                Color nc = (i == 9) ? RED_N : BTN_DN;
                Color hc = (i == 9) ? RED_H : BTN_DH;
                if (drawButton(window, ev, x, y, bw, bh, labels[i], nc, hc, TXT))
                {
                    switch (i)
                    {
                    case 0: addDocSt.init(); gScreen = SCR_ADMIN_ADD_DOCTOR; break;
                    case 1: removeDocSt.init();
                        removeDocSt.list.clear();
                        for (int j = 0; j < gDoctors.size(); j++) {
                            doctor& d = gDoctors.getAll()[j];
                            char row[256]; row[0] = '\0';
                            char idB[8]; intToCharArr(d.getId(), idB);
                            char feeB[20]; floatToCharArr(d.getFee(), feeB);
                            myStrCat(row, "ID:", 256); myStrCat(row, idB, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, d.getName(), 256);
                            myStrCat(row, "  ", 256); myStrCat(row, d.getSpecialization(), 256);
                            myStrCat(row, "  PKR:", 256); myStrCat(row, feeB, 256);
                            removeDocSt.list.addRow(row);
                        }
                        gScreen = SCR_ADMIN_REMOVE_DOCTOR; break;
                    case 2: gList.clear();
                        for (int j = 0; j < gPatients.size(); j++) {
                            patient& p = gPatients.getAll()[j];
                            int unpaid = 0;
                            for (int k = 0; k < gBills.size(); k++)
                                if (gBills.getAll()[k].getPatientId() == p.getId() &&
                                    myStrEq(gBills.getAll()[k].getStatus(), "unpaid"))
                                    unpaid++;
                            char row[256]; row[0] = '\0';
                            char idB[8], ageB[6], balB[20], upB[6];
                            intToCharArr(p.getId(), idB); intToCharArr(p.getAge(), ageB);
                            floatToCharArr(p.getBalance(), balB); intToCharArr(unpaid, upB);
                            myStrCat(row, "ID:", 256); myStrCat(row, idB, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, p.getName(), 256);
                            myStrCat(row, "  Age:", 256); myStrCat(row, ageB, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, p.getGender(), 256);
                            myStrCat(row, "  Bal:PKR", 256); myStrCat(row, balB, 256);
                            myStrCat(row, "  UnpaidBills:", 256); myStrCat(row, upB, 256);
                            gList.addRow(row);
                        }
                        gScreen = SCR_ADMIN_VIEW_PATIENTS; break;
                    case 3: gList.clear();
                        for (int j = 0; j < gDoctors.size(); j++) {
                            doctor& d = gDoctors.getAll()[j];
                            char row[256]; row[0] = '\0';
                            char idB[8], feeB[20];
                            intToCharArr(d.getId(), idB); floatToCharArr(d.getFee(), feeB);
                            myStrCat(row, "ID:", 256); myStrCat(row, idB, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, d.getName(), 256);
                            myStrCat(row, "  ", 256); myStrCat(row, d.getSpecialization(), 256);
                            myStrCat(row, "  ", 256); myStrCat(row, d.getContact(), 256);
                            myStrCat(row, "  Fee:PKR", 256); myStrCat(row, feeB, 256);
                            gList.addRow(row);
                        }
                        gScreen = SCR_ADMIN_VIEW_DOCTORS; break;
                    case 4: gList.clear();
                        for (int j = 0; j < gAppointments.size(); j++) {
                            appointment& a = gAppointments.getAll()[j];
                            patient* p = gPatients.findByID(a.getPatientID());
                            doctor* d = gDoctors.findByID(a.getDoctorID());
                            const char* pn = p ? p->getName() : "?";
                            const char* dn = d ? d->getName() : "?";
                            char row[512]; buildApptRow(a, row, 512, dn, pn);
                            Color rc = myStrEq(a.getStatus(), "completed") ? SUCCESS :
                                myStrEq(a.getStatus(), "cancelled") ? TXT2 :
                                myStrEq(a.getStatus(), "noshow") ? DANGER : TXT;
                            gList.addRow(row, rc);
                        }
                        gScreen = SCR_ADMIN_VIEW_APPTS; break;
                    case 5: // unpaid bills with overdue flag
                    {
                        gList.clear();
                        char today[12]; getTodayStr(today);
                        auto dateToNum = [](const char* d)->int {
                            if (myStrLen(d) < 10)return 0;
                            int day = (d[0] - '0') * 10 + (d[1] - '0');
                            int mon = (d[3] - '0') * 10 + (d[4] - '0');
                            int yr = (d[6] - '0') * 1000 + (d[7] - '0') * 100 + (d[8] - '0') * 10 + (d[9] - '0');
                            return yr * 10000 + mon * 100 + day;
                            };
                        int todayNum = dateToNum(today);
                        for (int j = 0; j < gBills.size(); j++) {
                            bill& b = gBills.getAll()[j];
                            if (!myStrEq(b.getStatus(), "unpaid"))continue;
                            patient* p = gPatients.findByID(b.getPatientId());
                            const char* pn = p ? p->getName() : "Unknown";
                            char row[256]; row[0] = '\0';
                            char bId[8], amtB[20];
                            intToCharArr(b.getBillId(), bId);
                            floatToCharArr(b.getAmount(), amtB);
                            myStrCat(row, "Bill:", 256); myStrCat(row, bId, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, pn, 256);
                            myStrCat(row, "  PKR:", 256); myStrCat(row, amtB, 256);
                            myStrCat(row, "  ", 256); myStrCat(row, b.getDate(), 256);
                            // check overdue (more than 7 days before today)
                            int billNum = dateToNum(b.getDate());
                            // approximate: 7 days = ~7 in day field (ignores month boundaries but sufficient)
                            if (todayNum - billNum > 7)
                                myStrCat(row, "  [OVERDUE]", 256);
                            Color rc = (todayNum - billNum > 7) ? DANGER : AMBER;
                            gList.addRow(row, rc);
                        }
                        if (gList.count == 0)gList.addRow("No unpaid bills.", TXT2);
                        gScreen = SCR_ADMIN_UNPAID_BILLS; break;
                    }
                    case 6: dischargeSt.init(); gScreen = SCR_ADMIN_DISCHARGE; break;
                    case 7: // security log
                    {
                        gList.clear();
                        std::ifstream logf("security_log.txt");
                        if (!logf.is_open()) { gList.addRow("No security events logged.", TXT2); }
                        else {
                            char buf[256];
                            while (logf.getline(buf, 256))
                                if (myStrLen(buf) > 0)gList.addRow(buf, AMBER);
                            logf.close();
                            if (gList.count == 0)gList.addRow("No security events logged.", TXT2);
                        }
                        gScreen = SCR_ADMIN_SECURITY_LOG; break;
                    }
                    case 8: // daily report
                    {
                        gList.clear();
                        char today2[12]; getTodayStr(today2);
                        char hdrLine[64]; hdrLine[0] = '\0';
                        myStrCat(hdrLine, "Daily Report for: ", 64); myStrCat(hdrLine, today2, 64);
                        gList.addRow(hdrLine, TEAL);

                        int totPend = 0, totComp = 0, totNS = 0, totCanc = 0;
                        float revenue = 0;
                        for (int j = 0; j < gAppointments.size(); j++) {
                            appointment& a = gAppointments.getAll()[j];
                            if (!myStrEq(a.getDate(), today2))continue;
                            if (myStrEq(a.getStatus(), "pending"))   totPend++;
                            if (myStrEq(a.getStatus(), "completed")) totComp++;
                            if (myStrEq(a.getStatus(), "noshow"))    totNS++;
                            if (myStrEq(a.getStatus(), "cancelled")) totCanc++;
                        }
                        // paid bills today
                        for (int j = 0; j < gBills.size(); j++) {
                            bill& b = gBills.getAll()[j];
                            if (myStrEq(b.getDate(), today2) && myStrEq(b.getStatus(), "paid"))
                                revenue += b.getAmount();
                        }

                        char sumRow[256]; sumRow[0] = '\0';
                        char pB[6], cB[6], nB[6], cnB[6];
                        intToCharArr(totPend, pB); intToCharArr(totComp, cB);
                        intToCharArr(totNS, nB); intToCharArr(totCanc, cnB);
                        myStrCat(sumRow, "Appts - Pending:", 256); myStrCat(sumRow, pB, 256);
                        myStrCat(sumRow, " Completed:", 256); myStrCat(sumRow, cB, 256);
                        myStrCat(sumRow, " No-Show:", 256); myStrCat(sumRow, nB, 256);
                        myStrCat(sumRow, " Cancelled:", 256); myStrCat(sumRow, cnB, 256);
                        gList.addRow(sumRow, TXT);

                        char revRow[64]; revRow[0] = '\0';
                        myStrCat(revRow, "Revenue (paid bills today): PKR ", 64);
                        char revB[20]; floatToCharArr(revenue, revB);
                        myStrCat(revRow, revB, 64);
                        gList.addRow(revRow, SUCCESS);

                        gList.addRow("--- Patients with unpaid bills ---", AMBER);
                        for (int j = 0; j < gPatients.size(); j++) {
                            patient& p = gPatients.getAll()[j];
                            float owed = 0;
                            for (int k = 0; k < gBills.size(); k++) {
                                bill& b = gBills.getAll()[k];
                                if (b.getPatientId() == p.getId() && myStrEq(b.getStatus(), "unpaid"))
                                    owed += b.getAmount();
                            }
                            if (owed > 0) {
                                char pr[128]; pr[0] = '\0';
                                myStrCat(pr, p.getName(), 128); myStrCat(pr, "  Owes: PKR ", 128);
                                char owB[20]; floatToCharArr(owed, owB); myStrCat(pr, owB, 128);
                                gList.addRow(pr, AMBER);
                            }
                        }

                        gList.addRow("--- Doctor Summary for Today ---", TEAL);
                        for (int j = 0; j < gDoctors.size(); j++) {
                            doctor& d = gDoctors.getAll()[j];
                            int dc = 0, dp = 0, dn2 = 0;
                            for (int k = 0; k < gAppointments.size(); k++) {
                                appointment& a = gAppointments.getAll()[k];
                                if (a.getDoctorID() != d.getId() || !myStrEq(a.getDate(), today2))continue;
                                if (myStrEq(a.getStatus(), "completed"))dc++;
                                if (myStrEq(a.getStatus(), "pending"))dp++;
                                if (myStrEq(a.getStatus(), "noshow"))dn2++;
                            }
                            if (dc + dp + dn2 > 0) {
                                char dr[128]; dr[0] = '\0';
                                char dcB[6], dpB[6], dnB[6];
                                intToCharArr(dc, dcB); intToCharArr(dp, dpB); intToCharArr(dn2, dnB);
                                myStrCat(dr, d.getName(), 128);
                                myStrCat(dr, "  Comp:", 128); myStrCat(dr, dcB, 128);
                                myStrCat(dr, "  Pend:", 128); myStrCat(dr, dpB, 128);
                                myStrCat(dr, "  NoShow:", 128); myStrCat(dr, dnB, 128);
                                gList.addRow(dr, TXT);
                            }
                        }
                        gScreen = SCR_ADMIN_DAILY_REPORT; break;
                    }
                    case 9: gScreen = SCR_LOGIN; loginSt.init(); break;
                    }
                }
            }
            break;
        }

        // ====================================================
        //  ADMIN: ADD DOCTOR
        // ====================================================
        case SCR_ADMIN_ADD_DOCTOR:
        {
            drawHeader(window, "Add New Doctor");
            addDocSt.handleEvent(ev, window);
            const char* flds[5] = { "Name:","Specialization:","Contact (11 digits):","Password (min 6):","Fee (PKR):" };
            TBox* boxes[5] = { &addDocSt.nameBox,&addDocSt.specBox,&addDocSt.contBox,&addDocSt.passBox,&addDocSt.feeBox };
            for (int i = 0; i < 5; i++)
            {
                drawText(window, flds[i], 60, boxes[i]->y - 22, 13, TXT2);
                boxes[i]->draw(window);
            }
            if (drawButton(window, ev, 60, 450, 180, 44, "Add Doctor", SUCCESS, Color(70, 230, 170)))
            {
                // validate
                if (!Validator::validateContact(addDocSt.contBox.buf))
                {
                    addDocSt.msg.set("Contact must be exactly 11 digits.", true);
                }
                else if (!Validator::validatePassword(addDocSt.passBox.buf))
                {
                    addDocSt.msg.set("Password must be at least 6 characters.", true);
                }
                else
                {
                    float fee = 0;
                    const char* fb = addDocSt.feeBox.buf;
                    bool hasDot = false; float frac = 0, div = 1;
                    for (int i = 0; fb[i]; i++) {
                        if (fb[i] == '.')hasDot = true;
                        else if (fb[i] >= '0' && fb[i] <= '9') {
                            if (!hasDot)fee = fee * 10 + (fb[i] - '0');
                            else { div *= 10; frac += (fb[i] - '0') / div; }
                        }
                    }
                    fee += frac;
                    if (!Validator::validatePositiveFloat(fee))
                    {
                        addDocSt.msg.set("Fee must be a positive number.", true);
                    }
                    else
                    {
                        int newId = fh.getNextID("doctor.txt", 0);
                        doctor nd(newId, addDocSt.nameBox.buf, addDocSt.passBox.buf,
                            addDocSt.contBox.buf, addDocSt.specBox.buf, fee);
                        gDoctors.add(nd);
                        fh.addDoctor(nd);
                        char ok[80]; ok[0] = '\0';
                        myStrCat(ok, "Doctor added! ID:", 80);
                        char idB[8]; intToCharArr(newId, idB); myStrCat(ok, idB, 80);
                        addDocSt.msg.set(ok, false);
                        addDocSt.nameBox.clear(); addDocSt.specBox.clear();
                        addDocSt.contBox.clear(); addDocSt.passBox.clear(); addDocSt.feeBox.clear();
                    }
                }
            }
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_ADMIN_DASHBOARD;
            addDocSt.msg.draw(window); addDocSt.msg.update();
            break;
        }

        // ====================================================
        //  ADMIN: REMOVE DOCTOR
        // ====================================================
        case SCR_ADMIN_REMOVE_DOCTOR:
        {
            drawHeader(window, "Remove Doctor");
            removeDocSt.handleEvent(ev, window);
            removeDocSt.list.draw(window, 40, 80, 1020, 380, "All Doctors:");
            drawText(window, "Enter Doctor ID to remove:", 40, 478, 14, TXT2);
            removeDocSt.docIdBox.draw(window, "Doctor ID");
            if (drawButton(window, ev, 420, 475, 160, 36, "Remove", DANGER, RED_H, TXT))
            {
                int did = 0;
                for (int i = 0; removeDocSt.docIdBox.buf[i]; i++)
                    if (removeDocSt.docIdBox.buf[i] >= '0' && removeDocSt.docIdBox.buf[i] <= '9')
                        did = did * 10 + (removeDocSt.docIdBox.buf[i] - '0');

                // check no pending appointments
                bool hasPending = false;
                for (int i = 0; i < gAppointments.size(); i++) {
                    appointment& a = gAppointments.getAll()[i];
                    if (a.getDoctorID() == did && myStrEq(a.getStatus(), "pending"))
                    {
                        hasPending = true; break;
                    }
                }
                if (hasPending)
                {
                    removeDocSt.msg.set("Cannot remove: doctor has pending appointments.", true);
                }
                else if (!gDoctors.findByID(did))
                {
                    removeDocSt.msg.set("Doctor ID not found.", true);
                }
                else
                {
                    fh.deleteDoctor(did);
                    gDoctors.removeByID(did);
                    // refresh list
                    removeDocSt.list.clear();
                    for (int j = 0; j < gDoctors.size(); j++) {
                        doctor& d = gDoctors.getAll()[j];
                        char row[256]; row[0] = '\0';
                        char idB[8], feeB[20];
                        intToCharArr(d.getId(), idB); floatToCharArr(d.getFee(), feeB);
                        myStrCat(row, "ID:", 256); myStrCat(row, idB, 256);
                        myStrCat(row, "  ", 256); myStrCat(row, d.getName(), 256);
                        myStrCat(row, "  ", 256); myStrCat(row, d.getSpecialization(), 256);
                        myStrCat(row, "  PKR:", 256); myStrCat(row, feeB, 256);
                        removeDocSt.list.addRow(row);
                    }
                    removeDocSt.msg.set("Doctor removed.", false);
                    removeDocSt.docIdBox.clear();
                }
            }
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_ADMIN_DASHBOARD;
            removeDocSt.msg.draw(window); removeDocSt.msg.update();
            break;
        }

        // ====================================================
        //  ADMIN: VIEW ALL PATIENTS / DOCTORS / APPOINTMENTS
        //         UNPAID BILLS / SECURITY LOG / DAILY REPORT
        //  (all just show gList)
        // ====================================================
        case SCR_ADMIN_VIEW_PATIENTS:
        case SCR_ADMIN_VIEW_DOCTORS:
        case SCR_ADMIN_VIEW_APPTS:
        case SCR_ADMIN_UNPAID_BILLS:
        case SCR_ADMIN_SECURITY_LOG:
        case SCR_ADMIN_DAILY_REPORT:
        {
            const char* titles[] = {
                "All Patients","All Doctors","All Appointments",
                "Unpaid Bills","Security Log","Daily Report"
            };
            int idx2 = (int)gScreen - (int)SCR_ADMIN_VIEW_PATIENTS;
            drawHeader(window, titles[idx2]);
            gList.handleScroll(ev);
            gList.draw(window, 40, 80, 1020, 560, "");
            if (drawButton(window, ev, 20, 635, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_ADMIN_DASHBOARD;
            break;
        }

        // ====================================================
        //  ADMIN: DISCHARGE PATIENT
        // ====================================================
        case SCR_ADMIN_DISCHARGE:
        {
            drawHeader(window, "Discharge Patient");
            dischargeSt.handleEvent(ev, window);
            drawTextCentre(window, "Enter Patient ID to discharge:", WIN_W / 2.f, 250, 16, TXT2);
            dischargeSt.patIdBox.draw(window, "Patient ID");
            if (drawButton(window, ev, WIN_W / 2.f - 80, 360, 160, 44, "Discharge", DANGER, RED_H, TXT))
            {
                int pid = 0;
                for (int i = 0; dischargeSt.patIdBox.buf[i]; i++)
                    if (dischargeSt.patIdBox.buf[i] >= '0' && dischargeSt.patIdBox.buf[i] <= '9')
                        pid = pid * 10 + (dischargeSt.patIdBox.buf[i] - '0');

                if (!gPatients.findByID(pid))
                {
                    dischargeSt.msg.set("Patient not found.", true);
                }
                else
                {
                    // check unpaid bills
                    bool hasUnpaid = false;
                    for (int i = 0; i < gBills.size(); i++)
                        if (gBills.getAll()[i].getPatientId() == pid &&
                            myStrEq(gBills.getAll()[i].getStatus(), "unpaid"))
                        {
                            hasUnpaid = true; break;
                        }

                    bool hasPending = false;
                    for (int i = 0; i < gAppointments.size(); i++)
                        if (gAppointments.getAll()[i].getPatientID() == pid &&
                            myStrEq(gAppointments.getAll()[i].getStatus(), "pending"))
                        {
                            hasPending = true; break;
                        }

                    if (hasUnpaid)
                        dischargeSt.msg.set("Cannot discharge: patient has unpaid bills.", true);
                    else if (hasPending)
                        dischargeSt.msg.set("Cannot discharge: patient has pending appointments.", true);
                    else
                    {
                        fh.archiveAndDischargePatient(pid, gPatients, gAppointments, gBills, gPrescriptions);
                        gPatients.removeByID(pid);
                        dischargeSt.msg.set("Patient discharged and archived successfully.", false);
                        dischargeSt.patIdBox.clear();
                    }
                }
            }
            if (drawButton(window, ev, 20, 620, 120, 36, "Back", BTN_DN, BTN_DH, TXT2))
                gScreen = SCR_ADMIN_DASHBOARD;
            dischargeSt.msg.draw(window); dischargeSt.msg.update();
            break;
        }

        default: gScreen = SCR_LOGIN; break;
        } // end switch

        window.display();
    } // end main loop

    // clean up the admin object
    delete gAdmin;
    gAdmin = nullptr;

    return 0;
}