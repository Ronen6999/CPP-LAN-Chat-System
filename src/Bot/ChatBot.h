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
// ====
#pragma once

#include <string>
#include <vector>
#include <utility>
#include <functional>

struct ChatContext {

	std::function<void(uintptr_t, const std::string &)> sendToClient;

	std::function<void(const std::string &)> broadcastToAll;

	std::function<std::vector<std::pair<std::string, std::string>>()> listUsers;

	std::function<bool(const std::string &)> kickUserByName;

	std::function<std::string()> getTimeString;
};

class ChatBot {
public:
	ChatBot() = default;
	void processCommand(const std::string &senderName,
					   const std::string &text,
					   bool isAdmin,
					   uintptr_t senderSocketKey,
					   const ChatContext &ctx) const;

private:
	void handleHelp(uintptr_t sender, const ChatContext &ctx, bool isAdmin) const;
	void handleAllUsers(uintptr_t sender, const ChatContext &ctx) const;
	void handleTime(uintptr_t sender, const ChatContext &ctx) const;
	void handleAnnounce(const std::string &args, bool isAdmin, const ChatContext &ctx) const;
	void handleRemove(const std::string &args, bool isAdmin, uintptr_t sender, const ChatContext &ctx) const;
};
