#include "matching.h"
#include <iostream>
#include <algorithm>
#include <cctype>
using namespace std;

// helper function to lowercase string
string toLower(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void Matcher::findTopMatches(JobLinkedList &jobs, ResumeLinkedList &resumes) {
    JobNode *job = jobs.getHead();

    cout << "\n=== Matching Results (Top 3 Candidates for Each Job) ===\n";

    // loop through each job
    while (job != nullptr) {
        cout << "\nJob [" << job->jobID << "]:\n";
        cout << "Title: " << job->title << "\n";
        cout << "Keywords: ";
        bool printed = false;
        for (int i = 0; i < 10; ++i) {
            if (!job->keywords[i].empty()) {
                if (printed) cout << ", ";
                cout << job->keywords[i];
                printed = true;
            }
        }
        if (!printed) cout << "(none)";
        cout << "\nOriginal Text: " << job->description << "\n";

        // if no job keywords, skip
        int jobKeywordCount = 0;
        for (int i = 0; i < 10; ++i)
            if (!job->keywords[i].empty()) jobKeywordCount++;

        if (jobKeywordCount == 0) {
            cout << "No keywords found for this job.\n";
            job = job->next;
            continue;
        }

        // store matches as pair<resumeID, rate>
        struct Match {
            int resumeID;
            double rate;
            string desc;
        };
        Match matches[100];
        int matchCount = 0;

        // loop through resumes
        ResumeNode *resume = resumes.getHead();
        while (resume != nullptr && matchCount < 100) {
            int matchKeywords = 0;

            for (int i = 0; i < 10; ++i) {
                if (job->keywords[i].empty()) continue;
                string jobKey = toLower(job->keywords[i]);

                for (int j = 0; j < 10; ++j) {
                    if (resume->keywords[j].empty()) continue;
                    string resumeKey = toLower(resume->keywords[j]);

                    if (jobKey == resumeKey) {
                        matchKeywords++;
                        break; // prevent double counting
                    }
                }
            }

            double rate = (double)matchKeywords / jobKeywordCount * 100.0;

            matches[matchCount].resumeID = resume->resumeID;
            matches[matchCount].rate = rate;
            matches[matchCount].desc = resume->description;
            matchCount++;

            resume = resume->next;
        }

        // sort by rate descending
        sort(matches, matches + matchCount, [](const Match &a, const Match &b) {
            return a.rate > b.rate;
        });

        cout << "Candidate:\n";
        int top = min(3, matchCount);
        for (int i = 0; i < top; ++i) {
            cout << i + 1 << ". Resume [" << matches[i].resumeID << "] ("
                 << matches[i].rate << "%)\n";
            cout << matches[i].desc << "\n\n";
        }

        job = job->next;
    }
}

// Match top 3 resumes for a specific job ID
void Matcher::matchTop3ForJob(int jobID, JobLinkedList &jobs, ResumeLinkedList &resumes) {
    // Find the job by ID
    JobNode *job = jobs.getHead();
    while (job != nullptr) {
        if (job->jobID == jobID) {
            break;
        }
        job = job->next;
    }
    
    if (job == nullptr) {
        cout << "Job ID " << jobID << " not found.\n";
        return;
    }
    
    cout << "\n=== Matching Top 3 Resumes for Job ID " << jobID << " ===\n";
    cout << "Job Title: " << job->title << "\n";
    cout << "Job Keywords: ";
    bool printed = false;
    for (int i = 0; i < 10; ++i) {
        if (!job->keywords[i].empty()) {
            if (printed) cout << ", ";
            cout << job->keywords[i];
            printed = true;
        }
    }
    if (!printed) cout << "(none)";
    cout << "\n\n";
    
    // Count job keywords
    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i)
        if (!job->keywords[i].empty()) jobKeywordCount++;
    
    if (jobKeywordCount == 0) {
        cout << "No keywords found for this job.\n";
        return;
    }
    
    // Store matches
    struct Match {
        int resumeID;
        double rate;
        string desc;
    };
    Match matches[100];
    int matchCount = 0;
    
    // Loop through resumes
    ResumeNode *resume = resumes.getHead();
    while (resume != nullptr && matchCount < 100) {
        int matchKeywords = 0;
        
        for (int i = 0; i < 10; ++i) {
            if (job->keywords[i].empty()) continue;
            string jobKey = toLower(job->keywords[i]);
            
            for (int j = 0; j < 10; ++j) {
                if (resume->keywords[j].empty()) continue;
                string resumeKey = toLower(resume->keywords[j]);
                
                if (jobKey == resumeKey) {
                    matchKeywords++;
                    break;
                }
            }
        }
        
        double rate = (double)matchKeywords / jobKeywordCount * 100.0;
        
        matches[matchCount].resumeID = resume->resumeID;
        matches[matchCount].rate = rate;
        matches[matchCount].desc = resume->description;
        matchCount++;
        
        resume = resume->next;
    }
    
    // Sort by rate descending
    sort(matches, matches + matchCount, [](const Match &a, const Match &b) {
        return a.rate > b.rate;
    });
    
    cout << "Top 3 Candidates:\n";
    int top = min(3, matchCount);
    for (int i = 0; i < top; ++i) {
        cout << i + 1 << ". Resume [" << matches[i].resumeID << "] - Match Rate: "
             << matches[i].rate << "%\n";
        cout << "   " << matches[i].desc << "\n\n";
    }
}