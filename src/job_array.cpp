#include "job_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
using namespace std;

// trim whitespace and double quotes from both ends
static string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\r\n\"");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n\"");
    return s.substr(start, end - start + 1);
}

// safe stod wrapper - returns 0.0 on failure and prints a warning
static double safeStod(const string &s) {
    string t = trim(s);
    if (t.empty()) return 0.0;
    try {
        return stod(t);
    } catch (const std::exception &e) {
        cerr << "[Warning] safeStod: invalid numeric value \"" << s << "\". Using 0.0\n";
        return 0.0;
    }
}

// very small CSV parser that respects quoted fields (handles commas inside quotes)
static vector<string> parseCSVLine(const string &line) {
    vector<string> fields;
    string field;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            // handle escaped quotes ("")
            if (inQuotes && i + 1 < line.size() && line[i+1] == '"') {
                field.push_back('"');
                ++i; // skip the escaped quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.push_back(field);
            field.clear();
        } else {
            field.push_back(c);
        }
    }
    fields.push_back(field);
    return fields;
}

// 1. Load CSV
void JobArray::loadJobs(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    // read header (if present). If header not present, it's fine — we just skip the first line.
    if (!getline(file, line)) {
        cerr << "Empty file: " << filename << endl;
        return;
    }

    while (getline(file, line)) {
        if (line.empty()) continue;

        vector<string> fields = parseCSVLine(line);

        // Fields expected: id, title, description, location, salary (but CSV may vary)
        // We'll map them defensively by index if present.
        Job job;
        job.id = (fields.size() > 0) ? trim(fields[0]) : "";
        job.title = (fields.size() > 1) ? trim(fields[1]) : "";
        job.description = (fields.size() > 2) ? trim(fields[2]) : "";
        job.location = (fields.size() > 3) ? trim(fields[3]) : "";

        // If salary column exists, parse safely; otherwise default 0.0
        if (fields.size() > 4) {
            job.salary = safeStod(fields[4]);
        } else {
            job.salary = 0.0;
        }

        jobs.push_back(job);
    }
    file.close();
}

// 2. Match with other jobs (dummy test for performance)
void JobArray::matchJobs(const vector<Job>& otherJobs) {
    int matchCount = 0;
    for (const auto& j1 : jobs) {
        for (const auto& j2 : otherJobs) {
            if (j1.title == j2.title)
                matchCount++;
        }
    }
    cout << "Total matches: " << matchCount << endl;
}

// 3. Input job index, match 3 resumes (simulate search)
void JobArray::matchThreeResumes(int jobIndex) {
    if (jobIndex < 0 || jobIndex >= (int)jobs.size()) {
        cout << "Invalid index.\n";
        return;
    }
    cout << "Matching 3 resumes for job: " << jobs[jobIndex].title << endl;
    // In practice: you’d call ResumeArray’s matching function here
}

// 4. Add job
void JobArray::addJob(const Job& newJob) {
    jobs.push_back(newJob);
    cout << "Added job: " << newJob.title << endl;
}

// 5. Delete job
void JobArray::deleteJob(int position) {
    if (jobs.empty()) {
        cout << "No jobs to delete.\n";
        return;
    }
    if (position < 0 || position >= (int)jobs.size()) {
        cout << "Invalid position.\n";
        return;
    }
    jobs.erase(jobs.begin() + position);
    cout << "Deleted job at position " << position << endl;
}

void JobArray::printJobs(int count) const {
    for (int i = 0; i < count && i < (int)jobs.size(); ++i) {
        cout << i + 1 << ". " << jobs[i].title << " (" << jobs[i].location << ")\n";
    }
}