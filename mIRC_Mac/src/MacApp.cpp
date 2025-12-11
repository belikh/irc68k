#include "MacApp.h"
#include <cstdio>
#include <cstring>

// Constants
const int kStatusWindowID = 128;
const int kChannelWindowID = 129;

const int kAppleMenuID = 128;
const int kFileMenuID = 129;
const int kEditMenuID = 130;
const int kCommandsMenuID = 131;

// Command IDs
const int kCmdConnect = 1;
const int kCmdDisconnect = 2;
const int kCmdQuit = 4;
const int kCmdJoin = 1;
const int kCmdPart = 2;
const int kCmdList = 4;

MacApp::MacApp() : running(false) {
}

MacApp::~MacApp() {
}

void MacApp::Init() {
    InitializeToolbox();
    SetupMenus();

    // Bind IRC callbacks
    irc.onLog = [this](const std::string& msg) { this->OnIRCLog(msg); };
    irc.onMessage = [this](const std::string& t, const std::string& s, const std::string& m) { this->OnIRCMessage(t, s, m); };
    irc.onJoin = [this](const std::string& c) { this->OnIRCJoin(c); };
    irc.onPart = [this](const std::string& c) { this->OnIRCPart(c); };

    CreateStatusWindow();
}

void MacApp::InitializeToolbox() {
#ifndef LOCAL_TESTING
    InitGraf(&qd.thePort);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nil);
#else
    InitGraf(nullptr);
    InitFonts();
    InitWindows();
    InitMenus();
    TEInit();
    InitDialogs(nullptr);
#endif
    InitCursor();
}

void MacApp::SetupMenus() {
    Handle menuBar = GetNewMBar(128);
    SetMenuBar(menuBar);
    DisposeHandle(menuBar);

    AppendResMenu(GetMenuHandle(kAppleMenuID), 'DRVR');
    DrawMenuBar();
}

void MacApp::Run() {
    running = true;
    EventRecord event;

    while (running) {
        // Run IRC Update
        irc.Update();

        // Handle Mac Events
        if (WaitNextEvent(everyEvent, &event, 0, nil)) {
            HandleEvent(event);
        }
    }
}

void MacApp::HandleEvent(EventRecord& event) {
    switch (event.what) {
        case mouseDown:
            DoMouseDown(event);
            break;
        case keyDown:
        case autoKey:
            DoKeyDown(event);
            break;
        case updateEvt:
            DoUpdate(event);
            break;
        case activateEvt:
            DoActivate(event);
            break;
    }
}

void MacApp::DoMouseDown(EventRecord& event) {
    WindowPtr window;
    int16_t part = FindWindow(event.where, &window);

    switch (part) {
        case inMenuBar:
            DoMenuCommand(MenuSelect(event.where));
            break;
        case inSysWindow:
            SystemClick(&event, window);
            break;
        case inContent:
            SelectWindow(window);
            // Handle clicks in TextEdit
            {
                ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
                if (data) {
                    Point localPt = event.where;
                    SetPort(window);
                    GlobalToLocal(&localPt);

                    // Check if click is in Input TE
                    if (PtInRect(localPt, &(*data->inputTE)->viewRect)) {
                        TEClick(localPt, (event.modifiers & shiftKey), data->inputTE);
                    } else {
                        // Log TE (readonly usually, but allow select)
                        TEClick(localPt, (event.modifiers & shiftKey), data->logTE);
                    }
                }
            }
            break;
        case inDrag:
            DragWindow(window, event.where, &qd.screenBits.bounds);
            break;
        case inGoAway:
            if (TrackGoAway(window, event.where)) {
                DisposeChatWindow(window);
            }
            break;
        case inGrow:
            {
                long newSize = GrowWindow(window, event.where, &qd.screenBits.bounds);
                if (newSize != 0) {
                    SizeWindow(window, LoWord(newSize), HiWord(newSize), true);
                    ResizeWindow(window, { (int16_t)LoWord(newSize), (int16_t)HiWord(newSize) });
                }
            }
            break;
    }
}

void MacApp::DoMenuCommand(long menuResult) {
    int16_t menuID = HiWord(menuResult);
    int16_t itemID = LoWord(menuResult);

    if (menuID == kAppleMenuID) {
        if (itemID == 1) {
            // About
            // Alert(128, nil);
        } else {
            Str255 daName;
            GetMenuItemText(GetMenuHandle(kAppleMenuID), itemID, daName);
            OpenDeskAcc(daName);
        }
    }
    else if (menuID == kFileMenuID) {
        switch (itemID) {
            case kCmdConnect:
                // Hardcoded connect for MVP
                irc.Connect("irc.libera.chat", 6667, "mIRC_SE30", "mirc", "Mac SE/30 User");
                break;
            case kCmdDisconnect:
                irc.Disconnect("User disconnected");
                break;
            case kCmdQuit:
                running = false;
                break;
        }
    }
    else if (menuID == kEditMenuID) {
        if (!SystemEdit(itemID - 1)) {
            // Handle local edit
             WindowPtr window = FrontWindow();
             if (window) {
                 ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
                 if (data) {
                     // Simple forward to active TE. Ideally check which TE has focus.
                     // For MVP, just assume input.
                     switch(itemID) {
                         case 3: TECut(data->inputTE); break;
                         case 4: TECopy(data->inputTE); break;
                         case 5: TEPaste(data->inputTE); break;
                         case 6: TEDelete(data->inputTE); break;
                     }
                 }
             }
        }
    }
    else if (menuID == kCommandsMenuID) {
         WindowPtr window = FrontWindow();
         ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);

         switch(itemID) {
             case kCmdJoin:
                 // In a real app, show a Dialog. Here, hardcode or use the input line?
                 // Let's assume user typed /join #channel in the input line,
                 // but this menu item would trigger a prompt.
                 // For MVP: Join a test channel.
                 irc.Join("#macintosh");
                 break;
             case kCmdPart:
                 if (data && data->type == kWindowTypeChannel) {
                     irc.Part(data->target);
                     // Close window?
                 }
                 break;
         }
    }

    HiliteMenu(0);
}

void MacApp::DoKeyDown(EventRecord& event) {
    char key = event.message & charCodeMask;
    if (event.modifiers & cmdKey) {
        DoMenuCommand(MenuKey(key));
    } else {
        WindowPtr window = FrontWindow();
        if (window) {
            ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
            if (data) {
                if (key == '\r' || key == '\n' || key == 3) { // Enter
                    HandleInput(window);
                } else {
                    TEKey(key, data->inputTE);
                }
            }
        }
    }
}

void MacApp::DoUpdate(EventRecord& event) {
    WindowPtr window = (WindowPtr)event.message;
    BeginUpdate(window);
    SetPort(window); // Ensure we draw into the update region of the correct window

    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (data) {
        EraseRect(&window->portRect);
        TEUpdate(&data->logTE[0]->viewRect, data->logTE);

        // Draw divider line
        MoveTo(0, window->portRect.bottom - 20);
        LineTo(window->portRect.right, window->portRect.bottom - 20);

        TEUpdate(&data->inputTE[0]->viewRect, data->inputTE);
    }

    EndUpdate(window);
}

void MacApp::DoActivate(EventRecord& event) {
    WindowPtr window = (WindowPtr)event.message;
    bool active = (event.modifiers & activeFlag) != 0;

    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (data) {
        if (active) {
            TEActivate(data->inputTE);
            TEActivate(data->logTE);
        } else {
            TEDeactivate(data->inputTE);
            TEDeactivate(data->logTE);
        }
    }
}

// Window Management
WindowPtr MacApp::CreateStatusWindow() {
    WindowPtr window = GetNewWindow(kStatusWindowID, nil, (WindowPtr)-1);

    ChatWindowData* data = new ChatWindowData();
    data->type = kWindowTypeStatus;
    data->target = "";

    SetPort(window);
    TextFont(0); // System Font
    TextSize(12);

    Rect logRect = window->portRect;
    logRect.bottom -= 20; // Space for input
    logRect.right -= 15;  // Space for scrollbar (visual only for now)

    Rect inputRect = window->portRect;
    inputRect.top = inputRect.bottom - 18;
    inputRect.left += 2;
    inputRect.right -= 2;
    inputRect.bottom -= 2;

    data->logTE = TENew(&logRect, &logRect);
    data->inputTE = TENew(&inputRect, &inputRect);

    SetWRefCon(window, (long)data);

    ShowWindow(window);
    return window;
}

WindowPtr MacApp::CreateChannelWindow(const std::string& name) {
    WindowPtr window = GetNewWindow(kChannelWindowID, nil, (WindowPtr)-1);

    Str255 pName;
    const char* cStr = name.c_str();
    int len = name.length();
    if (len > 255) len = 255;
    pName[0] = len;
    memcpy(pName+1, cStr, len);
    SetWTitle(window, pName);

    ChatWindowData* data = new ChatWindowData();
    data->type = kWindowTypeChannel;
    data->target = name;

    SetPort(window);
    TextFont(0);
    TextSize(12);

    Rect logRect = window->portRect;
    logRect.bottom -= 20;
    logRect.right -= 15;

    Rect inputRect = window->portRect;
    inputRect.top = inputRect.bottom - 18;
    inputRect.left += 2;
    inputRect.right -= 2;
    inputRect.bottom -= 2;

    data->logTE = TENew(&logRect, &logRect);
    data->inputTE = TENew(&inputRect, &inputRect);

    SetWRefCon(window, (long)data);
    ShowWindow(window);
    return window;
}

void MacApp::ResizeWindow(WindowPtr window, Point newSize) {
    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (!data) return;

    Rect logRect = window->portRect;
    logRect.bottom -= 20;
    logRect.right -= 15;

    Rect inputRect = window->portRect;
    inputRect.top = inputRect.bottom - 18;
    inputRect.left += 2;
    inputRect.right -= 2;
    inputRect.bottom -= 2;

    // Resize TEs
    // Note: Standard TextEdit doesn't have a simple "Resize" call that reflows perfect,
    // we often have to manipulate the rects directly.
    (*data->logTE)->viewRect = logRect;
    (*data->logTE)->destRect = logRect; // Reflow
    TECalText(data->logTE);

    (*data->inputTE)->viewRect = inputRect;
    (*data->inputTE)->destRect = inputRect;
    TECalText(data->inputTE);

    InvalRect(&window->portRect);
}

void MacApp::DisposeChatWindow(WindowPtr window) {
    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (data) {
        TEDispose(data->logTE);
        TEDispose(data->inputTE);
        delete data;
    }
    DisposeWindow(window);
}

void MacApp::AppendText(WindowPtr window, const std::string& text) {
    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (!data) return;

    std::string line = text + "\r";
    TEInsert(line.c_str(), line.length(), data->logTE);

    // Auto scroll to bottom
    // Calculate difference between text height and view height
    // TEScroll is tricky. Simplified:
    // In a real app we'd verify line heights.
    // For now, let's just trigger an update.
    InvalRect(&window->portRect);
}

void MacApp::HandleInput(WindowPtr window) {
    ChatWindowData* data = (ChatWindowData*)GetWRefCon(window);
    if (!data) return;

    TEHandle te = data->inputTE;
    int len = (*te)->teLength;
    if (len == 0) return;

    Handle hText = (*te)->hText;
    char* textPtr = *hText;
    std::string input(textPtr, len);

    // Clear Input
    TESetSelect(0, len, te);
    TEDelete(te);

    // Process Input
    if (input[0] == '/') {
        // Parse Command
        if (input.substr(0, 5) == "/join") {
             std::string chan = input.substr(6);
             irc.Join(chan);
        } else if (input.substr(0, 5) == "/part") {
             irc.Part(data->target);
        } else if (input.substr(0, 4) == "/msg") {
            // /msg user text...
        }
    } else {
        if (data->type == kWindowTypeChannel) {
            irc.PrivMsg(data->target, input);
            AppendText(window, "<Me> " + input);
        } else {
            // Status window input? Raw command?
            irc.SendRaw(input);
             AppendText(window, "> " + input);
        }
    }
}

WindowPtr MacApp::FindWindowByTarget(const std::string& target) {
    WindowPtr win = FrontWindow();
    while (win != nil) {
        ChatWindowData* data = (ChatWindowData*)GetWRefCon(win);
        if (data && data->target == target) return win;
        win = (WindowPtr)((WindowPeek)win)->nextWindow;
    }
    return nil;
}

// IRC Callbacks
void MacApp::OnIRCLog(const std::string& text) {
    // Find Status Window
    WindowPtr win = FrontWindow();
    // In a real app we keep a pointer to status window.
    // Let's iterate.
    while (win != nil) {
        ChatWindowData* data = (ChatWindowData*)GetWRefCon(win);
        if (data && data->type == kWindowTypeStatus) {
            AppendText(win, text);
            break;
        }
        win = (WindowPtr)((WindowPeek)win)->nextWindow;
    }
}

void MacApp::OnIRCMessage(const std::string& target, const std::string& sender, const std::string& text) {
    std::string winTarget = (target[0] == '#') ? target : sender;

    WindowPtr win = FindWindowByTarget(winTarget);
    if (!win) {
        // Open new window for private message?
        if (winTarget[0] != '#') {
             win = CreateChannelWindow(winTarget); // Reuse channel window logic for PM
        }
    }

    if (win) {
        AppendText(win, "<" + sender + "> " + text);
    } else {
        OnIRCLog(sender + " says: " + text);
    }
}

void MacApp::OnIRCJoin(const std::string& channel) {
    CreateChannelWindow(channel);
}

void MacApp::OnIRCPart(const std::string& channel) {
    WindowPtr win = FindWindowByTarget(channel);
    if (win) {
        DisposeChatWindow(win);
    }
}
