#include <iostream>
#include <string>
#include <fstream>
#include <utility>
#include <vector>
#include <sstream>

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
    string type;
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
        cout << "Type: " << c.type << endl;
        cout << endl;
    }
}

void addShow(string name, string category, string startTime, int duration, string dayOfWeek, string channelCode) {
    show s;
    s.name = move(name);
    s.category =  move(category);
    s.startTime =  move(startTime);
    s.duration = duration;
    s.dayOfWeek =  move(dayOfWeek);
    s.channelCode =  move(channelCode);
    programs.push_back(s);

    ofstream o("Program.txt");
    o << s.name << " " << s.category << " " << s.startTime << " " << s.duration << " " << s.dayOfWeek << " " << s.channelCode << endl;
    o.close();
}

int main() {
    ifstream  p("Program.txt");
    string line;
    while (getline( p, line)) {
        show s;
        stringstream ss(line);
        ss >> s.name >> s.category >> s.startTime >> s.duration >> s.dayOfWeek >> s.channelCode;
        programs.push_back(s);
    }

    ifstream ch("Channel.txt");
    while (getline(ch, line)) {
        channel c;
        stringstream ss(line);
        ss >> c.code >> c.name >> c.type;
        channels.push_back(c);
    }

    allShows();
    cout << "------------------------" << endl << endl;
    allChannels();

    addShow("testName", "testCategory", "testTime", 120, "Monday", "testCode");
    cout << endl;
    allShows();


    p.close();
    return 0;
}