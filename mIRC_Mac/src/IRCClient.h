#ifndef IRC_CLIENT_H
#define IRC_CLIENT_H

#include <string>
#include <vector>
#include <functional>
#include <queue>

// Forward declaration for platform specific socket
#ifdef LOCAL_TESTING
    typedef int SocketHandle;
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <netdb.h>
    #include <fcntl.h>
    typedef int SocketHandle;
#endif

class IRCClient {
public:
    enum class State {
        Disconnected,
        Connecting,
        Connected
    };

    struct Message {
        std::string prefix;
        std::string command;
        std::vector<std::string> params;
    };

    IRCClient();
    ~IRCClient();

    void Connect(const std::string& server, int port, const std::string& nick, const std::string& user, const std::string& realname);
    void Disconnect(const std::string& reason);
    void SendRaw(const std::string& data);

    // Non-blocking update loop to be called from WaitNextEvent
    void Update();

    State GetState() const { return currentState; }

    // Commands
    void Join(const std::string& channel);
    void Part(const std::string& channel);
    void PrivMsg(const std::string& target, const std::string& message);

    // Callbacks
    std::function<void(const std::string&)> onLog; // Raw log or status messages
    std::function<void(const std::string& channel, const std::string& user, const std::string& msg)> onMessage;
    std::function<void(const std::string& channel)> onJoin;
    std::function<void(const std::string& channel)> onPart;

private:
    State currentState;
    SocketHandle socketFD;
    std::string currentNick;
    std::string buffer; // Receive buffer

    void HandleData(const std::string& data);
    void ParseLine(const std::string& line);
    void ProcessMessage(const Message& msg);

    // Platform agnostic socket helpers
    bool SocketConnect(const std::string& host, int port);
    void SocketClose();
    int SocketRead(char* buf, int maxlen);
    int SocketWrite(const std::string& data);
};

#endif // IRC_CLIENT_H
