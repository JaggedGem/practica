#ifndef TVMODULE_H
#define TVMODULE_H

#include <string>
#include <vector>

using namespace std;

struct show {
    string name;
    string category;
    int startHour;
    int startMinute;
    int duration;        // in minutes
    string dayOfWeek;
    string channelCode;
};

struct channel {
    string code;
    string name;
    string originCountry;
};

// Global containers
extern vector<show> programs;
extern vector<channel> channels;

// File utilities
bool fileExists(const string& fileName);
void createFileIfNotExists(const string& fileName);

// Display
void allShows();
void allChannels();

// CRUD operations
void addShow(const string& name, const string& category, const string& startTime, int duration, const string& dayOfWeek, string channelCode);
void addChannel(const string& name, const string& originCountry);
void deleteShow(const string& name);
void deleteChannel(const string& name);
void editShow(const string& name, string newName = "", string newCategory = "", string newStartTime = "", int newDuration = 0, string newDayOfWeek = "", string newChannelCode = "");
void editChannel(const string& name, string newName = "", string newOriginCountry = "");

// Summaries and queries
void broadcastSummary();
void specificDayShow(const string& day);
void maxShow();
void minShow();
void averageShow();

// Menu
void showMenu();

// ID generation
string generateNextChannelId();

#endif // TVMODULE_H
