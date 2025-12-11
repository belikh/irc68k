#ifndef MOCK_MAC_H
#define MOCK_MAC_H

#ifdef LOCAL_TESTING

#include <cstdint>
#include <cstring>
#include <cstdio>

// Basic Types
typedef int16_t OSErr;
typedef uint32_t OSType;
typedef int32_t longint;
typedef int16_t integer;
typedef uint8_t Boolean;
typedef unsigned char Str255[256];
typedef uint32_t ResType;
typedef char* Ptr;
typedef Ptr* Handle;

#define noErr 0
#define true 1
#define false 0
#define nil NULL

// Geometry
struct Point {
    int16_t v;
    int16_t h;
};

struct Rect {
    int16_t top;
    int16_t left;
    int16_t bottom;
    int16_t right;
};

// Events
struct EventRecord {
    uint16_t what;
    uint32_t message;
    uint32_t when;
    Point    where;
    uint16_t modifiers;
};

#define nullEvent 0
#define mouseDown 1
#define mouseUp 2
#define keyDown 3
#define keyUp 4
#define autoKey 5
#define updateEvt 6
#define diskEvt 7
#define activateEvt 8
#define osEvt 15

#define everyEvent 0xFFFF

#define cmdKey 256
#define shiftKey 512
#define alphaLock 1024
#define optionKey 2048
#define controlKey 4096
#define activeFlag 1
#define charCodeMask 0x000000FF

// BitMap (for screenBits)
struct BitMap {
    void* baseAddr;
    int16_t rowBytes;
    Rect bounds;
};

// QuickDraw
struct GrafPort {
    Rect portRect;
    // ...
};
typedef GrafPort* GrafPtr;
typedef GrafPtr WindowPtr;
typedef WindowPtr WindowRef;

struct WindowRecord {
    GrafPort port;
    WindowPtr nextWindow;
    // ...
};
typedef WindowRecord* WindowPeek;

struct QDGlobals {
    GrafPtr thePort;
    BitMap screenBits;
};
extern QDGlobals qd;

// Controls / Scrollbars
typedef struct ControlRecord* ControlHandle;

// TextEdit
struct TERec {
    Rect viewRect;
    Rect destRect;
    Rect otherRect;
    Handle hText;
    int16_t teLength;
    // ... extensive fields ...
};
typedef TERec* TEPtr;
typedef TEPtr* TEHandle;

// Menus
struct MenuInfo {
    // ...
};
typedef struct MenuInfo* MenuHandle;

// Functions
void InitGraf(GrafPtr*);
void InitFonts();
void InitWindows();
void InitMenus();
void TEInit();
void InitDialogs(void*);
void FlushEvents(uint16_t, uint16_t);
void InitCursor();

Boolean WaitNextEvent(uint16_t, EventRecord*, uint32_t, void*);
void GetNextEvent(uint16_t, EventRecord*);

WindowPtr GetNewWindow(int16_t, void*, WindowPtr);
void DisposeWindow(WindowPtr);
void SelectWindow(WindowPtr);
void ShowWindow(WindowPtr);
void HideWindow(WindowPtr);
void SetPort(WindowPtr);
void GetPort(GrafPtr*);
void BeginUpdate(WindowPtr);
void EndUpdate(WindowPtr);
void SetWTitle(WindowPtr, const unsigned char*);
void DragWindow(WindowPtr, Point, Rect*);
long GrowWindow(WindowPtr, Point, Rect*);
void SizeWindow(WindowPtr, int16_t, int16_t, Boolean);
void InvalRect(const Rect*);
void EraseRect(const Rect*);
int16_t FindWindow(Point, WindowPtr*);
long GetWRefCon(WindowPtr);
void SetWRefCon(WindowPtr, long);
WindowPtr FrontWindow();
Boolean TrackGoAway(WindowPtr, Point);

#define inDesk 0
#define inMenuBar 1
#define inSysWindow 2
#define inContent 3
#define inDrag 4
#define inGrow 5
#define inGoAway 6
#define inZoomIn 7
#define inZoomOut 8

Handle GetNewMBar(int16_t);
void SetMenuBar(Handle);
void DisposeHandle(Handle);
void AppendResMenu(MenuHandle, uint32_t);
MenuHandle GetMenuHandle(int16_t);

MenuHandle GetMenu(int16_t);
void InsertMenu(MenuHandle, int16_t);
void DrawMenuBar();
long MenuSelect(Point);
long MenuKey(char);
void HiliteMenu(int16_t);
void GetItem(MenuHandle, int16_t, Str255);
void SetItem(MenuHandle, int16_t, const unsigned char*);
void GetMenuItemText(MenuHandle, int16_t, unsigned char*);
void OpenDeskAcc(const unsigned char*);
Boolean SystemEdit(int16_t);

// Macros for LoWord/HiWord
inline int16_t LoWord(long x) { return (int16_t)(x & 0xFFFF); }
inline int16_t HiWord(long x) { return (int16_t)((x >> 16) & 0xFFFF); }

void SystemClick(const EventRecord*, WindowPtr);
void SystemTask();

// TextEdit Functions
TEHandle TENew(const Rect*, const Rect*);
void TEDispose(TEHandle);
void TEKey(char, TEHandle);
void TEClick(Point, Boolean, TEHandle);
void TEUpdate(const Rect*, TEHandle);
void TEActivate(TEHandle);
void TEDeactivate(TEHandle);
void TEIdle(TEHandle);
void TEInsert(const void*, int32_t, TEHandle);
void TESetSelect(long, long, TEHandle);
void TEAutoView(Boolean, TEHandle);
void TEScroll(int16_t, int16_t, TEHandle);
void TECut(TEHandle);
void TECopy(TEHandle);
void TEPaste(TEHandle);
void TEDelete(TEHandle);
void TECalText(TEHandle);

// QuickDraw Functions
void MoveTo(int16_t, int16_t);
void LineTo(int16_t, int16_t);
void DrawString(const unsigned char*);
int16_t StringWidth(const unsigned char*);
void TextFont(int16_t);
void TextSize(int16_t);
void GlobalToLocal(Point*);
Boolean PtInRect(Point, Rect*);

// Misc
void SysBeep(int16_t);
void ExitToShell();
void GetDateTime(unsigned long*);

// String helpers (Pascal)
void CopyPascalString(const unsigned char* src, unsigned char* dst);
void C2PStr(const char* src, unsigned char* dst);
void P2CStr(const unsigned char* src, char* dst);

#endif // LOCAL_TESTING
#endif // MOCK_MAC_H
