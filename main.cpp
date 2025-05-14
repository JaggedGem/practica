#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <sstream>
#include <filesystem>
#include <algorithm>

using namespace std;

struct show {
    string name;
    string category;
    int startHour;
    int startMinute;
    int duration; // in minutes
    string dayOfWeek;
    string channelCode;
};

struct channel {
    string code;
    string name;
    string originCountry;
};

vector<show> programs;
vector<channel> channels;

void allShows() {
    for (auto &s : programs) {
        cout << "Name: " << s.name << endl;
        cout << "Category: " << s.category << endl;
        cout << "Start Time: " << (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) << ":" << (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute)) << endl;
        cout << "Duration: " << s.duration << endl;
        cout << "Day of Week: " << s.dayOfWeek << endl;
        cout << "Channel Code: " << s.channelCode << endl;
        cout << endl;
    }
}

void allChannels() {
    for (auto &c : channels) {
        cout << "Code: " << c.code << endl;
        cout << "Name: " << c.name << endl;
        cout << "Country of Origin: " << c.originCountry << endl;
        cout << endl;
    }
}

void addShow(string name, string category, string startTime, int duration, string dayOfWeek, string channelCode) {
    if (name.empty() || category.empty() || startTime.empty() || duration <= 0 || dayOfWeek.empty() || channelCode.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    if (any_of(programs.begin(), programs.end(), [&name](const show& s) { return s.name == name; })) {
        cout << "Show with this name already exists." << endl;
        return;
    }

    // Check if channel code exists
    bool channelExists = any_of(channels.begin(), channels.end(),
                               [&channelCode](const channel& c) { return c.code == channelCode; });
    if (!channelExists) {
        cout << "Error: Channel code does not exist. Please enter a valid channel code." << endl;
        return;
    }

    // Parse and validate start time
    int startHour = 0, startMinute = 0;
    try {
        // Find the position of the colon
        size_t colonPos = startTime.find(':');
        if (colonPos != string::npos) {
            // Extract hours and minutes
            string hourStr = startTime.substr(0, colonPos);
            string minStr = startTime.substr(colonPos + 1);

            // Convert to integers
            startHour = stoi(hourStr);
            startMinute = stoi(minStr);

            // Validate the time
            if (startHour < 0 || startHour > 24 || startMinute < 0 || startMinute > 59) {
                cout << "Invalid time values. Hours must be 0-24, minutes 0-59." << endl;
                return;
            }
        } else {
            cout << "Invalid time format. Use HH:MM format." << endl;
            return;
        }
    } catch (const exception& e) {
        cout << "Error parsing time: " << e.what() << endl;
        return;
    }

    show s;
    s.name = move(name);
    s.category = move(category);
    s.startHour = startHour;
    s.startMinute = startMinute;
    s.duration = duration;
    s.dayOfWeek = move(dayOfWeek);
    s.channelCode = move(channelCode);
    programs.push_back(s);

    ofstream o("../Program.txt", ios::app);
    o << endl << s.name << " " << s.category << " "
      << (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) << ":"
      << (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute))
      << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode;
    o.close();

    cout << "Show added successfully." << endl;
}

void addChannel(string code, string name, string originCountry) {
    if (code.empty() || name.empty() || originCountry.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    if (any_of(channels.begin(), channels.end(), [&code](const channel& c) { return c.code == code; }) || any_of(channels.begin(), channels.end(), [&name](const channel& c) { return c.name == name; })) {
        cout << "Channel with this code or name already exists." << endl;
        return;
    }

    channel c;
    c.code = move(code);
    c.name = move(name);
    c.originCountry = move(originCountry);
    channels.push_back(c);

    ofstream o("../Channel.txt", ios::app);
    o << endl << c.code << " " << c.name << " " << c.originCountry;
    o.close();
}

void deleteShow(string name) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }
    auto it = remove_if(programs.begin(), programs.end(), [&name](const show& s) { return s.name == name; });
    if (it != programs.end()) {
        programs.erase(it, programs.end());
        cout << "Show deleted successfully." << endl;
    } else {
        cout << "Show not found." << endl;
    }
    ofstream o("../Program.txt");
    for (const auto& s : programs) {
        string startTimeStr = (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) + ":" +
                              (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute));
        o << s.name << " " << s.category << " " << startTimeStr << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
    }
}

void deleteChannel(string name) {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }
    auto it = remove_if(channels.begin(), channels.end(), [&name](const channel& c) { return c.name == name; });
    if (it != channels.end()) {
        channels.erase(it, channels.end());
        cout << "Channel deleted successfully." << endl;
    } else {
        cout << "Channel not found." << endl;
    }
    ofstream o("../Channel.txt");
    for (const auto& c : channels) {
        o << c.code << " " << c.name << " " << c.originCountry << endl;
    }
}

void editShow(string name, string newName = "", string newCategory = "", string newStartTime = "", int newDuration = 0, string newDayOfWeek = "", string newChannelCode = "") {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    auto it = find_if(programs.begin(), programs.end(), [&name](const show& s) { return s.name == name; });
    if (it != programs.end()) {
        cout << "Editing show: " << it->name << endl;

        if (newName.empty() || newCategory.empty() || newStartTime.empty() ||
            newDuration == 0 || newDayOfWeek.empty() || newChannelCode.empty()) {
            cout << "Enter new details (leave blank to keep current value):" << endl;
        }

        if (newName.empty()) {
            cout << "Name: ";
            getline(cin, newName);
            if (!newName.empty() && any_of(programs.begin(), programs.end(), [&](const show& s) {
                return s.name == newName && s.name != name; })) {
                cout << "Show with this name already exists." << endl;
                return;
            }
            if (!newName.empty()) {
                it->name = move(newName);
            }
        } else {
            if (any_of(programs.begin(), programs.end(), [&](const show& s) {
                return s.name == newName && s.name != name; })) {
                cout << "Show with this name already exists." << endl;
                return;
            }
            it->name = move(newName);
        }

        if (newCategory.empty()) {
            cout << "Category: ";
            getline(cin, newCategory);
            if (!newCategory.empty()) {
                it->category = move(newCategory);
            }
        } else {
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
                    if (hour >= 0 && hour <= 24 && minute >= 0 && minute <= 59) {
                        it->startHour = hour;
                        it->startMinute = minute;
                    } else {
                        cout << "Invalid time values. Hours must be 0-24, minutes 0-59." << endl;
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
                bool channelExists = any_of(channels.begin(), channels.end(),
                                          [&newChannelCode](const channel& c) { return c.code == newChannelCode; });
                if (!channelExists) {
                    cout << "Error: Channel code does not exist. Channel not updated." << endl;
                    return;
                }
                it->channelCode = move(newChannelCode);
            }
        } else {
            // Check if channel code exists
            bool channelExists = any_of(channels.begin(), channels.end(),
                                      [&newChannelCode](const channel& c) { return c.code == newChannelCode; });
            if (!channelExists) {
                cout << "Error: Channel code does not exist. Channel not updated." << endl;
                return;
            }
            it->channelCode = move(newChannelCode);
        }

        ofstream o("../Program.txt");
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

void editChannel(string name, string newCode = "", string newName = "", string newOriginCountry = "") {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    auto it = find_if(channels.begin(), channels.end(), [&name](const channel& c) { return c.name == name; });
    if (it != channels.end()) {
        cout << "Editing channel: " << it->name << endl;

        if (newCode.empty() || newName.empty() || newOriginCountry.empty()) {
            cout << "Enter new details (leave blank to keep current value):" << endl;
        }

        if (newCode.empty()) {
            cout << "Code: ";
            getline(cin, newCode);
            if (any_of(channels.begin(), channels.end(), [&newCode](const channel& c) { return c.code == newCode; })) {
                cout << "Channel with this code already exists." << endl;
                return;
            }
            if (!newCode.empty()) {
                it->code = move(newCode);
            }
        } else {
            if (any_of(channels.begin(), channels.end(), [&newCode](const channel& c) { return c.code == newCode; })) {
                cout << "Channel with this code already exists." << endl;
                return;
            }
            it->code = move(newCode);
        }

        if (newName.empty()) {
            cout << "Name: ";
            getline(cin, newName);
            if (any_of(channels.begin(), channels.end(), [&newName](const channel& c) { return c.name == newName; })) {
                cout << "Channel with this name already exists." << endl;
                return;
            }
            if (!newName.empty()) {
                it->name = move(newName);
            }
        } else {
            if (any_of(channels.begin(), channels.end(), [&newName](const channel& c) { return c.name == newName; })) {
                cout << "Channel with this name already exists." << endl;
                return;
            }
            it->name = move(newName);
        }

        if (newOriginCountry.empty()) {
            cout << "Origin Country: ";
            getline(cin, newOriginCountry);
            if (!newOriginCountry.empty()) {
                it->originCountry = move(newOriginCountry);
            }
        } else {
            it->originCountry = move(newOriginCountry);
        }

        ofstream o("../Channel.txt");
        for (const auto& c : channels) {
            o << c.code << " " << c.name << " " << c.originCountry << endl;
        }
        o.close();

        cout << "Channel updated successfully." << endl;
    } else {
        cout << "Channel not found." << endl;
    }
}

void specificDayShow(const string& day) {
    vector<show> sortedShows;

    // Convert input day to lowercase
    string dayLower = day;
    transform(dayLower.begin(), dayLower.end(), dayLower.begin(), ::tolower);

    // Case-insensitive day matching
    for (auto &s : programs) {
        string programDayLower = s.dayOfWeek;
        transform(programDayLower.begin(), programDayLower.end(), programDayLower.begin(), ::tolower);

        if (programDayLower == dayLower) {
            sortedShows.push_back(s);
        }
    }

    if (sortedShows.empty()) {
        cout << "No shows found for the specified day." << endl;
        return;
    }

    cout << endl << "Shows on " << day << ":" << endl;
    sort(sortedShows.begin(), sortedShows.end(), [](const show& a, const show& b) {
        if (a.startHour != b.startHour) {
            return a.startHour < b.startHour;
        }
        return a.startMinute < b.startMinute;
    });

    for (const auto& s : sortedShows) {
        cout << endl;
        cout << "Name: " << s.name << endl;
        cout << "Category: " << s.category << endl;
        cout << "Start Time: " << (s.startHour < 10 ? "0" + to_string(s.startHour) : to_string(s.startHour)) << ":" << (s.startMinute < 10 ? "0" + to_string(s.startMinute) : to_string(s.startMinute)) << endl;
        cout << "Duration: " << s.duration << endl;
        cout << "Channel Code: " << s.channelCode << endl;
    }
}

void maxShow() {
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

    cout << "Shows with the longest duration (" << maxDuration << " minutes):" << endl;
    for (const auto& show : longestShows) {
        cout << "\nName: " << show.name << endl;
        cout << "Category: " << show.category << endl;
        cout << "Start Time: " << (show.startHour < 10 ? "0" + to_string(show.startHour) : to_string(show.startHour))
             << ":" << (show.startMinute < 10 ? "0" + to_string(show.startMinute) : to_string(show.startMinute)) << endl;
        cout << "Duration: " << show.duration << endl;
        cout << "Day of Week: " << show.dayOfWeek << endl;
        cout << "Channel Code: " << show.channelCode << endl;
    }
}

void minShow() {
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

    cout << "Shows with the shortest duration (" << minDuration << " minutes):" << endl;
    for (const auto& show : shortestShows) {
        cout << "\nName: " << show.name << endl;
        cout << "Category: " << show.category << endl;
        cout << "Start Time: " << (show.startHour < 10 ? "0" + to_string(show.startHour) : to_string(show.startHour))
             << ":" << (show.startMinute < 10 ? "0" + to_string(show.startMinute) : to_string(show.startMinute)) << endl;
        cout << "Duration: " << show.duration << endl;
        cout << "Day of Week: " << show.dayOfWeek << endl;
        cout << "Channel Code: " << show.channelCode << endl;
    }
}

void averageShow() {
    int sum = 0, count = 0;
    for (auto& s : programs) {
        sum += s.duration;
        count++;
    }
    if (count == 0) {
        cout << "No shows available." << endl;
    } else {
        double average = static_cast<double>(sum) / count;
        cout << "Average duration of shows: " << average << " minutes." << endl;
    }
}

void showMenu() {
    int choice;
    string name, category, dayOfWeek, channelCode, code, originCountry;
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
        cout << "9. Show shows on a specific day" << endl;
        cout << "10. Show longest show" << endl;
        cout << "11. Show shortest show" << endl;
        cout << "12. Average show" << endl;
        cout << "13. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1:
                allShows();
                break;
            case 2:
                allChannels();
                break;
            case 3: {
                string startTime;
                cout << "Enter show name: ";
                getline(cin, name);
                cout << "Enter category: ";
                getline(cin, category);
                cout << "Enter start time (HH:MM): ";
                getline(cin, startTime);
                cout << "Enter duration (minutes): ";
                cin >> duration;
                cin.ignore();
                cout << "Enter day of week: ";
                getline(cin, dayOfWeek);
                cout << "Enter channel code: ";
                getline(cin, channelCode);
                addShow(move(name), move(category), startTime, duration, move(dayOfWeek), move(channelCode));
                break;
            }
            case 4:
                cout << "Enter channel code: ";
                getline(cin, code);
                cout << "Enter channel name: ";
                getline(cin, name);
                cout << "Enter origin country: ";
                getline(cin, originCountry);
                addChannel(code, name, originCountry);
                break;
            case 5:
                cout << "Enter name of show to delete: ";
                getline(cin, name);
                deleteShow(name);
                break;
            case 6:
                cout << "Enter name of channel to delete: ";
                getline(cin, name);
                deleteChannel(name);
                break;
            case 7:
                cout << "Enter name of show to edit: ";
                getline(cin, name);
                editShow(name);
                break;
            case 8:
                cout << "Enter name of channel to edit: ";
                getline(cin, name);
                editChannel(name);
                break;
            case 9:
                cout << "Enter day of week: ";
                getline(cin, dayOfWeek);
                specificDayShow(dayOfWeek);
                break;
            case 10:
                maxShow();
                break;
            case 11:
                minShow();
                break;
            case 12:
                averageShow();
                break;
            case 13:
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    } while (choice != 13);
}

int main() {
    ifstream  p("../Program.txt");
    string line;
    while (getline( p, line)) {
        show s;
        string startTime;
        stringstream ss(line);
        ss >> s.name >> s.category >> startTime >> s.duration >> s.dayOfWeek >> s.channelCode;
        s.startHour = stoi(startTime.substr(0, 2));
        s.startMinute = stoi(startTime.substr(3, 2));
        programs.push_back(s);
    }

    ifstream ch("../Channel.txt");
    while (getline(ch, line)) {
        channel c;
        stringstream ss(line);
        ss >> c.code >> c.name >> c.originCountry;
        channels.push_back(c);
    }

    showMenu();

    p.close();
    return 0;
}