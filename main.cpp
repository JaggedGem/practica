#include <iostream>
#include <fstream>
#include <sstream>
#include "tvmodule.h"

using namespace std;

int main() {
    // Initialize storage files
    if (!fileExists("Program.txt")) createFileIfNotExists("Program.txt");
    if (!fileExists("Channel.txt")) createFileIfNotExists("Channel.txt");

    // Load programs
    ifstream pFile("Program.txt");
    string line;
    while (getline(pFile, line)) {
        show s;
        string startTime;
        stringstream ss(line);
        ss >> s.name >> s.category >> startTime >> s.duration >> s.dayOfWeek >> s.channelCode;
        s.startHour = stoi(startTime.substr(0, 2));
        s.startMinute = stoi(startTime.substr(3, 2));
        programs.push_back(s);
    }
    pFile.close();

    // Load channels
    ifstream cFile("Channel.txt");
    while (getline(cFile, line)) {
        channel c;
        stringstream ss(line);
        ss >> c.code >> c.name >> c.originCountry;
        channels.push_back(c);
    }
    cFile.close();

    // Clear screen before starting the program
    clearScreen();
    
    // Start interface
    showMenu();
    return 0;
}
