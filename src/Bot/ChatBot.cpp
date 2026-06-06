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
// =======================================================

#include "ChatBot.h"

#include <algorithm>
#include <cctype>
#include <sstream>
#include <chrono>
#include <string>

static std::string toLowerCopy(const std::string &s) {
	std::string out = s;
	for (char &c : out) c = (char)std::tolower((unsigned char)c);
	return out;
}

void ChatBot::processCommand(const std::string &senderName,
							   const std::string &text,
							   bool isAdmin,
							   uintptr_t senderSocketKey,
							   const ChatContext &ctx) const {
	if (text.empty() || text[0] != '/') return; // Not a command

	// Split command and args
	std::string cmd, args;
	{
		std::istringstream iss(text);
		std::getline(iss, cmd, ' ');
		std::getline(iss, args);
	}
	std::string cmdLower = toLowerCopy(cmd);

	if (cmdLower == "/help") {
		handleHelp(senderSocketKey, ctx, isAdmin);
		return;
	}
	if (cmdLower == "/allusers") {
		handleAllUsers(senderSocketKey, ctx);
		return;
	}

	if (cmdLower == "/time") {
		handleTime(senderSocketKey, ctx);
		return;
	}
	if (cmdLower == "/announce") {
		handleAnnounce(args, isAdmin, ctx);
		return;
	}
	if (cmdLower == "/remove") {
		handleRemove(args, isAdmin, senderSocketKey, ctx);
		return;
	}

	ctx.sendToClient(senderSocketKey, "Bot: Unknown command. Type /help");
}

void ChatBot::handleHelp(uintptr_t sender, const ChatContext &ctx, bool isAdmin) const {
    std::string msg = "Bot: Available commands:\n"
        " - /help\n"
        " - /allusers\n"
        " - /time\n";
	if (isAdmin) {
		msg += " - /announce <text>\n - /remove <username>\n";
	}
	ctx.sendToClient(sender, msg);
}

void ChatBot::handleAllUsers(uintptr_t sender, const ChatContext &ctx) const {
	auto users = ctx.listUsers();
	std::ostringstream oss;
	oss << "Bot: Users (" << users.size() << ")\n";
	for (const auto &p : users) {
		oss << " - " << p.first << " (" << p.second << ")\n";
	}
	ctx.sendToClient(sender, oss.str());
}


void ChatBot::handleTime(uintptr_t sender, const ChatContext &ctx) const {
	ctx.sendToClient(sender, std::string("Bot: ") + ctx.getTimeString());
}

void ChatBot::handleAnnounce(const std::string &args, bool isAdmin, const ChatContext &ctx) const {
	if (!isAdmin) {
		ctx.broadcastToAll("Bot: You are not authorized to use /announce");
		return;
	}
	std::string text = args;
	if (text.empty()) {
		ctx.broadcastToAll("Bot: /announce <text>");
		return;
	}
=
    const std::string boldRed = "\x1b[1;31m";
    const std::string reset = "\x1b[0m";
    ctx.broadcastToAll(boldRed + std::string("ANNOUNCEMENT: ") + text + reset);
}

void ChatBot::handleRemove(const std::string &args, bool isAdmin, uintptr_t sender, const ChatContext &ctx) const {
	if (!isAdmin) {
		ctx.sendToClient(sender, "Bot: You are not authorized to use /remove");
		return;
	}
	std::string target = args;
	// trim spaces
	while (!target.empty() && std::isspace((unsigned char)target.front())) target.erase(target.begin());
	while (!target.empty() && std::isspace((unsigned char)target.back())) target.pop_back();
	if (target.empty()) {
		ctx.sendToClient(sender, "Bot: /remove <username>");
		return;
	}
	bool ok = ctx.kickUserByName(target);
	if (ok) ctx.broadcastToAll(std::string("Bot: ") + target + " was removed by admin");
	else ctx.sendToClient(sender, std::string("Bot: user not found: ") + target);
}
