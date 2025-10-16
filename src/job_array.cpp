#include "job_array.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <chrono>
using namespace std::chrono;

// Helper: trim whitespace and surrounding quotes
static std::string trim(const std::string &s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    std::string t = s.substr(start, end - start + 1);
    // remove surrounding quotes
    if (!t.empty() && t.front() == '"' && t.back() == '"') {
        t = t.substr(1, t.size() - 2);
    }
    return t;
}

static double safeStod(const std::string &s) {
    std::string t = trim(s);
    if (t.empty()) return 0.0;
    try { return std::stod(t); }
    catch (...) { std::cerr << "[Warning] safeStod: invalid numeric value '" << s << "'. Using 0.0\n"; return 0.0; }
}

// Simple CSV parser that returns a std::string array in a vector-like pattern
static void parseCSVLine(const std::string &line, std::string *outFields, int &outCount) {
    outCount = 0;
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuotes && i + 1 < line.size() && line[i+1] == '"') {
                cur.push_back('"');
                ++i; // skip escaped quote
            } else {
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            outFields[outCount++] = trim(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    // last field
    outFields[outCount++] = trim(cur);
}

// --- JobArray implementation ---
JobArray::JobArray(): jobs(nullptr), jobsCount(0), jobsCapacity(0) {}

JobArray::~JobArray() {
    delete[] jobs;
}

void JobArray::ensureCapacity(int minCapacity) {
    if (jobsCapacity >= minCapacity) return;
    int newCap = jobsCapacity == 0 ? 8 : jobsCapacity * 2;
    while (newCap < minCapacity) newCap *= 2;
    Job *temp = new Job[newCap];
    for (int i = 0; i < jobsCount; ++i) temp[i] = jobs[i];
    delete[] jobs;
    jobs = temp;
    jobsCapacity = newCap;
}

void JobArray::loadJobs(const std::string& filename) {
    auto start = high_resolution_clock::now();
    
    std::ifstream file(filename);
    if (!file.is_open()) { std::cerr << "Error opening file: " << filename << std::endl; return; }

    std::string line;
    if (!std::getline(file, line)) return; // skip header

    // temporary fixed-size storage for parsed fields per line
    const int MAX_FIELDS = 32;
    std::string fields[MAX_FIELDS];
    int fieldCount = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        parseCSVLine(line, fields, fieldCount);

        Job job;
        // If CSV contains only a single field per line (the full description),
        // treat that field as the description (this matches JobLinkedList behavior).
        if (fieldCount == 1) {
            job.id = "";
            job.title = "";
            job.description = fields[0];
            job.location = "";
            job.salary = 0.0;
        } else {
            job.id = (fieldCount > 0) ? fields[0] : "";
            job.title = (fieldCount > 1) ? fields[1] : "";
            job.description = (fieldCount > 2) ? fields[2] : "";
            job.location = (fieldCount > 3) ? fields[3] : "";
            job.salary = (fieldCount > 4) ? safeStod(fields[4]) : 0.0;
        }

        // extract title/keywords from description (similar to JobLinkedList's extractInfo)
        // convert description to lowercase copy for searching
        std::string descLower = job.description;
        std::transform(descLower.begin(), descLower.end(), descLower.begin(), ::tolower);

        size_t phrasePos = descLower.find("needed with experience");
        if (phrasePos == std::string::npos) {
            // fallback: use first token as title
            std::stringstream ss(job.description);
            ss >> job.title;
        } else {
            job.title = job.description.substr(0, phrasePos);
            while (!job.title.empty() && isspace(job.title.back())) job.title.pop_back();

            size_t startPos = phrasePos + std::string("needed with experience").length();
            size_t endPos = job.description.find('.', startPos);
            if (endPos == std::string::npos) endPos = job.description.length();
            std::string skills = job.description.substr(startPos, endPos - startPos);
            // remove leading " in " if present
            if (skills.find(" in ") == 0) skills.erase(0, 4);
            // trim
            if (!skills.empty()) {
                skills.erase(0, skills.find_first_not_of(" "));
                skills.erase(skills.find_last_not_of(" ") + 1);
            }
            // split by comma into keywords
            std::stringstream kss(skills);
            std::string token;
            int idx = 0;
            while (std::getline(kss, token, ',') && idx < 10) {
                // trim token
                token.erase(0, token.find_first_not_of(' '));
                token.erase(token.find_last_not_of(' ') + 1);
                job.keywords[idx++] = token;
            }
            // ensure remaining keywords are empty
            for (int i = idx; i < 10; ++i) job.keywords[i] = "";
        }

        // append
        ensureCapacity(jobsCount + 1);
        jobs[jobsCount++] = job;
    }
    csvFilename = filename;
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Load execution time: " << duration << " microseconds\n";
}

// Save jobs back to CSV (single-column description CSV format, matching linked-list save)
void JobArray::saveToCSV(const std::string &filename) {
    auto start = high_resolution_clock::now();
    std::ofstream file(filename);
    if (!file.is_open()) { std::cout << "Error: Cannot open " << filename << " for writing\n"; return; }
    file << "job_description" << std::endl;
    for (int i = 0; i < jobsCount; ++i) {
        file << '"' << jobs[i].description << '"' << std::endl;
    }
    file.close();
    std::cout << "Successfully saved " << jobsCount << " records to " << filename << std::endl;
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Save execution time: " << duration << " microseconds\n";
}

bool JobArray::confirmAction(const std::string &message) {
    std::cout << "\n" << message << std::endl;
    std::cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response; std::cin >> response; std::cin.ignore();
    return (response == 'y' || response == 'Y');
}

void JobArray::addRecord() {
    std::string title, skills;
    std::cout << "\n=== ADD NEW JOB (Array) ===\n";
    std::cout << "Enter job title: "; std::getline(std::cin, title);
    std::cout << "Enter required skills (comma-separated): "; std::getline(std::cin, skills);
    std::string description = title + " needed with experience in " + skills + ".";
    Job j; j.description = description; j.title = title;
    // extract keywords
    std::stringstream ss(skills);
    std::string token; int idx = 0;
    while (std::getline(ss, token, ',') && idx < 10) { token.erase(0, token.find_first_not_of(' ')); token.erase(token.find_last_not_of(' ') + 1); j.keywords[idx++] = token; }
    for (int i = idx; i < 10; ++i) j.keywords[i] = "";
    addJob(j);
    std::cout << "\n=== NEW JOB ADDED ===\n";
    std::cout << "Title: " << j.title << "\n";
    std::cout << "Keywords: "; bool printed = false; for (int i = 0; i < 10; ++i) { if (!j.keywords[i].empty()) { if (printed) std::cout << ", "; std::cout << j.keywords[i]; printed = true; }} if (!printed) std::cout << "(none)"; std::cout << "\nDescription: " << j.description << "\n";
    if (confirmAction("A new job record has been added to the array.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename); 
        else std::cout << "Warning: no CSV filename stored. Cannot save.\n";
    } else {
        std::cout << "Change saved in memory only.\n";
    }
}

void JobArray::deleteFromHead() {
    if (jobsCount == 0) { std::cout << "List is empty. Nothing to delete.\n"; return; }
    std::cout << "\n=== DELETING JOB FROM HEAD ===\n";
    std::cout << "Title: " << jobs[0].title << "\nDescription: " << jobs[0].description << "\n";
    for (int i = 0; i + 1 < jobsCount; ++i) jobs[i] = jobs[i+1]; --jobsCount;
    if (confirmAction("Job record has been deleted from the array.")) { if (!csvFilename.empty()) saveToCSV(csvFilename); else std::cout << "Warning: no CSV filename stored.\n"; } else std::cout << "Change saved in memory only.\n";
}

void JobArray::deleteFromMiddle(int position) {
    // position is 1-based
    if (position < 1 || position > jobsCount) { std::cout << "Invalid position.\n"; return; }
    if (position == 1) { deleteFromHead(); return; }
    if (position == jobsCount) { deleteFromTail(); return; }
    int idx = position - 1;
    std::cout << "\n=== DELETING JOB FROM POSITION " << position << " ===\n";
    std::cout << "Title: " << jobs[idx].title << "\nDescription: " << jobs[idx].description << "\n";
    for (int i = idx; i + 1 < jobsCount; ++i) jobs[i] = jobs[i+1]; --jobsCount;
    if (confirmAction("Job record has been deleted from the array.")) { if (!csvFilename.empty()) saveToCSV(csvFilename); else std::cout << "Warning: no CSV filename stored.\n"; } else std::cout << "Change saved in memory only.\n";
}

void JobArray::deleteFromTail() {
    if (jobsCount == 0) { std::cout << "List is empty.\n"; return; }
    std::cout << "\n=== DELETING JOB FROM TAIL ===\n";
    std::cout << "Title: " << jobs[jobsCount-1].title << "\nDescription: " << jobs[jobsCount-1].description << "\n";
    --jobsCount;
    if (confirmAction("Job record has been deleted from the array.")) { if (!csvFilename.empty()) saveToCSV(csvFilename); else std::cout << "Warning: no CSV filename stored.\n"; } else std::cout << "Change saved in memory only.\n";
}

void JobArray::matchJobs(const Job *otherJobs, int otherCount) {
    int matchCount = 0;
    for (int i = 0; i < jobsCount; ++i) {
        for (int j = 0; j < otherCount; ++j) {
            if (jobs[i].title == otherJobs[j].title) matchCount++;
        }
    }
    std::cout << "Total matches: " << matchCount << std::endl;
}

void JobArray::matchThreeResumes(int jobIndex) {
    if (jobIndex < 0 || jobIndex >= jobsCount) { std::cout << "Invalid index.\n"; return; }
    std::cout << "Matching 3 resumes for job: " << jobs[jobIndex].title << std::endl;
}

// Match top 3 resumes for a given job by comparing keywords
// ResumeArray forward-declared to avoid circular include in header
#include "resume_array.h"

void JobArray::matchTop3ForJobWithResumes(int jobIndex, const ResumeArray &resumesArr) {
    if (jobIndex < 0 || jobIndex >= jobsCount) { std::cout << "Invalid index.\n"; return; }
    const Job &job = jobs[jobIndex];

    std::cout << "\n=== Matching Top 3 Resumes for Job ID " << (jobIndex + 1) << " ===\n";
    std::cout << "Job Title: " << job.title << "\n";
    std::cout << "Job Keywords: ";
    bool printed = false;
    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i) if (!job.keywords[i].empty()) { if (printed) std::cout << ", "; std::cout << job.keywords[i]; printed = true; jobKeywordCount++; }
    if (!printed) std::cout << "(none)";
    std::cout << "\n\n";

    if (jobKeywordCount == 0) { std::cout << "No keywords found for this job.\n"; return; }

    struct Match { int resumeIndex; double rate; std::string desc; };
    Match matches[100];
    int matchCount = 0;

    int totalResumes = resumesArr.getSize();
    for (int r = 0; r < totalResumes && matchCount < 100; ++r) {
        const auto &res = resumesArr.getResume(r);
        // parse resume skills into keywords (comma-separated)
        std::vector<std::string> resKeys;
        std::stringstream rss(res.skills);
        std::string tok;
        while (std::getline(rss, tok, ',')) {
            // trim
            size_t s = tok.find_first_not_of(' ');
            size_t e = tok.find_last_not_of(' ');
            if (s == std::string::npos) continue;
            std::string key = tok.substr(s, e - s + 1);
            resKeys.push_back(key);
        }

        int matchKeywords = 0;
        for (int i = 0; i < 10; ++i) {
            if (job.keywords[i].empty()) continue;
            std::string jobKey = job.keywords[i];
            for (const auto &rk : resKeys) {
                if (jobKey == rk) { matchKeywords++; break; }
            }
        }

        double rate = (double)matchKeywords / (double)jobKeywordCount * 100.0;
        matches[matchCount].resumeIndex = r;
        matches[matchCount].rate = rate;
        matches[matchCount].desc = res.name + " - " + res.skills;
        matchCount++;
    }

    // sort matches by rate desc
    std::sort(matches, matches + matchCount, [](const Match &a, const Match &b){ return a.rate > b.rate; });

    int top = std::min(3, matchCount);
    std::cout << "Top " << top << " Candidates:\n";
    for (int i = 0; i < top; ++i) {
        std::cout << i + 1 << ". Resume [" << (matches[i].resumeIndex + 1) << "] (" << matches[i].rate << "%)\n";
        std::cout << "   " << matches[i].desc << "\n\n";
    }
}

void JobArray::findTopMatchesWithResumes(const ResumeArray &resumesArr) {
    std::cout << "\n=== Matching Results (Top 3 Candidates for Each Job) ===\n";
    int totalResumes = resumesArr.getSize();

    for (int i = 0; i < jobsCount; ++i) {
        const Job &job = jobs[i];
        std::cout << "\nJob [" << (i+1) << "]:\n";
        std::cout << "Title: " << job.title << "\n";
        std::cout << "Keywords: ";
        bool printed = false;
        int jobKeywordCount = 0;
        for (int k = 0; k < 10; ++k) if (!job.keywords[k].empty()) { if (printed) std::cout << ", "; std::cout << job.keywords[k]; printed = true; jobKeywordCount++; }
        if (!printed) std::cout << "(none)";
        std::cout << "\nOriginal Text: " << job.description << "\n";

        if (jobKeywordCount == 0) {
            std::cout << "No keywords found for this job.\n";
            continue;
        }

        struct Match { int resumeIndex; double rate; std::string desc; };
        Match matches[100];
        int matchCount = 0;

        for (int r = 0; r < totalResumes && matchCount < 100; ++r) {
            const auto &res = resumesArr.getResume(r);
            // parse resume skills into keywords
            std::vector<std::string> resKeys;
            std::stringstream rss(res.skills);
            std::string tok;
            while (std::getline(rss, tok, ',')) {
                size_t s = tok.find_first_not_of(' ');
                size_t e = tok.find_last_not_of(' ');
                if (s == std::string::npos) continue;
                std::string key = tok.substr(s, e - s + 1);
                resKeys.push_back(key);
            }

            int matchKeywords = 0;
            for (int a = 0; a < 10; ++a) {
                if (job.keywords[a].empty()) continue;
                for (const auto &rk : resKeys) {
                    if (job.keywords[a] == rk) { matchKeywords++; break; }
                }
            }
            double rate = (double)matchKeywords / (double)jobKeywordCount * 100.0;
            matches[matchCount].resumeIndex = r;
            matches[matchCount].rate = rate;
            matches[matchCount].desc = res.name + " - " + res.skills;
            matchCount++;
        }

        std::sort(matches, matches + matchCount, [](const Match &a, const Match &b){ return a.rate > b.rate; });

        std::cout << "Candidate:\n";
        int top = std::min(3, matchCount);
        for (int t = 0; t < top; ++t) {
            std::cout << t + 1 << ". Resume [" << (matches[t].resumeIndex + 1) << "] (" << matches[t].rate << "%)\n";
            std::cout << matches[t].desc << "\n\n";
        }
    }
}

void JobArray::addJob(const Job& newJob) {
    ensureCapacity(jobsCount + 1);
    jobs[jobsCount++] = newJob;
    std::cout << "Added job: " << newJob.title << std::endl;
}

void JobArray::deleteJob(int position) {
    if (jobsCount == 0) { std::cout << "No jobs to delete.\n"; return; }
    if (position < 0 || position >= jobsCount) { std::cout << "Invalid position.\n"; return; }
    for (int i = position; i + 1 < jobsCount; ++i) jobs[i] = jobs[i+1];
    --jobsCount;
    std::cout << "Deleted job at position " << position << std::endl;
}

void JobArray::printJobs(int count) const {
    auto start = high_resolution_clock::now();
    
    if (jobsCount == 0) {
        std::cout << "(No jobs loaded)\n";
        return;
    }
    std::cout << "\n=== Job List (Array) ===\n";
    for (int i = 0; i < count && i < jobsCount; ++i) {
        const Job &j = jobs[i];
        std::cout << "ID: " << (i + 1) << "\n";
        std::cout << "Title: " << j.title << "\n";
        std::cout << "Keywords: ";
        bool printed = false;
        for (int k = 0; k < 10; ++k) {
            if (!j.keywords[k].empty()) {
                if (printed) std::cout << ", ";
                std::cout << j.keywords[k];
                printed = true;
            }
        }
        if (!printed) std::cout << "(none)";
        std::cout << "\nOriginal Text: " << j.description << "\n\n";
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    std::cout << "[Performance] Display execution time: " << duration << " microseconds\n";
}