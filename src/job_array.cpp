#include "job_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include "utility.h"
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
    double memBefore = getCurrentMemoryKB();  // record before loading
    auto start = high_resolution_clock::now();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    csvFilename = filename;
    string line;
    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        insertAtEnd(line);
    }

    file.close();

    auto end = high_resolution_clock::now();
    double memAfter = getCurrentMemoryKB();  // record after loading

    cout << "[Performance] loadFromCSV [Job Array] execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    cout << "[Memory] loadFromCSV [Job Array] memory space: "
         << (memAfter - memBefore) << " KB\n\n";
}


// ---------------- saveToCSV ----------------
void JobArray::saveToCSV(const string &filename) {
    // Note: Performance timer stops BEFORE file writing.
    auto start = high_resolution_clock::now();

    auto end = high_resolution_clock::now();
    cout << "[Performance] (pre-save) processing time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    // File I/O (not included in timing)
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
    cout << "[Performance] display execution time: "
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
    // 1) Collect user input (do NOT time this — it's human interactive)
    string jobTitle, skills;
    cout << "\n=== ADD NEW JOB ===\n";
    cout << "Enter job title: ";
    getline(cin, jobTitle);
    cout << "Enter required skills (comma-separated): ";
    getline(cin, skills);

    // 2) Start timing for internal processing only
    // Build description and insert (this includes the extractInfo step)
    string desc = jobTitle + " needed with experience in " + skills + ".";
    auto start = high_resolution_clock::now();
    insertAtEnd(desc);

    // Stop timing before any file I/O or user confirmation
    auto end = high_resolution_clock::now();
    cout << "[Performance] addRecord (processing) execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    // 3) Ask user whether to save (do not include this interaction in the processing time)
    if (confirmAction("Save to CSV?")) {
        if (!csvFilename.empty()) {
            // saveToCSV() prints its own timing; don't double-count it here
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored.\n";
        }
    }
}


void JobArray::deleteFromMiddle(int position) {
    auto start = high_resolution_clock::now();

    if (position < 1 || position > jobsCount) {
        cout << "Invalid position.\n";
        return;
    }
    for (int i = position; i < jobsCount; ++i)
        jobs[i - 1] = jobs[i];
    jobsCount--;
    cout << "Deleted record at position " << position << ".\n";

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromMiddle execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

void JobArray::deleteFromTail() {
    auto start = high_resolution_clock::now();

    if (jobsCount == 0) {
        cout << "Array is empty.\n";
        return;
    }
    jobsCount--;
    cout << "Deleted last record.\n";

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromTail execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ---------------- Utility ----------------
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

// ---------------- deleteFromHead ----------------
void JobArray::deleteFromHead() {
    if (jobsCount == 0) {
        cout << "Array is empty. Nothing to delete.\n";
        return;
    }

    // Show what will be deleted (don't include this in timing — it's I/O)
    cout << "\n=== DELETING JOB FROM HEAD ===\n";
    cout << "Job ID: " << jobs[0].jobID << endl;
    cout << "Title: " << jobs[0].title << endl;
    cout << "Description: " << jobs[0].description << endl;
    cout << "==============================\n";

    // Start timing for processing (shifting & bookkeeping) only
    auto start = high_resolution_clock::now();

    // Shift everything left by one
    for (int i = 1; i < jobsCount; ++i) {
        jobs[i - 1] = jobs[i];
    }
    jobsCount--;

    // Reassign jobID to keep them sequential (1..jobsCount)
    for (int i = 0; i < jobsCount; ++i) {
        jobs[i].jobID = i + 1;
    }

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromHead (processing) execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    // Ask for confirmation to save (do not include save time above)
    if (confirmAction("Job record has been deleted from the array.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}
