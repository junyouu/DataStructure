#include "resume_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

// 1. Load CSV
void ResumeArray::loadResumes(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // skip header
    while (getline(file, line)) {
        stringstream ss(line);
        Resume r;
        getline(ss, r.id, ',');
        getline(ss, r.name, ',');
        getline(ss, r.skills, ',');
        getline(ss, r.experience, ',');
        resumes.push_back(r);
    }
    file.close();
}

// 2. Match resumes
void ResumeArray::matchResumes(const vector<Resume>& otherResumes) {
    int count = 0;
    for (const auto& r1 : resumes) {
        for (const auto& r2 : otherResumes) {
            if (r1.skills == r2.skills)
                count++;
        }
    }
    cout << "Matched " << count << " resumes.\n";
}

// 3. Match 3 jobs
void ResumeArray::matchThreeJobs(int resumeIndex) {
    if (resumeIndex < 0 || resumeIndex >= resumes.size()) {
        cout << "Invalid index.\n";
        return;
    }
    cout << "Matching 3 jobs for resume: " << resumes[resumeIndex].name << endl;
}

// 4. Add resume
void ResumeArray::addResume(const Resume& newResume) {
    resumes.push_back(newResume);
    cout << "Added resume: " << newResume.name << endl;
}

// 5. Delete resume
void ResumeArray::deleteResume(int position) {
    if (resumes.empty()) {
        cout << "No resumes to delete.\n";
        return;
    }
    if (position < 0 || position >= resumes.size()) {
        cout << "Invalid position.\n";
        return;
    }
    resumes.erase(resumes.begin() + position);
    cout << "Deleted resume at position " << position << endl;
}

void ResumeArray::printResumes(int count) const {
    for (int i = 0; i < count && i < resumes.size(); ++i) {
        cout << i + 1 << ". " << resumes[i].name << " - " << resumes[i].skills << endl;
    }
}