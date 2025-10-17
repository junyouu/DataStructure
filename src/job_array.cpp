#include "job_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ---------------- Constructor / Destructor ----------------
JobArray::JobArray() {
    jobs = nullptr;
    jobsCount = 0;
    jobsCapacity = 0;
    csvFilename = "";
}

JobArray::~JobArray() {
    delete[] jobs;
}

// ---------------- ensureCapacity ----------------
void JobArray::ensureCapacity(int minCapacity) {
    if (minCapacity <= jobsCapacity) return;

    int newCapacity = max(100, jobsCapacity * 2);
    if (newCapacity < minCapacity) newCapacity = minCapacity;

    Job *newJobs = new Job[newCapacity];
    for (int i = 0; i < jobsCount; ++i)
        newJobs[i] = jobs[i];

    delete[] jobs;
    jobs = newJobs;
    jobsCapacity = newCapacity;
}

// ---------------- extractInfo ----------------
static void extractInfo(Job &job) {
    string desc = job.description;
    transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

    size_t phrasePos = desc.find("needed with experience");
    if (phrasePos == string::npos) {
        stringstream ss(job.description);
        ss >> job.title;
        return;
    }

    job.title = job.description.substr(0, phrasePos);
    while (!job.title.empty() && isspace(job.title.back()))
        job.title.pop_back();

    size_t startPos = phrasePos + string("needed with experience").length();
    size_t endPos = job.description.find('.', startPos);
    if (endPos == string::npos)
        endPos = job.description.length();

    string skills = job.description.substr(startPos, endPos - startPos);
    if (skills.find(" in ") == 0) skills.erase(0, 4);
    skills.erase(0, skills.find_first_not_of(" "));
    skills.erase(skills.find_last_not_of(" ") + 1);

    stringstream ss(skills);
    string token;
    int idx = 0;
    while (getline(ss, token, ',') && idx < 10) {
        token.erase(0, token.find_first_not_of(" "));
        token.erase(token.find_last_not_of(" ") + 1);
        if (!token.empty()) job.keywords[idx++] = token;
    }
    for (int i = idx; i < 10; ++i) job.keywords[i] = "";
}

// ---------------- insertAtEnd ----------------
void JobArray::insertAtEnd(const string &desc) {
    ensureCapacity(jobsCount + 1);
    Job &job = jobs[jobsCount++];
    job.jobID = jobsCount;
    job.description = desc;
    extractInfo(job);
}

// ---------------- loadFromCSV ----------------
void JobArray::loadFromCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    csvFilename = filename;
    string line;
    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        insertAtEnd(line);
    }

    file.close();

    auto end = high_resolution_clock::now();
    cout << "[Performance] Load execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ---------------- saveToCSV ----------------
void JobArray::saveToCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << " for writing\n";
        return;
    }

    file << "job_description\n";
    for (int i = 0; i < jobsCount; ++i)
        file << "\"" << jobs[i].description << "\"\n";

    file.close();
    cout << "Successfully saved " << jobsCount << " records to " << filename << endl;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Save execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ---------------- display ----------------
void JobArray::display() const {
    auto start = high_resolution_clock::now();

    cout << "\n=== Job List ===\n";
    for (int i = 0; i < jobsCount; ++i) {
        const Job &job = jobs[i];
        cout << "ID: " << job.jobID << "\n";
        cout << "Title: " << job.title << "\n";
        cout << "Keywords: ";
        bool printed = false;
        for (int k = 0; k < 10; ++k) {
            if (!job.keywords[k].empty()) {
                if (printed) cout << ", ";
                cout << job.keywords[k];
                printed = true;
            }
        }
        if (!printed) cout << "(none)";
        cout << "\nDescription: " << job.description << "\n\n";
    }
    if (jobsCount == 0) cout << "(No jobs loaded)\n";

    auto end = high_resolution_clock::now();
    cout << "[Performance] Display execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ---------------- addRecord / delete operations ----------------
bool JobArray::confirmAction(const string &message) {
    cout << "\n" << message << endl;
    cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response;
    cin >> response;
    cin.ignore();
    return (response == 'y' || response == 'Y');
}

void JobArray::addRecord() {
    string jobTitle, skills;
    cout << "\n=== ADD NEW JOB ===\n";
    cout << "Enter job title: ";
    getline(cin, jobTitle);
    cout << "Enter required skills (comma-separated): ";
    getline(cin, skills);

    string desc = jobTitle + " needed with experience in " + skills + ".";
    insertAtEnd(desc);

    cout << "New job added successfully.\n";
    if (confirmAction("Save to CSV?")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
        else cout << "Warning: No CSV filename stored.\n";
    }
}

void JobArray::deleteFromHead() {
    if (jobsCount == 0) {
        cout << "Array is empty.\n";
        return;
    }
    for (int i = 1; i < jobsCount; ++i)
        jobs[i - 1] = jobs[i];
    jobsCount--;
    cout << "Deleted first record.\n";
}

void JobArray::deleteFromMiddle(int position) {
    if (position < 1 || position > jobsCount) {
        cout << "Invalid position.\n";
        return;
    }
    for (int i = position; i < jobsCount; ++i)
        jobs[i - 1] = jobs[i];
    jobsCount--;
    cout << "Deleted record at position " << position << ".\n";
}

void JobArray::deleteFromTail() {
    if (jobsCount == 0) {
        cout << "Array is empty.\n";
        return;
    }
    jobsCount--;
    cout << "Deleted last record.\n";
}

void JobArray::clear() {
    delete[] jobs;
    jobs = nullptr;
    jobsCount = 0;
    jobsCapacity = 0;
}

int JobArray::getSize() const {
    return jobsCount;
}

Job* JobArray::findJobByID(int jobID) {
    for (int i = 0; i < jobsCount; ++i)
        if (jobs[i].jobID == jobID)
            return &jobs[i];
    return nullptr;
}

const Job& JobArray::getJob(int index) const {
    return jobs[index];
}
