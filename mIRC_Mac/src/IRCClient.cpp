#include "IRCClient.h"
#include <iostream>
#include <sstream>

#ifdef LOCAL_TESTING
    // Dummy socket impl for local testing
    #include <unistd.h>
    #include <fcntl.h>
    #include <errno.h>
#endif

IRCClient::IRCClient() : currentState(State::Disconnected), socketFD(-1) {
}

IRCClient::~IRCClient() {
    Disconnect("Client exiting");
}

void IRCClient::Connect(const std::string& server, int port, const std::string& nick, const std::string& user, const std::string& realname) {
    if (currentState != State::Disconnected) {
        Disconnect("Reconnecting");
    }

    currentNick = nick;
    if (onLog) onLog("Connecting to " + server + "...");

    if (SocketConnect(server, port)) {
        currentState = State::Connecting;

        // Send registration
        SendRaw("NICK " + nick);
        SendRaw("USER " + user + " 0 * :" + realname);

        currentState = State::Connected; // Simplified for MVP (real world waits for 001)
        if (onLog) onLog("Connected.");
    } else {
        if (onLog) onLog("Connection failed.");
        currentState = State::Disconnected;
    }
}

void IRCClient::Disconnect(const std::string& reason) {
    if (currentState != State::Disconnected) {
        SendRaw("QUIT :" + reason);
        SocketClose();
        currentState = State::Disconnected;
        if (onLog) onLog("Disconnected: " + reason);
    }
}

void IRCClient::SendRaw(const std::string& data) {
    if (socketFD != -1) {
        std::string out = data + "\r\n";
        SocketWrite(out);
        // Debug
        // if (onLog) onLog("-> " + data);
    }
}

void IRCClient::Update() {
    if (currentState == State::Disconnected) return;

    char buf[1024];
    int bytes = SocketRead(buf, sizeof(buf) - 1);

    if (bytes > 0) {
        buf[bytes] = '\0';
        buffer.append(buf);
        HandleData(buffer);
    } else if (bytes == 0) {
        // Disconnected by remote
        Disconnect("Remote host closed connection");
    } else {
        // Error or EWOULDBLOCK
    }
}

void IRCClient::HandleData(const std::string& data) {
    size_t pos = 0;
    size_t found;

    while ((found = buffer.find("\r\n", pos)) != std::string::npos) {
        std::string line = buffer.substr(pos, found - pos);
        ParseLine(line);
        pos = found + 2;
    }
    buffer.erase(0, pos);
}

void IRCClient::ParseLine(const std::string& line) {
    if (line.empty()) return;

    // Handle PING immediately
    if (line.substr(0, 4) == "PING") {
        SendRaw("PONG " + line.substr(5));
        return;
    }

    Message msg;
    std::stringstream ss(line);
    std::string segment;

    // Prefix
    if (line[0] == ':') {
        ss >> segment;
        msg.prefix = segment.substr(1);
    }

    // Command
    ss >> msg.command;

    // Params
    while (ss >> segment) {
        if (segment[0] == ':') {
            std::string trailing;
            getline(ss, trailing);
            msg.params.push_back(segment.substr(1) + trailing);
            break;
        } else {
            msg.params.push_back(segment);
        }
    }

    ProcessMessage(msg);
}

void IRCClient::ProcessMessage(const Message& msg) {
    // Log raw (verbose) or specific events
    // if (onLog) onLog(msg.command + " " + (msg.params.empty() ? "" : msg.params[0]));

    if (msg.command == "PRIVMSG" && msg.params.size() >= 2) {
        std::string target = msg.params[0];
        std::string text = msg.params[1];

        // Extract nick from prefix (nick!user@host)
        std::string sender = msg.prefix;
        size_t bang = sender.find('!');
        if (bang != std::string::npos) {
            sender = sender.substr(0, bang);
        }

        if (onMessage) onMessage(target, sender, text);
    }
    else if (msg.command == "JOIN" && !msg.params.empty()) {
        if (onJoin) onJoin(msg.params[0]);
    }
    else if (msg.command == "PART" && !msg.params.empty()) {
        if (onPart) onPart(msg.params[0]);
    }
    // Handle numeric responses for login verification if needed
}

void IRCClient::Join(const std::string& channel) {
    SendRaw("JOIN " + channel);
}

void IRCClient::Part(const std::string& channel) {
    SendRaw("PART " + channel);
}

void IRCClient::PrivMsg(const std::string& target, const std::string& message) {
    SendRaw("PRIVMSG " + target + " :" + message);
}

// Platform Sockets
bool IRCClient::SocketConnect(const std::string& host, int port) {
#ifdef LOCAL_TESTING
    // Dummy
    socketFD = 1;
    return true;
#else
    struct sockaddr_in serv_addr;
    struct hostent *server;

    socketFD = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFD < 0) return false;

    server = gethostbyname(host.c_str());
    if (server == NULL) return false;

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(socketFD, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        return false;
    }

    // Set non-blocking
    fcntl(socketFD, F_SETFL, O_NONBLOCK);
    return true;
#endif
}

void IRCClient::SocketClose() {
#ifndef LOCAL_TESTING
    if (socketFD != -1) {
        close(socketFD);
        socketFD = -1;
    }
#endif
}

int IRCClient::SocketRead(char* buf, int maxlen) {
#ifdef LOCAL_TESTING
    return -1; // No data in test
#else
    if (socketFD == -1) return -1;
    return recv(socketFD, buf, maxlen, 0);
#endif
}

int IRCClient::SocketWrite(const std::string& data) {
#ifdef LOCAL_TESTING
    return data.length();
#else
    if (socketFD == -1) return -1;
    return send(socketFD, data.c_str(), data.length(), 0);
#endif
}
