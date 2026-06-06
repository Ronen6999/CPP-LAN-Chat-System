// =====================================================
// HS 2nd Year Computer Science Project
// Project Title : LAN Chat System in C++ using TCP/IP Sockets
// Description   : Simple LAN-based chat between computers using client-server model.
//
// Team Members  :
//   - Ronen Singha (Team Lead & Main Developer)
//   - Sanju (Co-Developer)
//   - Nabonita (Research & Theory)
//   - Pretty (Testing & Debugging)
//   - Sumedha (Documentation & Report Writing)
//   - Mehek (Feature Implementation & Enhancement)
//
// Academic Year : 2025
// Institution   : Gurucharan University
// =====================================================

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#include <cstring>
#include <chrono>
#include <windows.h>

#pragma comment(lib, "Ws2_32.lib")

static void enableAnsiColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) return;
    mode |= 0x0004;
    SetConsoleMode(hOut, mode);
}

void receiveMessages(SOCKET sock) {
    char buffer[1024];
    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;
        std::cout << "\n" << buffer << "\nYou: ";
        std::cout.flush();
    }
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed!\n";
        return 1;
    }

    enableAnsiColors();

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!\n";
        WSACleanup();
        return 1;
    }

    std::string serverIP;
    std::cout << "Enter server IP: ";
    std::getline(std::cin, serverIP);

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(12345);
    inet_pton(AF_INET, serverIP.c_str(), &server_addr.sin_addr);

    if (connect(sock, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Connection failed!\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::string username;
    while (true) {
        std::cout << "Enter your name: ";
        if (!std::getline(std::cin, username)) {
            closesocket(sock);
            WSACleanup();
            return 0;
        }
        if (username.empty()) {
            std::cout << "Name cannot be empty. Please enter a different name.\n";
            continue;
        }

        send(sock, username.c_str(), (int)username.size(), 0);

        char resp[32];
        std::memset(resp, 0, sizeof(resp));
        int rbytes = recv(sock, resp, sizeof(resp) - 1, 0);
        if (rbytes <= 0) {
            std::cerr << "Disconnected while selecting username.\n";
            closesocket(sock);
            WSACleanup();
            return 1;
        }
        std::string reply(resp, rbytes);

        if (reply.find("OK") != std::string::npos) {
            break;
        }
        if (reply.find("TAKEN") != std::string::npos) {
            std::cout << "That username is already taken. Please enter a different name.\n";
            continue;
        }

        std::cout << "Unexpected server response. Please try a different name.\n";
    }

    std::cout << "✅ Connected to chat room! Type your messages below.\n";
    std::cout << "---------------------------------------------\n";

    std::thread receiver(receiveMessages, sock);
    receiver.detach();

    std::string msg;
    while (true) {
        std::cout << "You: ";
        if (!std::getline(std::cin, msg)) break;
        if (msg == "/exit") break;

        send(sock, msg.c_str(), (int)msg.size(), 0);
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
