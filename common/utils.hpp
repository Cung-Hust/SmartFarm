#pragma once

#include <iostream>
#include <string>
#include <vector>

using namespace std;

uint64_t localTimestamp(void);

string getDayInWeek(void);

string osExec(const string md);

string getMacGW(void);

string genUuid(void);

vector<string> splitStr(string str, string token);