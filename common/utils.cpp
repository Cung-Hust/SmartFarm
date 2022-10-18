#include "utils.hpp"

#include <random>
#include <time.h>
#include <ctime>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <algorithm>

uint64_t localTimestamp(void)
{
	auto gmt = std::time(nullptr);
	struct tm *now = localtime(&gmt);
	return (uint64_t)gmt + (uint64_t)now->tm_gmtoff;
}

string getDayInWeek()
{
	int cTime = localTimestamp();
	int date = ((int)(floor(cTime / 86400) + 4)) % 7;
	string dataStr;
	switch (date)
	{
	case 0:
		dataStr = "Sun";
		break;
	case 1:
		dataStr = "Mon";
		break;
	case 2:
		dataStr = "Tue";
		break;
	case 3:
		dataStr = "Wed";
		break;
	case 4:
		dataStr = "Thu";
		break;
	case 5:
		dataStr = "Fri";
		break;
	case 6:
		dataStr = "Sat";
		break;
	}
	return dataStr;
}

string osExec(const string cmd)
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}

	return result;
}

string getMacGW(void)
{
	std::string str = osExec("cat /sys/class/net/eth0/address");
	str.erase(std::remove(str.begin(), str.end(), ':'), str.end());
	str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());

	return str;
}

string genUuid()
{
	std::array<char, 128> buffer;
	std::string result;
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("uuidgen", "r"), pclose);
	if (!pipe)
	{
		throw std::runtime_error("popen() failed!");
	}
	while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
	{
		result += buffer.data();
	}
	result.erase(remove(result.begin(), result.end(), ':'), result.end());
	result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());

	return result;
}

vector<string> splitStr(string str, string token)
{
	std::vector<string> result;
	while (str.size())
	{
		std::size_t index = str.find(token);
		if (index != string::npos)
		{
			result.push_back(str.substr(0, index));
			str = str.substr(index + token.size());
			if (str.size() == 0)
				result.push_back(str);
		}
		else
		{
			result.push_back(str);
			str = "";
		}
	}
	return result;
}