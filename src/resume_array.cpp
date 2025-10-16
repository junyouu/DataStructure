#include "resume_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
using namespace std::chrono;

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
    auto start = high_resolution_clock::now();
    
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Error opening file: " << filename << std::endl; return; }

    // remember filename so we can save back to same temp file
    csvFilename = filename;

    std::string line;
    std::getline(file, line); // skip header
    
    // Read each line as a description (matching linked list format)
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // Remove surrounding quotes if present
        std::string description = line;
        if (!description.empty() && description.front() == '"' && description.back() == '"') {
            description = description.substr(1, description.size() - 2);
        }
        
        Resume r;
        r.id = "";
        r.name = "";
        r.skills = description;  // Store the full description in skills field
        r.experience = "";

        ensureCapacity(resumesCount + 1);
        resumes[resumesCount++] = r;
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Load execution time: " << duration << " microseconds\n";
}

void ResumeArray::saveToCSV(const std::string &filename) {
    auto start = high_resolution_clock::now();
    std::ofstream file(filename);
    if (!file.is_open()) { std::cerr << "Error: cannot open " << filename << " for writing\n"; return; }
    // Write header matching linked list format
    file << "resume" << std::endl;
    
    // Write all records (just the skills field which contains the description)
    for (int i = 0; i < resumesCount; ++i) {
        file << '"' << resumes[i].skills << '"' << std::endl;
    }
    file.close();
    std::cout << "Successfully saved " << resumesCount << " records to " << filename << std::endl;
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Save execution time: " << duration << " microseconds\n";
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
    std::cout << "Matching 3 jobs for resume: " << resumes[resumeIndex].skills << std::endl;
}

// 4. Add resume
void ResumeArray::addResume(const Resume& newResume) {
    ensureCapacity(resumesCount + 1);
    resumes[resumesCount++] = newResume;
}

// Interactive add record (matching linked list interface)
void ResumeArray::addRecord() {
    std::string skills;
    std::cout << "\n=== ADD NEW RESUME (Array) ===\n";
    std::cout << "Enter your skills (comma-separated, e.g., Python, Java, SQL): ";
    std::getline(std::cin, skills);
    
    // Format the description to match linked list format
    std::string description = "Experienced professional skilled in " + skills + ".";
    
    Resume r;
    r.id = "";
    r.name = "";
    r.skills = description;  // Store formatted description in skills field
    r.experience = "";
    
    ensureCapacity(resumesCount + 1);
    resumes[resumesCount++] = r;
    
    std::cout << "\n=== NEW RESUME ADDED ===\n";
    std::cout << "Resume ID: " << resumesCount << "\n";
    std::cout << "Description: " << r.skills << "\n";
    
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
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < count && i < resumesCount; ++i) {
        std::cout << i + 1 << ". " << resumes[i].skills << std::endl;
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Display execution time: " << duration << " microseconds\n";
}