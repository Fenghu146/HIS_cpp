#pragma once

#include <string>
#include <limits>
#include <iostream>
using namespace std;

void ClearInputBuffer();
bool inputLine(string& out);
bool getConfirm();
bool confirmAction(const string& msg);
void waitForEnter();
int getValidChoice(int min, int max);
