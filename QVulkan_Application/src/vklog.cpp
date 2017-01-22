#include <vklog.h>

//STATIC SINGLETONE

Log Log::proxylog;

void Log::logSection(const std::string &msg)
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	if (!GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info))
		return;
	size_t line_size = (info.srWindow.Right - info.srWindow.Left) / 2;

	size_t size = msg.size();
	std::string bit("");
	std::string line(line_size - (size / 2) - 1, '-');

	bool rounded = false;
	if ((size % 2) == 0)
		rounded = true;

	bit.append(line + ' ').append(msg).append(' ' + line);
	if (!rounded)
		bit.pop_back();

	std::cout << bit << std::endl;
}
