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
    string startTime;
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
        cout << "Start Time: " << s.startTime << endl;
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
    if (name.empty() || category.empty() || startTime.empty() || duration <= 0 || dayOfWeek.empty() || channelCode.empty() ) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    if (any_of(programs.begin(), programs.end(), [&name](const show& s) { return s.name == name; })) {
        cout << "Show with this name already exists." << endl;
        return;
    }

    show s;
    s.name = move(name);
    s.category =  move(category);
    s.startTime =  move(startTime);
    s.duration = duration;
    s.dayOfWeek =  move(dayOfWeek);
    s.channelCode =  move(channelCode);
    programs.push_back(s);

    ofstream o("../Program.txt", ios::app);
    o << endl << s.name << " " << s.category << " " << s.startTime << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode;
    o.close();
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
        o << s.name << " " << s.category << " " << s.startTime << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
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
            if (any_of(programs.begin(), programs.end(), [&newName](const show& s) { return s.name == newName; })) {
                cout << "Show with this name already exists." << endl;
                return;
            }
            if (!newName.empty()) {
                it->name = move(newName);
            }
        } else {
            if (any_of(programs.begin(), programs.end(), [&newName](const show& s) { return s.name == newName; })) {
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
            cout << "Start Time: ";
            getline(cin, newStartTime);
            if (!newStartTime.empty()) {
                it->startTime = move(newStartTime);
            }
        } else {
            it->startTime = move(newStartTime);
        }
        if (!newDuration) {
            cout << "Duration: ";
            cin >> newDuration;
            cin.ignore();
            if (newDuration < 0) {
                cout << "Invalid duration. Please provide a valid duration." << endl;
                return;
            }

            if (newDuration != 0) {
                it->duration = newDuration;
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
                it->channelCode = move(newChannelCode);
            }
        } else {
            it->channelCode = move(newChannelCode);
        }

        ofstream o("../Program.txt");
        for (const auto& s : programs) {
            o << s.name << " " << s.category << " " << s.startTime << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
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

void showMenu() {
    int choice;
    string name, category, startTime, dayOfWeek, channelCode, code, originCountry;
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
        cout << "9. Exit" << endl;
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
            case 3:
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
                addShow(name, category, startTime, duration, dayOfWeek, channelCode);
                break;
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
                cout << "Exiting program. Goodbye!" << endl;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
                break;
        }
    } while (choice != 9);
}

int main() {
    ifstream  p("../Program.txt");
    string line;
    while (getline( p, line)) {
        show s;
        stringstream ss(line);
        ss >> s.name >> s.category >> s.startTime >> s.duration >> s.dayOfWeek >> s.channelCode;
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