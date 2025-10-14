#include "resume_array.h"
#include <iostream>
#include <fstream>
#include <sstream>

ResumeArray::ResumeArray(): resumes(nullptr), resumesCount(0), resumesCapacity(0) {}

ResumeArray::~ResumeArray() { delete[] resumes; }

void ResumeArray::ensureCapacity(int minCapacity) {
    if (resumesCapacity >= minCapacity) return;
    int newCap = resumesCapacity == 0 ? 8 : resumesCapacity * 2;
    while (newCap < minCapacity) newCap *= 2;
    Resume *temp = new Resume[newCap];
    for (int i = 0; i < resumesCount; ++i) temp[i] = resumes[i];
    delete[] resumes;
    resumes = temp;
    resumesCapacity = newCap;
}

// 1. Load CSV
void ResumeArray::loadResumes(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Error opening file: " << filename << std::endl; return; }

    // remember filename so we can save back to same temp file
    csvFilename = filename;

    std::string line;
    std::getline(file, line); // skip header
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        Resume r;
        std::getline(ss, r.id, ',');
        std::getline(ss, r.name, ',');
        std::getline(ss, r.skills, ',');
        std::getline(ss, r.experience, ',');

        ensureCapacity(resumesCount + 1);
        resumes[resumesCount++] = r;
    }
}

void ResumeArray::saveToCSV(const std::string &filename) {
    std::ofstream file(filename);
    if (!file.is_open()) { std::cerr << "Error: cannot open " << filename << " for writing\n"; return; }
    // simple header matching expected fields
    file << "id,name,skills,experience" << std::endl;
    auto esc = [](const std::string &s){
        std::string out; out.reserve(s.size()*2);
        for (char c: s) {
            if (c == '"') out += "\"\""; // CSV escape: double the quote
            else out.push_back(c);
        }
        return out;
    };
    for (int i = 0; i < resumesCount; ++i) {
        file << '"' << esc(resumes[i].id) << '"' << ','
             << '"' << esc(resumes[i].name) << '"' << ','
             << '"' << esc(resumes[i].skills) << '"' << ','
             << '"' << esc(resumes[i].experience) << '"' << std::endl;
    }
    file.close();
    std::cout << "Successfully saved " << resumesCount << " resumes to " << filename << std::endl;
}

bool ResumeArray::confirmAction(const std::string &message) {
    std::cout << "\n" << message << std::endl;
    std::cout << "Do you want to save this change to the CSV file? (y/n): ";
    char c; std::cin >> c; std::cin.ignore();
    return (c == 'y' || c == 'Y');
}

// 2. Match resumes
void ResumeArray::matchResumes(const Resume *otherResumes, int otherCount) {
    int count = 0;
    for (int i = 0; i < resumesCount; ++i) {
        for (int j = 0; j < otherCount; ++j) {
            if (resumes[i].skills == otherResumes[j].skills) count++;
        }
    }
    std::cout << "Matched " << count << " resumes.\n";
}

// 3. Match 3 jobs
void ResumeArray::matchThreeJobs(int resumeIndex) {
    if (resumeIndex < 0 || resumeIndex >= resumesCount) { std::cout << "Invalid index.\n"; return; }
    std::cout << "Matching 3 jobs for resume: " << resumes[resumeIndex].name << std::endl;
}

// 4. Add resume
void ResumeArray::addResume(const Resume& newResume) {
    ensureCapacity(resumesCount + 1);
    resumes[resumesCount++] = newResume;
    std::cout << "Added resume: " << newResume.name << std::endl;
    if (!csvFilename.empty()) {
        if (confirmAction("A new resume has been added to the array.")) saveToCSV(csvFilename);
        else std::cout << "Change saved in memory only.\n";
    } else {
        std::cout << "Warning: no CSV filename stored. Cannot auto-save.\n";
    }
}

// 5. Delete resume
void ResumeArray::deleteResume(int position) {
    if (resumesCount == 0) { std::cout << "No resumes to delete.\n"; return; }
    if (position < 0 || position >= resumesCount) { std::cout << "Invalid position.\n"; return; }
    for (int i = position; i + 1 < resumesCount; ++i) resumes[i] = resumes[i+1];
    --resumesCount;
    std::cout << "Deleted resume at position " << position << std::endl;
    if (!csvFilename.empty()) {
        if (confirmAction("A resume record has been deleted from the array.")) saveToCSV(csvFilename);
        else std::cout << "Change saved in memory only.\n";
    } else {
        std::cout << "Warning: no CSV filename stored. Cannot auto-save.\n";
    }
}

void ResumeArray::printResumes(int count) const {
    for (int i = 0; i < count && i < resumesCount; ++i) {
        std::cout << i + 1 << ". " << resumes[i].name << " - " << resumes[i].skills << std::endl;
    }
}