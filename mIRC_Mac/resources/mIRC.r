#include "Types.r"
#include "Systypes.r"

// Menu IDs
#define APPLE_MENU_ID 128
#define FILE_MENU_ID 129
#define EDIT_MENU_ID 130
#define COMMANDS_MENU_ID 131

// Window IDs
#define STATUS_WINDOW_ID 128
#define CHANNEL_WINDOW_ID 129

resource 'MBAR' (128) {
    { APPLE_MENU_ID, FILE_MENU_ID, EDIT_MENU_ID, COMMANDS_MENU_ID };
};

resource 'MENU' (APPLE_MENU_ID, "Apple", preload) {
    APPLE_MENU_ID, textMenuProc,
    0x7FFFFFFF, // Enable all items
    enabled,
    "\0x14", // Apple Logo character
    {
        "About mIRC SE/30...", noIcon, noKey, noMark, plain;
        "-", noIcon, noKey, noMark, plain
    }
};

resource 'MENU' (FILE_MENU_ID, "File", preload) {
    FILE_MENU_ID, textMenuProc,
    0x7FFFFFFF,
    enabled,
    "File",
    {
        "Connect", noIcon, "K", noMark, plain;
        "Disconnect", noIcon, "D", noMark, plain;
        "-", noIcon, noKey, noMark, plain;
        "Quit", noIcon, "Q", noMark, plain
    }
};

resource 'MENU' (EDIT_MENU_ID, "Edit", preload) {
    EDIT_MENU_ID, textMenuProc,
    0x7FFFFFFF,
    enabled,
    "Edit",
    {
        "Undo", noIcon, "Z", noMark, plain;
        "-", noIcon, noKey, noMark, plain;
        "Cut", noIcon, "X", noMark, plain;
        "Copy", noIcon, "C", noMark, plain;
        "Paste", noIcon, "V", noMark, plain;
        "Clear", noIcon, noKey, noMark, plain
    }
};

resource 'MENU' (COMMANDS_MENU_ID, "Commands", preload) {
    COMMANDS_MENU_ID, textMenuProc,
    0x7FFFFFFF,
    enabled,
    "Commands",
    {
        "Join Channel...", noIcon, "J", noMark, plain;
        "Part Channel", noIcon, "L", noMark, plain;
        "-", noIcon, noKey, noMark, plain;
        "List Channels", noIcon, noKey, noMark, plain
    }
};

// Main Status Window Template
// Top, Left, Bottom, Right
resource 'WIND' (STATUS_WINDOW_ID, "Status", preload) {
    {50, 40, 300, 480}, // Initial bounds
    zoomDocProc,        // Zoomable document window
    invisible,          // Initially invisible (shown by code)
    goAway,             // Has close box
    0x0,                // RefCon
    "mIRC Status"       // Title
};

// Channel Window Template (cloned for new channels)
resource 'WIND' (CHANNEL_WINDOW_ID, "Channel", preload) {
    {60, 50, 310, 490},
    zoomDocProc,
    invisible,
    goAway,
    0x0,
    "Channel"
};

// Application Size Resource (Memory requirements)
resource 'SIZE' (-1) {
    dontSaveScreen,
    acceptSuspendResumeEvents,
    enableOptionSwitch,
    canBackground,
    multiFinderAware,
    backgroundAndForeground,
    dontGetFrontClicks,
    ignoreChildDiedEvents,
    is32BitCompatible,
    isHighLevelEventAware,
    onlyLocalHLEvents,
    notStationeryAware,
    dontUseTextEditServices,
    reserved,
    reserved,
    reserved,

    // Memory sizes in bytes
    1024 * 1024, // Preferred size: 1MB
    512 * 1024   // Minimum size: 512KB
};
