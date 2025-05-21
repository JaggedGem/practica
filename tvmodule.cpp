#include "tvmodule.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <map>
#include <iomanip>
#include <filesystem>

using namespace std;

// Define global containers
vector<show> programs;
vector<channel> channels;

// Function to clear the screen (cross-platform)
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static string encode(const string& s) {
    string r;
    for (char c : s) r += (c == ' ' ? '_' : c);
    return r;
}

static string decode(const string& s) {
    string r;
    for (char c : s) r += (c == '_' ? ' ' : c);
    return r;
}

bool fileExists(const string& fileName) {
    ifstream file(fileName);
    return file.good();
}

void createFileIfNotExists(const string& fileName) {
    if (!fileExists(fileName)) {
        ofstream file(fileName);
        file.close();
        cout << "Created new file: " << fileName << endl;
    }
}

string generateNextChannelId() {
    int maxId = 0;
    
    // Find the highest existing ID
    for (const auto& c : channels) {
        try {
            int currentId = stoi(c.code);
            maxId = max(maxId, currentId);
        } catch (const invalid_argument&) {
            // Skip non-numeric IDs
        } catch (const out_of_range&) {
            // Skip IDs that are too large
        }
    }
    
    // Return next ID as string
    return to_string(maxId + 1);
}

void allShows() {
    if (programs.empty()) {
        cout << "No shows available." << endl;
        return;
    }

    // First pass: determine needed column widths based on content
    int nameWidth = 4;      // minimum width for "Name"
    int categoryWidth = 8;  // minimum width for "Category"
    int timeWidth = 10;     // minimum width for "Start Time"
    int durationWidth = 8;  // minimum width for "Duration"
    int dayWidth = 3;       // minimum width for "Day"
    int channelWidth = 12;  // minimum width for "Channel Code"

    // Determine maximum content width for each column
    for (const auto& s : programs) {
        nameWidth = max(nameWidth, static_cast<int>(decode(s.name).length()));
        categoryWidth = max(categoryWidth, static_cast<int>(decode(s.category).length()));
        dayWidth = max(dayWidth, static_cast<int>(decode(s.dayOfWeek).length()));
        channelWidth = max(channelWidth, static_cast<int>(s.channelCode.length()));

        // Calculate duration string length and consider it for column width
        string durationStr = to_string(s.duration) + " min";
        durationWidth = max(durationWidth, static_cast<int>(durationStr.length()));

        // Calculate time string length
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                          (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        timeWidth = max(timeWidth, static_cast<int>(startTime.length()));
    }

    // Add padding (1 space on each side)
    nameWidth += 2;
    categoryWidth += 2;
    timeWidth += 2;
    durationWidth += 2;
    dayWidth += 2;
    channelWidth += 2;

    // Print header
    int totalWidth = nameWidth + categoryWidth + timeWidth + durationWidth + dayWidth + channelWidth + 7; // 7 for the separators

    cout << endl << string(totalWidth, '-') << endl;
    cout << "|" << setw(nameWidth) << left << " Name"
         << "|" << setw(categoryWidth) << " Category"
         << "|" << setw(timeWidth) << " Start Time"
         << "|" << setw(durationWidth) << " Duration"
         << "|" << setw(dayWidth) << " Day"
         << "|" << setw(channelWidth) << " Channel Code" << "|" << endl;
    cout << string(totalWidth, '-') << endl;

    // Print data rows
    for (const auto& s : programs) {
        string name = decode(s.name);
        string category = decode(s.category);
        string day = decode(s.dayOfWeek);
        string channelCode = s.channelCode;
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                         (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        string durationStr = to_string(s.duration) + " min";

        cout << "|" << setw(nameWidth) << " " + name
             << "|" << setw(categoryWidth) << " " + category
             << "|" << setw(timeWidth) << " " + startTime
             << "|" << setw(durationWidth) << " " + durationStr
             << "|" << setw(dayWidth) << " " + day
             << "|" << setw(channelWidth) << " " + channelCode << "|" << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << programs.size() << " shows found." << endl;
}

void allChannels() {
    if (channels.empty()) {
        cout << "No channels available." << endl;
        return;
    }

    // First pass: determine needed column widths based on content
    int codeWidth = 4;      // minimum width for "Code"
    int nameWidth = 4;      // minimum width for "Name"
    int countryWidth = 17;   // minimum width for "Country"

    // Determine maximum content width for each column
    for (const auto& c : channels) {
        codeWidth = max(codeWidth, static_cast<int>(c.code.length()) + 1);
        nameWidth = max(nameWidth, static_cast<int>(c.name.length()) + 1);
        countryWidth = max(countryWidth, static_cast<int>(c.originCountry.length()) + 1);
    }

    // Print header
    int totalWidth = codeWidth + nameWidth + countryWidth + (3 * 3) + 1;
    cout << string(totalWidth, '-') << endl;
    cout << "| " << left << setw(codeWidth) << "Code"
         << " | " << setw(nameWidth) << "Name"
         << " | " << setw(countryWidth) << "Country of Origin" << " |" << endl;
    cout << string(totalWidth, '-') << endl;

    // And update the data rows to match:
    for (const auto& c : channels) {
        cout << "| " << setw(codeWidth) << c.code
             << " | " << setw(nameWidth) << decode(c.name)
             << " | " << setw(countryWidth) << decode(c.originCountry) << " |" << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << channels.size() << " channels found." << endl;
}

void addShow(const string& name, const string& category, const string& startTime, int duration, const string& dayOfWeek, string channelCode) {
    if (name.empty() || category.empty() || startTime.empty() || duration <= 0 || dayOfWeek.empty() || channelCode.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    string encName = encode(name);
    string encCategory = encode(category);
    string encDay = encode(dayOfWeek);

    if (ranges::any_of(programs, [&encName](const show& s) { return s.name == encName; })) {
        cout << "Show with this name already exists." << endl;
        return;
    }

    bool channelExists = ranges::any_of(channels, [&channelCode](const channel& c) { return c.code == channelCode; });
    if (!channelExists) {
        cout << "Error: Channel code does not exist. Please enter a valid channel code." << endl;
        return;
    }

    int startHour = 0, startMinute = 0;
    size_t colonPos = startTime.find(':');
    if (colonPos != string::npos) {
        try {
            startHour = stoi(startTime.substr(0, colonPos));
            startMinute = stoi(startTime.substr(colonPos + 1));

            // Validate time ranges
            if (startHour < 0 || startHour > 23 || startMinute < 0 || startMinute > 59) {
                cout << "Invalid time. Hours must be 0-23, minutes must be 0-59." << endl;
                return; // or set to default values
            }
        } catch (const exception& e) {
            cout << "Invalid time format: " << e.what() << endl;
            return; // or set to default values
        }
    }

    show s;
    s.name = encName;
    s.category = encCategory;
    s.startHour = startHour;
    s.startMinute = startMinute;
    s.duration = duration;
    s.dayOfWeek = encDay;
    s.channelCode = channelCode;
    programs.push_back(s);

    createFileIfNotExists("Program.txt");
    ofstream o("Program.txt", ios::app);
    o << s.name << ' ' << s.category << ' '
      << (s.startHour < 10 ? "0" : "") << s.startHour << ':'
      << (s.startMinute < 10 ? "0" : "") << s.startMinute << ' '
      << s.duration << ' ' << s.dayOfWeek << ' ' << s.channelCode << endl;
    o.close();

    cout << "Show added successfully." << endl;
}

void addChannel(const string& name, const string& originCountry) {
    if (name.empty() || originCountry.empty()) {
        cout << "Invalid input. Please provide valid channel details." << endl;
        return;
    }

    string encName = encode(name);
    string encCountry = encode(originCountry);

    if (ranges::any_of(channels, [&encName](const channel& c) { return c.name == encName; })) {
        cout << "Channel with this name already exists." << endl;
        return;
    }

    string code = generateNextChannelId();
    
    channel c;
    c.code = code;
    c.name = encName;
    c.originCountry = encCountry;
    channels.push_back(c);

    createFileIfNotExists("Channel.txt");
    ofstream o("Channel.txt", ios::app);
    o << c.code << ' ' << c.name << ' ' << c.originCountry << endl;
    o.close();
    
    cout << "Channel added successfully with ID: " << code << endl;
}

void deleteShow(const string& name) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    string encName = encode(name);
    auto it = ranges::remove_if(programs, [&encName](const show& s) { return s.name == encName; }).begin();
    if (it != programs.end()) {
        programs.erase(it, programs.end());
        cout << "Show deleted successfully." << endl;
    } else {
        cout << "Show not found." << endl;
    }
    if (!fileExists("Program.txt")) {
        cout << "File not found. Cannot update." << endl;
        return;
    }
    ofstream o("Program.txt");
    for (const auto& s : programs) {
        string startTimeStr = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                              (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        o << s.name << " " << s.category << " " << startTimeStr << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
    }
}

void deleteChannel(const string& name) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    string encName = encode(name);
    auto it = ranges::remove_if(channels, [&encName](const channel& c) { return c.name == encName; }).begin();
    if (it != channels.end()) {
        channels.erase(it, channels.end());
        cout << "Channel deleted successfully." << endl;
    } else {
        cout << "Channel not found." << endl;
    }
    if (!fileExists("Channel.txt")) {
        cout << "File not found. Cannot update." << endl;
        return;
    }
    ofstream o("Channel.txt");
    for (const auto& c : channels) {
        o << c.code << " " << c.name << " " << c.originCountry << endl;
    }
}

void editShow(const string& name, string newName, string newCategory, string newStartTime, int newDuration, string newDayOfWeek, string newChannelCode) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }
    string encName = encode(name);

    auto it = ranges::find_if(programs, [&encName](const show& s) { return s.name == encName; });
    if (it != programs.end()) {
        cout << "Editing show: " << decode(it->name) << endl;

        if (newName.empty() || newCategory.empty() || newStartTime.empty() ||
            newDuration == 0 || newDayOfWeek.empty() || newChannelCode.empty()) {
            cout << "Enter new details (leave blank to keep current value):" << endl;
        }

        if (newName.empty()) {
            cout << "Name: ";
            getline(cin, newName);
            newName = encode(newName);
            if (!newName.empty() && ranges::any_of(programs, [&](const show& s) {
                return s.name == newName && s.name != name; })) {
                cout << "Show with this name already exists." << endl;
                return;
            }
            if (!newName.empty()) {
                it->name = move(newName);
            }
        } else {
            newName = encode(newName);
            if (ranges::any_of(programs, [&](const show& s) {
                return s.name == newName && s.name != name; })) {
                cout << "Show with this name already exists." << endl;
                return;
            }
            it->name = move(newName);
        }

        if (newCategory.empty()) {
            cout << "Category: ";
            getline(cin, newCategory);
            newCategory = encode(newCategory);
            if (!newCategory.empty()) {
                it->category = move(newCategory);
            }
        } else {
            newCategory = encode(newCategory);
            it->category = move(newCategory);
        }

        if (newStartTime.empty()) {
            cout << "Start Time (HH:MM): ";
            getline(cin, newStartTime);
        }

        if (!newStartTime.empty()) {
            try {
                // Find the position of the colon
                size_t colonPos = newStartTime.find(':');
                if (colonPos != string::npos) {
                    // Extract hours and minutes
                    string hourStr = newStartTime.substr(0, colonPos);
                    string minStr = newStartTime.substr(colonPos + 1);

                    // Convert to integers
                    int hour = stoi(hourStr);
                    int minute = stoi(minStr);

                    // Validate the time
                    if (hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59) {
                        it->startHour = hour;
                        it->startMinute = minute;
                    } else {
                        cout << "Invalid time values. Hours must be 0-23, minutes 0-59." << endl;
                    }
                } else {
                    cout << "Invalid time format. Use HH:MM format." << endl;
                }
            } catch (const exception& e) {
                cout << "Error parsing time: " << e.what() << ". Using original time." << endl;
            }
        }

        if (!newDuration) {
            cout << "Duration: ";
            string durationStr;
            getline(cin, durationStr);

            if (!durationStr.empty()) {
                try {
                    newDuration = stoi(durationStr);
                    if (newDuration <= 0) {
                        cout << "Invalid duration. Please provide a positive value." << endl;
                    } else {
                        it->duration = newDuration;
                    }
                } catch (const exception& e) {
                    cout << "Error parsing duration: " << e.what() << endl;
                }
            }
        } else {
            it->duration = newDuration;
        }

        if (newDayOfWeek.empty()) {
            cout << "Day of Week: ";
            getline(cin, newDayOfWeek);
            if (!newDayOfWeek.empty()) {
                it->dayOfWeek = move(newDayOfWeek);
            }
        } else {
            it->dayOfWeek = move(newDayOfWeek);
        }

        if (newChannelCode.empty()) {
            cout << "Channel Code: ";
            getline(cin, newChannelCode);
            if (!newChannelCode.empty()) {
                // Check if channel code exists
                bool channelExists = ranges::any_of(channels,
                                                    [&newChannelCode](const channel& c) { return c.code == newChannelCode; });
                if (!channelExists) {
                    cout << "Error: Channel code does not exist. Channel not updated." << endl;
                    return;
                }
                it->channelCode = move(newChannelCode);
            }
        } else {
            // Check if channel code exists
            bool channelExists = ranges::any_of(channels,
                                                [&newChannelCode](const channel& c) { return c.code == newChannelCode; });
            if (!channelExists) {
                cout << "Error: Channel code does not exist. Channel not updated." << endl;
                return;
            }
            it->channelCode = move(newChannelCode);
        }

        if (!fileExists("Program.txt")) {
            cout << "File not found. Cannot update." << endl;
            return;
        }
        ofstream o("Program.txt");
        for (const auto& s : programs) {
            string startTimeStr = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                                 (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
            o << s.name << " " << s.category << " " << startTimeStr << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
        }
        o.close();

        cout << "Show updated successfully." << endl;
    } else {
        cout << "Show not found." << endl;
    }
}

void editChannel(const string& name, string newName, string newOriginCountry) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid channel details." << endl;
        return;
    }

    string encName = encode(name);
    auto it = ranges::find_if(channels, [&encName](const channel& c) { return c.name == encName; });
    if (it != channels.end()) {
        cout << "Editing channel: " << decode(it->name) << endl;

        if (newName.empty() || newOriginCountry.empty()) {
            cout << "Enter new details (leave blank to keep current value):" << endl;
        }

        if (newName.empty()) {
            cout << "Name: ";
            getline(cin, newName);
            newName = encode(newName);  // Fix: encode newName, not name
            if (ranges::any_of(channels.begin(), channels.end(), [&](const channel& c) {
                return c.name == newName && c.name != it->name; })) {
                cout << "Channel with this name already exists." << endl;
                return;
            }
            if (!newName.empty()) {
                it->name = move(newName);
            }
        } else {
            newName = encode(newName);
            if (ranges::any_of(channels, [&newName, &it](const channel& c) { 
                return c.name == newName && c.name != it->name; })) {
                cout << "Channel with this name already exists." << endl;
                return;
            }
            it->name = move(newName);
        }

        if (newOriginCountry.empty()) {
            cout << "Origin Country: ";
            getline(cin, newOriginCountry);
            if (!newOriginCountry.empty()) {
                newOriginCountry = encode(newOriginCountry);
                it->originCountry = move(newOriginCountry);
            }
        } else {
            newOriginCountry = encode(newOriginCountry);
            it->originCountry = move(newOriginCountry);
        }

        if (!fileExists("Channel.txt")) {
            cout << "File not found. Cannot update." << endl;
            return;
        }
        ofstream o("Channel.txt");
        for (const auto& c : channels) {
            o << c.code << " " << c.name << " " << c.originCountry << endl;
        }
        o.close();

        cout << "Channel updated successfully." << endl;
    } else {
        cout << "Channel not found." << endl;
    }
}

void broadcastSummary() {
    if (channels.empty() || programs.empty()) {
        cout << "No channels or shows available." << endl;
        return;
    }

    if (!fileExists("BroadcastSummary.txt")) {
        createFileIfNotExists("BroadcastSummary.txt");
    }
    ofstream o("BroadcastSummary.txt");

    // Map to store channel names and show counts
    map<string, int> channelCounts;

    // Count shows for each channel
    for (const auto& show : programs) {
        // Find the channel name for this show
        for (const auto& channel : channels) {
            if (channel.code == show.channelCode) {
                channelCounts[channel.name]++;
                break;
            }
        }
    }

    // Write results to file
    for (const auto& [channelName, count] : channelCounts) {
        o << channelName << " " << count << endl;
    }

    o.close();
    cout << "Broadcast summary has been written to BroadcastSummary.txt" << endl;
}

void specificDayShow(const string& day) {
    vector<show> sortedShows;

    // Convert input day to lowercase
    string dayLower = day;
    ranges::transform(dayLower, dayLower.begin(), ::tolower);

    // Case-insensitive day matching
    for (auto &s : programs) {
        string programDayLower = s.dayOfWeek;
        ranges::transform(programDayLower, programDayLower.begin(), ::tolower);

        if (programDayLower == dayLower) {
            sortedShows.push_back(s);
        }
    }

    if (sortedShows.empty()) {
        cout << "No shows found for the specified day." << endl;
        return;
    }

    // Sort shows by start time
    ranges::sort(sortedShows, [](const show& a, const show& b) {
        if (a.startHour != b.startHour) {
            return a.startHour < b.startHour;
        }
        return a.startMinute < b.startMinute;
    });

    // First pass: determine needed column widths based on content
    int nameWidth = 4;      // minimum width for "Name"
    int categoryWidth = 8;  // minimum width for "Category"
    int timeWidth = 10;     // minimum width for "Start Time"
    int durationWidth = 8;  // minimum width for "Duration"
    int channelWidth = 12;  // minimum width for "Channel Code"

    // Determine maximum content width for each column
    for (const auto& s : sortedShows) {
        nameWidth = max(nameWidth, static_cast<int>(decode(s.name).length()));
        categoryWidth = max(categoryWidth, static_cast<int>(decode(s.category).length()));
        channelWidth = max(channelWidth, static_cast<int>(s.channelCode.length()));

        // Calculate duration string length and consider it for column width
        string durationStr = to_string(s.duration) + " min";
        durationWidth = max(durationWidth, static_cast<int>(durationStr.length()));

        // Calculate time string length
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                          (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        timeWidth = max(timeWidth, static_cast<int>(startTime.length()));
    }

    // Add padding (1 space on each side)
    nameWidth += 2;
    categoryWidth += 2;
    timeWidth += 2;
    durationWidth += 2;
    channelWidth += 2;

    // Print header
    int totalWidth = nameWidth + categoryWidth + timeWidth + durationWidth + channelWidth + 6; // 6 for the separators
    cout << endl << "Shows on " << day << ":" << endl;
    cout << string(totalWidth, '-') << endl;
    cout << "|" << setw(nameWidth) << left << " Name"
         << "|" << setw(categoryWidth) << " Category"
         << "|" << setw(timeWidth) << " Start Time"
         << "|" << setw(durationWidth) << " Duration"
         << "|" << setw(channelWidth) << " Channel Code" << "|" << endl;
    cout << string(totalWidth, '-') << endl;

    // Print data rows
    for (const auto& s : sortedShows) {
        string name = decode(s.name);
        string category = decode(s.category);
        string channelCode = s.channelCode;
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                         (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        string durationStr = to_string(s.duration) + " min";

        cout << "|" << setw(nameWidth) << " " + name
             << "|" << setw(categoryWidth) << " " + category
             << "|" << setw(timeWidth) << " " + startTime
             << "|" << setw(durationWidth) << " " + durationStr
             << "|" << setw(channelWidth) << " " + channelCode << "|" << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << sortedShows.size() << " shows found." << endl;
}

void maxShow() {
    if (programs.empty()) {
        cout << "No shows available." << endl;
        return;
    }

    int maxDuration = 0;
    vector<show> longestShows;

    // First, find the maximum duration
    for (const auto& s : programs) {
        if (s.duration > maxDuration) {
            maxDuration = s.duration;
        }
    }

    // Then collect all shows with that duration
    for (const auto& s : programs) {
        if (s.duration == maxDuration) {
            longestShows.push_back(s);
        }
    }

    // First pass: determine needed column widths based on content
    int nameWidth = 4;      // minimum width for "Name"
    int categoryWidth = 8;  // minimum width for "Category"
    int timeWidth = 10;     // minimum width for "Start Time"
    int durationWidth = 8;  // minimum width for "Duration"
    int dayWidth = 3;       // minimum width for "Day"
    int channelWidth = 12;  // minimum width for "Channel Code"

    // Determine maximum content width for each column
    for (const auto& s : longestShows) {
        nameWidth = max(nameWidth, static_cast<int>(decode(s.name).length()));
        categoryWidth = max(categoryWidth, static_cast<int>(decode(s.category).length()));
        dayWidth = max(dayWidth, static_cast<int>(decode(s.dayOfWeek).length()));
        channelWidth = max(channelWidth, static_cast<int>(s.channelCode.length()));

        // Calculate duration string length and consider it for column width
        string durationStr = to_string(s.duration) + " min";
        durationWidth = max(durationWidth, static_cast<int>(durationStr.length()));

        // Calculate time string length
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                          (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        timeWidth = max(timeWidth, static_cast<int>(startTime.length()));
    }

    // Add padding (1 space on each side)
    nameWidth += 2;
    categoryWidth += 2;
    timeWidth += 2;
    durationWidth += 2;
    dayWidth += 2;
    channelWidth += 2;

    // Print header
    int totalWidth = nameWidth + categoryWidth + timeWidth + durationWidth + dayWidth + channelWidth + 7; // 7 for the separators
    cout << endl << "Shows with the longest duration (" << maxDuration << " minutes):" << endl;
    cout << string(totalWidth, '-') << endl;
    cout << "|" << setw(nameWidth) << left << " Name"
         << "|" << setw(categoryWidth) << " Category"
         << "|" << setw(timeWidth) << " Start Time"
         << "|" << setw(durationWidth) << " Duration"
         << "|" << setw(dayWidth) << " Day"
         << "|" << setw(channelWidth) << " Channel Code" << "|" << endl;
    cout << string(totalWidth, '-') << endl;

    // Print data rows
    for (const auto& s : longestShows) {
        string name = decode(s.name);
        string category = decode(s.category);
        string day = decode(s.dayOfWeek);
        string channelCode = s.channelCode;
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                         (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        string durationStr = to_string(s.duration) + " min";

        cout << "|" << setw(nameWidth) << " " + name
             << "|" << setw(categoryWidth) << " " + category
             << "|" << setw(timeWidth) << " " + startTime
             << "|" << setw(durationWidth) << " " + durationStr
             << "|" << setw(dayWidth) << " " + day
             << "|" << setw(channelWidth) << " " + channelCode << "|" << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << longestShows.size() << " shows found." << endl;
}

void minShow() {
    if (programs.empty()) {
        cout << "No shows available." << endl;
        return;
    }

    int minDuration = INT_MAX;
    vector<show> shortestShows;

    // First, find the minimum duration
    for (const auto& s : programs) {
        if (s.duration < minDuration) {
            minDuration = s.duration;
        }
    }

    // Then collect all shows with that duration
    for (const auto& s : programs) {
        if (s.duration == minDuration) {
            shortestShows.push_back(s);
        }
    }

    // First pass: determine needed column widths based on content
    int nameWidth = 4;      // minimum width for "Name"
    int categoryWidth = 8;  // minimum width for "Category"
    int timeWidth = 10;     // minimum width for "Start Time"
    int durationWidth = 8;  // minimum width for "Duration"
    int dayWidth = 3;       // minimum width for "Day"
    int channelWidth = 12;  // minimum width for "Channel Code"

    // Determine maximum content width for each column
    for (const auto& s : shortestShows) {
        nameWidth = max(nameWidth, static_cast<int>(decode(s.name).length()));
        categoryWidth = max(categoryWidth, static_cast<int>(decode(s.category).length()));
        dayWidth = max(dayWidth, static_cast<int>(decode(s.dayOfWeek).length()));
        channelWidth = max(channelWidth, static_cast<int>(s.channelCode.length()));

        // Calculate duration string length and consider it for column width
        string durationStr = to_string(s.duration) + " min";
        durationWidth = max(durationWidth, static_cast<int>(durationStr.length()));

        // Calculate time string length
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                          (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        timeWidth = max(timeWidth, static_cast<int>(startTime.length()));
    }

    // Add padding (1 space on each side)
    nameWidth += 2;
    categoryWidth += 2;
    timeWidth += 2;
    durationWidth += 2;
    dayWidth += 2;
    channelWidth += 2;

    // Print header
    int totalWidth = nameWidth + categoryWidth + timeWidth + durationWidth + dayWidth + channelWidth + 7; // 7 for the separators
    cout << endl << "Shows with the shortest duration (" << minDuration << " minutes):" << endl;
    cout << string(totalWidth, '-') << endl;
    cout << "|" << setw(nameWidth) << left << " Name"
         << "|" << setw(categoryWidth) << " Category"
         << "|" << setw(timeWidth) << " Start Time"
         << "|" << setw(durationWidth) << " Duration"
         << "|" << setw(dayWidth) << " Day"
         << "|" << setw(channelWidth) << " Channel Code" << "|" << endl;
    cout << string(totalWidth, '-') << endl;

    // Print data rows
    for (const auto& s : shortestShows) {
        string name = decode(s.name);
        string category = decode(s.category);
        string day = decode(s.dayOfWeek);
        string channelCode = s.channelCode;
        string startTime = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                         (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        string durationStr = to_string(s.duration) + " min";

        cout << "|" << setw(nameWidth) << " " + name
             << "|" << setw(categoryWidth) << " " + category
             << "|" << setw(timeWidth) << " " + startTime
             << "|" << setw(durationWidth) << " " + durationStr
             << "|" << setw(dayWidth) << " " + day
             << "|" << setw(channelWidth) << " " + channelCode << "|" << endl;
    }

    cout << string(totalWidth, '-') << endl;
    cout << shortestShows.size() << " shows found." << endl;
}

void averageShow(const string& category) {
    int sum = 0, count = 0;
    for (auto& s : programs) {
        if (s.category == encode(category)) {
            sum += s.duration;
            count++;
        }
    }
    if (count == 0) {
        cout << "No shows available in the " << category << " category." << endl;
    } else {
        double average = static_cast<double>(sum) / count;
        cout << "Average duration of shows in category "  << category << ": " << average << " minutes." << endl;
    }
}

void showMenu() {
    int choice = 0;
    string name, category, dayOfWeek, channelCode, originCountry;
    int duration;

    do {
        cout << "\n===== TV Program Management System =====" << endl;
        cout << "1. Show all shows" << endl;
        cout << "2. Show all channels" << endl;
        cout << "3. Add show" << endl;
        cout << "4. Add channel" << endl;
        cout << "5. Delete show" << endl;
        cout << "6. Delete channel" << endl;
        cout << "7. Edit show" << endl;
        cout << "8. Edit channel" << endl;
        cout << "9. Make a Broadcast Summary" << endl;
        cout << "10. Show shows on a specific day" << endl;
        cout << "11. Show longest show" << endl;
        cout << "12. Show shortest show" << endl;
        cout << "13. Average show" << endl;
        cout << "14. Exit" << endl;
        cout << "Enter your choice: ";

        string input;
        getline(cin, input);

        // Try to convert the input to an integer
        try {
            choice = stoi(input);
        } catch (const exception&) {
            cout << "Invalid input. Please enter a number." << endl;
            cout << "\nPress Enter to continue...";
            cin.get();
            clearScreen();
            continue; // Skip the rest of the loop iteration
        }

        switch (choice) {
            case 1:
                clearScreen();
                allShows();
                break;
            case 2:
                clearScreen();
                allChannels();
                break;
            case 3: {
                clearScreen();
                string startTime;
                cout << "Enter show name: ";
                getline(cin, name);
                cout << "Enter category: ";
                getline(cin, category);
                cout << "Enter start time (HH:MM): ";
                getline(cin, startTime);
                cout << "Enter duration (minutes): ";
                getline(cin, input);
                try {
                    duration = stoi(input);
                } catch (const exception&) {
                    cout << "Invalid duration. Operation cancelled." << endl;
                    cout << "\nPress Enter to continue...";
                    cin.get();
                    clearScreen();
                    continue;
                }
                cout << "Enter day of week: ";
                getline(cin, dayOfWeek);
                cout << "Enter channel code: ";
                getline(cin, channelCode);
                addShow(name, category, startTime, duration, dayOfWeek, move(channelCode));
                break;
            }
            case 4:
                clearScreen();
                cout << "Enter channel name: ";
                getline(cin, name);
                cout << "Enter origin country: ";
                getline(cin, originCountry);
                addChannel(name, originCountry);
                break;
            case 5:
                clearScreen();
                cout << "Enter name of show to delete: ";
                getline(cin, name);
                deleteShow(name);
                break;
            case 6:
                clearScreen();
                cout << "Enter name of channel to delete: ";
                getline(cin, name);
                deleteChannel(name);
                break;
            case 7:
                clearScreen();
                cout << "Enter name of show to edit: ";
                getline(cin, name);
                editShow(name);
                break;
            case 8:
                clearScreen();
                cout << "Enter name of channel to edit: ";
                getline(cin, name);
                editChannel(name);
                break;
            case 9:
                clearScreen();
                broadcastSummary();
                break;
            case 10:
                clearScreen();
                cout << "Enter day of week: ";
                getline(cin, dayOfWeek);
                specificDayShow(dayOfWeek);
                break;
            case 11:
                clearScreen();
                maxShow();
                break;
            case 12:
                clearScreen();
                minShow();
                break;
            case 13:
                clearScreen();
                cout << "Enter category name: ";
                getline(cin, category);
                averageShow(category);
                break;
            case 14:
                clearScreen();
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
        
        if (choice != 14) {
            cout << "\nPress Enter to continue...";
            cin.get();
            clearScreen();
        }
    } while (choice != 14);
}

