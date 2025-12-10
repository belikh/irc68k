#include "../include/mock_mac.h"
#include <cstdlib> // for NULL

// Define the global QDGlobals
QDGlobals qd;

// Dummy implementations
// Note: We use extern "C" usually only if C++ name mangling is an issue,
// but since both sides are C++, we just need to ensure signatures match.

void InitGraf(GrafPtr*) {}
void InitFonts() {}
void InitWindows() {}
void InitMenus() {}
void TEInit() {}
void InitDialogs(void*) {}
void FlushEvents(uint16_t, uint16_t) {}
void InitCursor() {}

Boolean WaitNextEvent(uint16_t, EventRecord*, uint32_t, void*) { return false; }
void GetNextEvent(uint16_t, EventRecord*) {}

WindowPtr GetNewWindow(int16_t, void*, WindowPtr) {
    WindowPtr wp = (WindowPtr)new WindowRecord();
    return wp;
}
void DisposeWindow(WindowPtr w) { delete w; }
void SelectWindow(WindowPtr) {}
void ShowWindow(WindowPtr) {}
void HideWindow(WindowPtr) {}
void SetPort(WindowPtr) {}
void GetPort(GrafPtr*) {}
void BeginUpdate(WindowPtr) {}
void EndUpdate(WindowPtr) {}
void SetWTitle(WindowPtr, const unsigned char*) {}
void DragWindow(WindowPtr, Point, Rect*) {}
long GrowWindow(WindowPtr, Point, Rect*) { return 0; }
void SizeWindow(WindowPtr, int16_t, int16_t, Boolean) {}
void InvalRect(const Rect*) {}
void EraseRect(const Rect*) {}
int16_t FindWindow(Point, WindowPtr*) { return 0; }
long GetWRefCon(WindowPtr) { return 0; }
void SetWRefCon(WindowPtr, long) {}
WindowPtr FrontWindow() { return NULL; }
Boolean TrackGoAway(WindowPtr, Point) { return false; }

Handle GetNewMBar(int16_t) { return NULL; }
void SetMenuBar(Handle) {}
void DisposeHandle(Handle) {}
void AppendResMenu(MenuHandle, uint32_t) {}
MenuHandle GetMenuHandle(int16_t) { return NULL; }

MenuHandle GetMenu(int16_t) { return NULL; }
void InsertMenu(MenuHandle, int16_t) {}
void DrawMenuBar() {}
long MenuSelect(Point) { return 0; }
long MenuKey(char) { return 0; }
void HiliteMenu(int16_t) {}
void GetItem(MenuHandle, int16_t, Str255) {}
void SetItem(MenuHandle, int16_t, const unsigned char*) {}
void GetMenuItemText(MenuHandle, int16_t, unsigned char*) {}
void OpenDeskAcc(const unsigned char*) {}
Boolean SystemEdit(int16_t) { return false; }

void SystemClick(const EventRecord*, WindowPtr) {}
void SystemTask() {}

// TextEdit Functions
TEHandle TENew(const Rect*, const Rect*) {
    TEPtr p = new TERec;
    p->teLength = 0;
    p->hText = new Ptr; // Mock handle
    *(p->hText) = new char[1024]; // buffer
    TEHandle h = new TEPtr;
    *h = p;
    return h;
}
void TEDispose(TEHandle) {}
void TEKey(char, TEHandle) {}
void TEClick(Point, Boolean, TEHandle) {}
void TEUpdate(const Rect*, TEHandle) {}
void TEActivate(TEHandle) {}
void TEDeactivate(TEHandle) {}
void TEIdle(TEHandle) {}
void TEInsert(const void*, int32_t, TEHandle) {}
void TESetSelect(long, long, TEHandle) {}
void TEAutoView(Boolean, TEHandle) {}
void TEScroll(int16_t, int16_t, TEHandle) {}
void TECut(TEHandle) {}
void TECopy(TEHandle) {}
void TEPaste(TEHandle) {}
void TEDelete(TEHandle) {}
void TECalText(TEHandle) {}

// QuickDraw Functions
void MoveTo(int16_t, int16_t) {}
void LineTo(int16_t, int16_t) {}
void DrawString(const unsigned char*) {}
int16_t StringWidth(const unsigned char*) { return 0; }
void TextFont(int16_t) {}
void TextSize(int16_t) {}
void GlobalToLocal(Point*) {}
Boolean PtInRect(Point, Rect*) { return false; }

// Misc
void SysBeep(int16_t) {}
void ExitToShell() {}
void GetDateTime(unsigned long*) {}

// String helpers (Pascal)
void CopyPascalString(const unsigned char*, unsigned char*) {}
void C2PStr(const char*, unsigned char*) {}
void P2CStr(const unsigned char*, char*) {}
