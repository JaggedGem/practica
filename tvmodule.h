#ifndef TVMODULE_H
#define TVMODULE_H

#include <string>
#include <vector>

struct show {
    std::string name;
    std::string category;
    int startHour;
    int startMinute;
    int duration;        // in minutes
    std::string dayOfWeek;
    std::string channelCode;
};

struct channel {
    std::string code;
    std::string name;
    std::string originCountry;
};

// Global containers
extern std::vector<show> programs;
extern std::vector<channel> channels;

// File utilities
bool fileExists(const std::string& fileName);
void createFileIfNotExists(const std::string& fileName);

// Display
void allShows();
void allChannels();

// CRUD operations
void addShow(std::string name, std::string category, const std::string& startTime, int duration, std::string dayOfWeek, std::string channelCode);
void addChannel(std::string code, std::string name, std::string originCountry);
void deleteShow(const std::string& name);
void deleteChannel(const std::string& name);
void editShow(const std::string& name, std::string newName = "", std::string newCategory = "", std::string newStartTime = "", int newDuration = 0, std::string newDayOfWeek = "", std::string newChannelCode = "");
void editChannel(const std::string& name, std::string newCode = "", std::string newName = "", std::string newOriginCountry = "");

// Summaries and queries
void broadcastSummary();
void specificDayShow(const std::string& day);
void maxShow();
void minShow();
void averageShow();

// Menu
void showMenu();

#endif // TVMODULE_H
