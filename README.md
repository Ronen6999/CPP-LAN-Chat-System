# LAN Chat System

**A Group Project for HS 2nd Year Computer Science**  
**Gurucharan University**

A multi-user, real-time chat application built in C++ using TCP/IP sockets for local area network communication.

## Project Overview

The LAN Chat System is a client-server application that enables multiple users within a local network to communicate in real time. It implements a robust networking layer using the TCP/IP protocol with a multi-threaded architecture to handle concurrent client connections.

**Course:** HS 2nd Year Computer Science Project  
**Institution:** Gurucharan University  
**Academic Year:** 2025  
**Project Type:** Group Project

## Key Features

- 🔄 **Real-time Messaging** - Instant message delivery between all connected users
- 👥 **Multi-user Support** - Multiple concurrent clients with unique usernames
- 🤖 **Integrated Chatbot** - Quick utilities with commands like `/help`, `/time`, `/ping`
- 👨‍💼 **Admin Console** - Admin commands: `/announce`, `/remove`
- 📢 **Notifications** - Automatic user join/leave announcements to all clients
- 🔒 **Thread-safe Operations** - Mutex locks ensure data consistency across threads
- 💻 **Cross-platform Networking** - Uses TCP/IP protocol for reliable data transmission

## System Requirements

- **Language:** C++ (C++11 or later)
- **Platform:** Windows
- **API:** WinSock2 (Windows Sockets 2)
- **Compiler:** GCC, MSVC, or any C++ compiler supporting C++11

### Required Libraries

- `ws2_32.lib` - Windows Sockets API

## Installation & Setup

### 1. Clone or Download the Project

```bash
cd "cpp lan chat system"
```

### 2. Compile the Source Code

**For Server:**
```bash
g++ -o server.exe src/Main/server.cpp -lws2_32
```

**For Client:**
```bash
g++ -o client.exe src/Main/client.cpp -lws2_32
```

Or compile both with full path support:
```bash
g++ -o server.exe src/Main/server.cpp src/Bot/ChatBot.cpp -lws2_32
g++ -o client.exe src/Main/client.cpp src/Bot/ChatBot.cpp -lws2_32
```

## Usage

### Starting the Server

1. Run the server executable:
   ```bash
   server.exe
   ```

2. The server will:
   - Initialize the socket and bind to port `12345`
   - Display all available LAN IP addresses
   - Wait for incoming client connections
   - Start accepting and broadcasting messages

### Connecting Clients

1. Run the client executable:
   ```bash
   client.exe
   ```

2. When prompted:
   - Enter the **server IP address** (shown when server starts)
   - Enter your **username** (must be unique)

3. Once connected, you can:
   - Type and send messages
   - Use slash commands
   - Disconnect with `/exit`

### Available Commands

| Command | Description |
|---------|-------------|
| `/help` | Display available commands |
| `/time` | Get current server time |
| `/ping` | Check server connectivity |
| `/users` | List all connected users |
| `/announce <msg>` | Broadcast announcement (admin) |
| `/remove <username>` | Disconnect a user (admin) |
| `/exit` | Disconnect from server |

## Demo & Screenshots

### Running Application Demo

**Demo 1:**
![Demo Screenshot 1](https://res.cloudinary.com/dymxdpunk/image/upload/v1780767056/output_1_goiunl.jpg)

**Demo 2:**
![Demo Screenshot 2](https://res.cloudinary.com/dymxdpunk/image/upload/v1780767056/output_2_leitou.jpg)

**Demo 3:**
![Demo Screenshot 3](https://res.cloudinary.com/dymxdpunk/image/upload/v1780767056/output3_cvxgd0.jpg)

## Project Architecture

### System Architecture

```
┌─────────────────────────────────────┐
│         Server                      │
│  ┌─────────────────────────────┐   │
│  │ Main Thread                 │   │
│  │ (Accept Connections)        │   │
│  │ (Process Admin Input)       │   │
│  └─────────────────────────────┘   │
│  ┌──────┐ ┌──────┐ ┌──────┐        │
│  │Client│ │Client│ │Client│   ...  │
│  │Thread│ │Thread│ │Thread│        │
│  └──────┘ └──────┘ └──────┘        │
└─────────────────────────────────────┘
         ↓ TCP/IP ↓
┌─────────────────────────────────────┐
│  Client 1      Client 2      Client N  │
│  ┌─────────┐   ┌─────────┐   ┌─────────┐
│  │ Receive │   │ Receive │   │ Receive │
│  │ Thread  │   │ Thread  │   │ Thread  │
│  └─────────┘   └─────────┘   └─────────┘
└─────────────────────────────────────┘
```

### Key Components

**Server Side:**
- Accepts multiple client connections
- Broadcasts messages to all connected users
- Manages user list and usernames
- Handles admin commands and bot functions
- Uses mutex locks for thread-safe operations

**Client Side:**
- Connects to server using IP and port
- Sends and receives chat messages
- Displays console-based chat interface
- Processes slash commands
- Handles graceful disconnection

## Technical Details

### Networking Protocol

- **Protocol:** TCP/IP (Transmission Control Protocol)
- **Port:** 12345
- **Connection Type:** SOCK_STREAM (TCP socket)
- **Reliability:** TCP guarantees ordered, reliable delivery

### Key Data Structures

```cpp
std::vector<SOCKET> clients;           // Active client sockets
std::vector<std::string> usernames;    // Connected user names
std::mutex mtx;                        // Thread synchronization
```

### Threading Model

- **Server:** One main thread + one thread per connected client
- **Client:** Main thread for sending + separate thread for receiving
- **Synchronization:** `std::mutex` with `std::lock_guard` for shared resource access

### Message Flow

1. Client connects and sends username
2. Server validates username uniqueness
3. Server broadcasts "User joined" message
4. All clients can send/receive messages
5. Messages are formatted as: `[username]: [message]`
6. On disconnect, server notifies all users

## File Structure

```
cpp lan chat system/
├── README.md                          # This file
├── BIBLIOGRAPHY.txt                   # References and sources
├── Documentation_and_Report.txt       # Detailed project documentation
├── Research_and_Theory.txt            # Networking theory and concepts
├── CONCLUSION.txt                     # Project conclusions
├── src/
│   ├── Bot/
│   │   ├── ChatBot.cpp               # Chatbot implementation
│   │   └── ChatBot.h                 # Chatbot header
│   └── Main/
│       ├── server.cpp                # Server implementation
│       └── client.cpp                # Client implementation
├── Executables/                       # Compiled .exe files
└── Documentation Files                # Support documentation
```

## Building and Compilation

### Minimum Compilation Command

```bash
# Windows with GCC
g++ -o server.exe src/Main/server.cpp -lws2_32
g++ -o client.exe src/Main/client.cpp -lws2_32
```

### With Optimization Flags

```bash
g++ -O2 -o server.exe src/Main/server.cpp -lws2_32
g++ -O2 -o client.exe src/Main/client.cpp -lws2_32
```

## Team Members

| Role | Name |
|------|------|
| Team Lead & Main Developer | Ronen Singha |
| Co-Developer | Sanju |
| Research & Theory | Nabonita |
| Testing & Debugging | Pretty |
| Documentation & Report | Sumedha |
| Features & Enhancement | Mehek |

## References

- Stevens, W. Richard. "TCP/IP Illustrated, Volume 1: The Protocols." Addison-Wesley Professional, 2011.
- Beej's Guide to Network Programming Using Internet Sockets
- Stroustrup, Bjarne. "The C++ Programming Language." Addison-Wesley Professional, 2013.
- MSDN Windows Sockets 2 Documentation

## License

Academic Project - Gurucharan University (2025)

## Troubleshooting

### Server won't start
- Ensure port 12345 is not in use by another application
- Check Windows Firewall settings
- Verify WinSock2 libraries are properly installed

### Client can't connect
- Verify server is running first
- Check the server IP address is correct
- Ensure both machines are on the same LAN
- Verify port 12345 is accessible

### Username taken error
- Choose a different, unique username
- Check if other users are still connected
- Try disconnecting and reconnecting

## Future Enhancements

- [ ] Graphical User Interface (GUI)
- [ ] Encrypted message transmission
- [ ] Private messaging between users
- [ ] Message history/logging
- [ ] File transfer capabilities
- [ ] User authentication system
- [ ] Cross-platform compatibility (Linux/macOS)

---

**Last Updated:** June 6, 2026  
**Status:** Completed Educational Project
