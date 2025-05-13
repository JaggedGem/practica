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

void editShow(string name, string newName = "", string newCategory = "", string newStartTime = "", int newDuration = -1, string newDayOfWeek = "", string newChannelCode = "") {
    if (name.empty()) {
        cout << "Invalid input. Please provide valid show details." << endl;
        return;
    }

    auto it = find_if(programs.begin(), programs.end(), [&name](const show& s) { return s.name == name; });
    if (it != programs.end()) {
        cout << "Editing show: " << it->name << endl;
        cout << "Enter new details (leave blank to keep current value):" << endl;

        if (newName == "") {
            cout << "Name: ";
            getline(cin, newName);
            it->name = move(newName);
        } else {
            it->name = move(newName);
        }
        if (newCategory == "") {
            cout << "Category: ";
            getline(cin, newCategory);
            it->category = move(newCategory);
        } else {
            it->category = move(newCategory);
        }
        if (newStartTime == "") {
            cout << "Start Time: ";
            getline(cin, newStartTime);
            it->startTime = move(newStartTime);
        } else {
            it->startTime = move(newStartTime);
        }
        if (newDuration == -1) {
            cout << "Duration: ";
            cin >> newDuration;
            cin.ignore();
            it->duration = newDuration;
        } else {
            it->duration = newDuration;
        }
        if (newDayOfWeek == "") {
            cout << "Day of Week: ";
            getline(cin, newDayOfWeek);
            it->dayOfWeek = move(newDayOfWeek);
        } else {
            it->dayOfWeek = move(newDayOfWeek);
        }
        if (newChannelCode == "") {
            cout << "Channel Code: ";
            getline(cin, newChannelCode);
            it->channelCode = move(newChannelCode);
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

    allShows();
    cout << "------------------------" << endl << endl;
    allChannels();

    addShow("testName", "testCategory", "testTime", 120, "Monday", "testCode");
    cout << "------------------------" << endl << endl;

    addChannel("0009", "testChannel", "testCountry");
    cout << "------------------------" << endl << endl;

    // deleteShow("testName");
    cout << "------------------------" << endl << endl;
    deleteChannel("testChannel");

    editShow("testName", "newName", "newCategory", "newTime", -1, "newDay", "newCode");

    p.close();
    return 0;
}