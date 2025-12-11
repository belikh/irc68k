#ifndef MAC_APP_H
#define MAC_APP_H

#ifdef LOCAL_TESTING
    #include "../include/mock_mac.h"
#else
    #include <MacTypes.h>
    #include <Quickdraw.h>
    #include <Windows.h>
    #include <Events.h>
    #include <Menus.h>
    #include <TextEdit.h>
    #include <Fonts.h>
    #include <ToolUtils.h>
    #include <Memory.h>
    #include <Dialogs.h>
#endif

#include "IRCClient.h"
#include <map>
#include <string>

// Window Types
const int kWindowTypeStatus = 1;
const int kWindowTypeChannel = 2;

struct ChatWindowData {
    int type;
    std::string target; // Channel name or "" for status
    TEHandle logTE;
    TEHandle inputTE;
    ControlHandle scrollBar; // For future expansion
};

class MacApp {
public:
    MacApp();
    ~MacApp();

    void Init();
    void Run();

private:
    bool running;
    IRCClient irc;

    // GUI Helpers
    void InitializeToolbox();
    void SetupMenus();

    // Event Handling
    void HandleEvent(EventRecord& event);
    void DoMouseDown(EventRecord& event);
    void DoKeyDown(EventRecord& event);
    void DoUpdate(EventRecord& event);
    void DoActivate(EventRecord& event);
    void DoMenuCommand(long menuResult);

    // Window Management
    WindowPtr CreateStatusWindow();
    WindowPtr CreateChannelWindow(const std::string& name);
    void ResizeWindow(WindowPtr window, Point newSize);
    void DisposeChatWindow(WindowPtr window);

    // Chat Logic
    void AppendText(WindowPtr window, const std::string& text);
    void HandleInput(WindowPtr window);
    WindowPtr FindWindowByTarget(const std::string& target);

    // IRC Callbacks
    void OnIRCLog(const std::string& text);
    void OnIRCMessage(const std::string& target, const std::string& sender, const std::string& text);
    void OnIRCJoin(const std::string& channel);
    void OnIRCPart(const std::string& channel);
};

#endif // MAC_APP_H
