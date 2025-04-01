#pragma once
#include <string>

struct ChatMessage {
private:
	std::wstring text;
	bool IsMyMessage;

public:
	ChatMessage(std::wstring msg, bool isMine) : text(msg), IsMyMessage(isMine) { }

	std::wstring Text() const { return text; }
	bool IsMyChat() const { return IsMyMessage; }
};