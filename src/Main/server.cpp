// =====================================================
// HS 2nd Year Computer Science Project
// Project Title : LAN Chat System in C++ using TCP/IP Sockets
// Description   : Multi-user LAN chat system using TCP/IP sockets.
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
#include <vector>
#include <mutex>
#include <cstring>
#include <algorithm>
#include <ctime>
#include <sstream>
#include <windows.h>
#include "../Bot/ChatBot.h"

#pragma comment(lib, "Ws2_32.lib")

std::vector<SOCKET> clients;
std::vector<std::string> usernames;
std::vector<std::string> clientIps;
std::mutex mtx;

static ChatBot g_bot;

static void enableAnsiColors() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hOut == INVALID_HANDLE_VALUE) return;
	DWORD mode = 0;
	if (!GetConsoleMode(hOut, &mode)) return;
	mode |= 0x0004;
	SetConsoleMode(hOut, mode);
}

void printLocalIPv4s() {
	char hostname[256] = {0};
	if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
		return;
	}

	addrinfo hints{};
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	addrinfo *result = nullptr;
	if (getaddrinfo(hostname, nullptr, &hints, &result) != 0) {
		return;
	}

	std::cout << " Server LAN IPv4 addresses (use one of these on clients):\n";
	for (addrinfo *p = result; p != nullptr; p = p->ai_next) {
		sockaddr_in *addr = reinterpret_cast<sockaddr_in *>(p->ai_addr);
		char ip[INET_ADDRSTRLEN] = {0};
		if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip))) {
			std::cout << " - " << ip << "\n";
		}
	}
	freeaddrinfo(result);
}

void broadcastMessage(const std::string &msg, SOCKET sender) {
    std::lock_guard<std::mutex> lock(mtx);
    for (auto client : clients) {
        if (client != sender) {
            send(client, msg.c_str(), (int)msg.size(), 0);
        }
    }
}

static void sendToSocket(SOCKET s, const std::string &msg) {
    send(s, msg.c_str(), (int)msg.size(), 0);
}

void adminInputLoop() {
	std::string line;
	while (true) {
		if (!std::getline(std::cin, line)) {
			break;
		}
		if (line.empty()) {
			continue;
		}
        if (!line.empty() && line[0] == '/') {
			ChatContext ctx{};
            ctx.broadcastToAll = [](const std::string &m){

                std::cout << m << "\n";
                broadcastMessage(m, INVALID_SOCKET);
            };
            ctx.sendToClient = [](uintptr_t /*key*/, const std::string &m){

                std::cout << m << "\n";
            };
			ctx.listUsers = [](){
				std::lock_guard<std::mutex> lock(mtx);
				std::vector<std::pair<std::string, std::string>> v;
				for (size_t i = 0; i < usernames.size(); ++i) {
					v.emplace_back(usernames[i], i < clientIps.size() ? clientIps[i] : std::string("unknown"));
				}
				return v;
			};
			ctx.kickUserByName = [](const std::string &name){
				std::lock_guard<std::mutex> lock(mtx);
				std::string targetLower = name; for (char &c : targetLower) c = (char)std::tolower((unsigned char)c);
				for (size_t i = 0; i < usernames.size(); ++i) {
					std::string lu = usernames[i]; for (char &c : lu) c = (char)std::tolower((unsigned char)c);
					if (lu == targetLower) {
						SOCKET s = clients[i];
						const char *kick = "KICK";
						send(s, kick, (int)std::strlen(kick), 0);
						shutdown(s, SD_BOTH);
						closesocket(s);

						return true;
					}
				}
				return false;
			};
			ctx.getTimeString = [](){
				std::time_t t = std::time(nullptr);
				std::tm tm{};
				localtime_s(&tm, &t);
				char buf[64];
				std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
				return std::string(buf);
			};
			g_bot.processCommand("ADMIN", line, true, 0, ctx);
		} else {
			std::string adminMsg = std::string("ADMIN: ") + line;

            std::cout << adminMsg << "\n";
            broadcastMessage(adminMsg, INVALID_SOCKET);
		}
	}
}

void broadcastUserList() {
    std::lock_guard<std::mutex> lock(mtx);
    std::string list = "\n Users in Room:\n";
    for (const auto &u : usernames) {
        list += " - " + u + "\n";
    }
    list += "----------------------------\n";
    for (auto client : clients) {
        send(client, list.c_str(), (int)list.size(), 0);
    }
}


void handleClient(SOCKET clientSocket, const std::string &clientIp) {
    char buffer[1024];
    std::string username;


    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        username = buffer;


        std::string lowered = username;
        for (char &c : lowered) c = (char)std::tolower((unsigned char)c);
        bool taken = false;
        {
            std::lock_guard<std::mutex> lock(mtx);
            for (const auto &u : usernames) {
                std::string lu = u;
                for (char &c : lu) c = (char)std::tolower((unsigned char)c);
                if (lu == lowered) {
                    taken = true;
                    break;
                }
            }
        }
        if (taken) {
            const char *resp = "TAKEN";
            send(clientSocket, resp, (int)std::strlen(resp), 0);

            while (true) {
                bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytes <= 0) {
                    closesocket(clientSocket);
                    return;
                }
                buffer[bytes] = '\0';
                username = buffer;
                lowered = username;
                for (char &c : lowered) c = (char)std::tolower((unsigned char)c);
                bool nowTaken = false;
                {
                    std::lock_guard<std::mutex> lock(mtx);
                    for (const auto &u : usernames) {
                        std::string lu = u;
                        for (char &c : lu) c = (char)std::tolower((unsigned char)c);
                        if (lu == lowered) { nowTaken = true; break; }
                    }
                }
                if (!nowTaken) break;
                const char *resp2 = "TAKEN";
                send(clientSocket, resp2, (int)std::strlen(resp2), 0);
            }
        }


        const char *ok = "OK";
        send(clientSocket, ok, (int)std::strlen(ok), 0);

        {
            std::lock_guard<std::mutex> lock(mtx);
            clients.push_back(clientSocket);
            usernames.push_back(username);
            clientIps.push_back(clientIp);
        }

        std::string joinMsg = username + " joined the room.\n";
        std::cout << joinMsg;
        broadcastMessage(joinMsg, clientSocket);
        broadcastUserList();
    }


    while (true) {
        std::memset(buffer, 0, sizeof(buffer));
        bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';
        std::string incoming = buffer;

        if (!incoming.empty() && incoming[0] == '/') {
            ChatContext ctx{};
            ctx.broadcastToAll = [](const std::string &m){ broadcastMessage(m, INVALID_SOCKET); };
            ctx.sendToClient = [clientSocket](uintptr_t key, const std::string &m){ sendToSocket((SOCKET)(key ? key : clientSocket), m); };
            ctx.listUsers = [](){
                std::lock_guard<std::mutex> lock(mtx);
                std::vector<std::pair<std::string, std::string>> v;
                for (size_t i = 0; i < usernames.size(); ++i) {
                    v.emplace_back(usernames[i], i < clientIps.size() ? clientIps[i] : std::string("unknown"));
                }
                return v;
            };
            ctx.kickUserByName = [](const std::string &name){
                std::lock_guard<std::mutex> lock(mtx);
                std::string targetLower = name; for (char &c : targetLower) c = (char)std::tolower((unsigned char)c);
                for (size_t i = 0; i < usernames.size(); ++i) {
                    std::string lu = usernames[i]; for (char &c : lu) c = (char)std::tolower((unsigned char)c);
                    if (lu == targetLower) {
                        SOCKET s = clients[i];
                        const char *kick = "KICK";
                        send(s, kick, (int)std::strlen(kick), 0);
                        shutdown(s, SD_BOTH);
                        closesocket(s);
                        return true;
                    }
                }
                return false;
            };
            ctx.getTimeString = [](){
                std::time_t t = std::time(nullptr);
                std::tm tm{};
                localtime_s(&tm, &t);
                char buf[64];
                std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
                return std::string(buf);
            };
            g_bot.processCommand(username, incoming, false, (uintptr_t)clientSocket, ctx);
            continue;
        }

        std::string msg = username + ": " + incoming;
        std::cout << msg << "\n";
        broadcastMessage(msg, clientSocket);
    }

    // On disconnect
    {
        std::lock_guard<std::mutex> lock(mtx);
        auto it = std::find(clients.begin(), clients.end(), clientSocket);
        if (it != clients.end()) {
            int index = (int)std::distance(clients.begin(), it);
            clients.erase(it);
            usernames.erase(usernames.begin() + index);
            if (index >= 0 && index < (int)clientIps.size()) clientIps.erase(clientIps.begin() + index);
        }
    }

    std::string leaveMsg = username + " left the room.\n";
    std::cout << leaveMsg;
    broadcastMessage(leaveMsg, clientSocket);
    broadcastUserList();
    closesocket(clientSocket);
}

int main() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed!\n";
        return 1;
    }

    enableAnsiColors();

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed!\n";
        WSACleanup();
        return 1;
    }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(12345);

    if (bind(server_fd, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Bind failed!\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 5) < 0) {
        std::cerr << "Listen failed!\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << " Chat Room Server started on port 12345...\n";
	printLocalIPv4s();


	std::thread(adminInputLoop).detach();

    while (true) {
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);
        SOCKET client_fd = accept(server_fd, (sockaddr *)&client_addr, &client_len);

        if (client_fd == INVALID_SOCKET) {
            std::cerr << "Accept failed!\n";
            continue;
        }

        char ip[INET_ADDRSTRLEN] = {0};
        inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));

        std::thread(handleClient, client_fd, std::string(ip)).detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
