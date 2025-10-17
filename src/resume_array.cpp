#include "resume_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include <algorithm>
#include <cctype>
using namespace std;
using namespace std::chrono;

ResumeArray::ResumeArray() : resumes(nullptr), resumesCount(0), resumesCapacity(0) {}

ResumeArray::~ResumeArray() {
    delete[] resumes;
}

void ResumeArray::ensureCapacity(int minCapacity) {
    if (resumesCapacity >= minCapacity) return;
    int newCap = (resumesCapacity == 0) ? 100 : resumesCapacity * 2;
    while (newCap < minCapacity) newCap *= 2;

    Resume *temp = new Resume[newCap];
    for (int i = 0; i < resumesCount; ++i)
        temp[i] = resumes[i];
    delete[] resumes;
    resumes = temp;
    resumesCapacity = newCap;
}

// ===============================
// Load and Save
// ===============================
void ResumeArray::loadFromCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Cannot open " << filename << endl;
        return;
    }

    csvFilename = filename;
    string line;
    getline(file, line); // skip header

    resumesCount = 0; // reset
    int id = 1;

    while (getline(file, line)) {
        if (line.empty()) continue;

        // Remove enclosing quotes
        if (line.front() == '"' && line.back() == '"')
            line = line.substr(1, line.size() - 2);

        ensureCapacity(resumesCount + 1);
        Resume &r = resumes[resumesCount++];
        r.resumeID = id++;
        r.description = line;

        // --- Keyword Extraction ---
        string desc = r.description;
        transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

        size_t phrasePos = desc.find("skilled in");
        if (phrasePos == string::npos)
            phrasePos = desc.find("experienced in");
        if (phrasePos == string::npos)
            phrasePos = desc.find("proficient in");

        string skills;
        if (phrasePos != string::npos) {
            size_t startPos = phrasePos + (desc.find("in", phrasePos) - phrasePos) + 2;
            size_t endPos = r.description.find('.', startPos);
            if (endPos == string::npos)
                endPos = r.description.length();
            skills = r.description.substr(startPos, endPos - startPos);
        } else {
            stringstream ss(r.description);
            string word;
            int k = 0;
            while (ss >> word && k < 10)
                r.keywords[k++] = word;
            for (int k2 = k; k2 < 10; ++k2)
                r.keywords[k2] = "";
            continue;
        }

        stringstream ss(skills);
        string token;
        int idx = 0;
        while (getline(ss, token, ',') && idx < 10) {
            token.erase(0, token.find_first_not_of(" "));
            token.erase(token.find_last_not_of(" ") + 1);
            if (!token.empty())
                r.keywords[idx++] = token;
        }
        for (int i = idx; i < 10; ++i)
            r.keywords[i] = "";
    }

    file.close();

    auto end = high_resolution_clock::now();
    cout << "[Performance] loadFromCSV [Resume Array] execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

void ResumeArray::saveToCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: cannot open " << filename << " for writing.\n";
        return;
    }

    file << "resume" << endl;
    for (int i = 0; i < resumesCount; ++i)
        file << '"' << resumes[i].description << '"' << endl;

    file.close();
    cout << "Successfully saved " << resumesCount << " record(s) to " << filename << endl;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Save execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ===============================
// Clear
// ===============================
void ResumeArray::clear() {
    delete[] resumes;
    resumes = nullptr;
    resumesCount = 0;
    resumesCapacity = 0;
}

// ===============================
// Display
// ===============================
void ResumeArray::display() const {
    auto start = high_resolution_clock::now();

    cout << "\n=== Resume List ===\n";
    if (resumesCount == 0) {
        cout << "(No resumes loaded)\n";
    } else {
        for (int i = 0; i < resumesCount; ++i) {
            cout << "ID: " << resumes[i].resumeID << "\n";
            cout << "Keywords: ";

            bool printed = false;
            for (int j = 0; j < 10; ++j) {
                if (!resumes[i].keywords[j].empty()) {
                    if (printed) cout << ", ";
                    cout << resumes[i].keywords[j];
                    printed = true;
                }
            }
            if (!printed) cout << "(none)";
            cout << "\nOriginal Text: " << resumes[i].description << "\n\n";
        }
    }

    auto end = high_resolution_clock::now();
    cout << "[Performance] Display execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ===============================
// Add Record
// ===============================
void ResumeArray::addRecord() {
    cout << "\n=== ADD NEW RESUME (Array) ===\n";
    string desc;
    cout << "Enter resume description: ";
    getline(cin, desc);

    auto start = high_resolution_clock::now();
    Resume r;
    r.resumeID = resumesCount + 1;
    r.description = desc;

    stringstream ss(desc);
    string word;
    int k = 0;
    while (ss >> word && k < 10)
        r.keywords[k++] = word;

    ensureCapacity(resumesCount + 1);
    resumes[resumesCount++] = r;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Add execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!csvFilename.empty()) {
        if (confirmAction("Save this change to CSV?"))
            saveToCSV(csvFilename);
        else
            cout << "Change saved in memory only.\n";
    }
}

// ===============================
// Delete Functions
// ===============================
void ResumeArray::deleteFromHead() {
    auto start = high_resolution_clock::now();

    if (resumesCount == 0) {
        cout << "No resumes to delete.\n";
        return;
    }
    for (int i = 1; i < resumesCount; ++i)
        resumes[i - 1] = resumes[i];
    resumesCount--;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Delete-from-head execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!csvFilename.empty() && confirmAction("Save this change to CSV?"))
        saveToCSV(csvFilename);
}

void ResumeArray::deleteFromMiddle(int position) {
    auto start = high_resolution_clock::now();

    if (resumesCount == 0) {
        cout << "No resumes to delete.\n";
        return;
    }
    if (position < 1 || position > resumesCount) {
        cout << "Invalid position.\n";
        return;
    }
    for (int i = position; i < resumesCount; ++i)
        resumes[i - 1] = resumes[i];
    resumesCount--;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Delete-from-middle execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!csvFilename.empty() && confirmAction("Save this change to CSV?"))
        saveToCSV(csvFilename);
}

void ResumeArray::deleteFromTail() {
    auto start = high_resolution_clock::now();

    if (resumesCount == 0) {
        cout << "No resumes to delete.\n";
        return;
    }
    resumesCount--;

    auto end = high_resolution_clock::now();
    cout << "[Performance] Delete-from-tail execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!csvFilename.empty() && confirmAction("Save this change to CSV?"))
        saveToCSV(csvFilename);
}

// ===============================
// Confirm Action
// ===============================
bool ResumeArray::confirmAction(const string &message) {
    cout << message << " (y/n): ";
    char c;
    cin >> c;
    cin.ignore();
    return (c == 'y' || c == 'Y');
}
