#include "matching.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;
using namespace std::chrono;

// Helper: lowercase
string toLower(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

/* ============================
   LINKED LIST VERSION
   ============================ */
void Matcher::findTopMatchesLinkedList(JobLinkedList &jobs, ResumeLinkedList &resumes) {
    auto startAll = high_resolution_clock::now();
    cout << "\n=== Matching Results (Top 3 Candidates for Each Job) [Linked List] ===\n";

    JobNode *job = jobs.getHead();
    while (job != nullptr) {
        cout << "\nJob [" << job->jobID << "]: " << job->title << "\n";

        int jobKeywordCount = 0;
        for (int i = 0; i < 10; ++i)
            if (!job->keywords[i].empty()) jobKeywordCount++;
        if (jobKeywordCount == 0) {
            cout << "No keywords found.\n";
            job = job->next;
            continue;
        }

        struct Match { int resumeID; double rate; string desc; };
        Match matches[100]; int matchCount = 0;

        ResumeNode *resume = resumes.getHead();
        while (resume && matchCount < 100) {
            int matchKeywords = 0;
            for (int i = 0; i < 10; ++i) {
                if (job->keywords[i].empty()) continue;
                string jobKey = toLower(job->keywords[i]);
                for (int j = 0; j < 10; ++j) {
                    if (resume->keywords[j].empty()) continue;
                    if (jobKey == toLower(resume->keywords[j])) {
                        matchKeywords++;
                        break;
                    }
                }
            }
            matches[matchCount++] = { resume->resumeID, (double)matchKeywords / jobKeywordCount * 100.0, resume->description };
            resume = resume->next;
        }

        sort(matches, matches + matchCount, [](auto &a, auto &b){ return a.rate > b.rate; });

        for (int i = 0; i < min(3, matchCount); ++i)
            cout << i + 1 << ". Resume [" << matches[i].resumeID << "] ("
                 << matches[i].rate << "%)\n" << matches[i].desc << "\n";

        job = job->next;
    }

    auto endAll = high_resolution_clock::now();
    cout << "\n[Performance] Total Matching execution time (Linked List): "
         << duration_cast<microseconds>(endAll - startAll).count()
         << " microseconds\n";
}

void Matcher::matchTop3ForJobLinkedList(int jobID, JobLinkedList &jobs, ResumeLinkedList &resumes) {
    auto start = high_resolution_clock::now();

    JobNode *job = jobs.findJobByID(jobID);
    if (!job) {
        cout << "Job ID " << jobID << " not found.\n";
        return;
    }

    cout << "\n=== Matching Top 3 Resumes for Job [" << jobID << "] [Linked List] ===\n";

    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i)
        if (!job->keywords[i].empty()) jobKeywordCount++;
    if (jobKeywordCount == 0) {
        cout << "No keywords found for this job.\n";
        return;
    }

    struct Match { int resumeID; double rate; string desc; };
    Match matches[100]; int matchCount = 0;

    ResumeNode *resume = resumes.getHead();
    while (resume && matchCount < 100) {
        int matchKeywords = 0;
        for (int i = 0; i < 10; ++i) {
            if (job->keywords[i].empty()) continue;
            string jobKey = toLower(job->keywords[i]);
            for (int j = 0; j < 10; ++j) {
                if (resume->keywords[j].empty()) continue;
                if (jobKey == toLower(resume->keywords[j])) {
                    matchKeywords++;
                    break;
                }
            }
        }
        matches[matchCount++] = { resume->resumeID, (double)matchKeywords / jobKeywordCount * 100.0, resume->description };
        resume = resume->next;
    }

    sort(matches, matches + matchCount, [](auto &a, auto &b){ return a.rate > b.rate; });

    for (int i = 0; i < min(3, matchCount); ++i)
        cout << i + 1 << ". Resume [" << matches[i].resumeID << "] ("
             << matches[i].rate << "%)\n" << matches[i].desc << "\n";

    auto end = high_resolution_clock::now();
    cout << "[Performance] Matching execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

/* ============================
   ARRAY VERSION
   ============================ */
void Matcher::findTopMatchesArray(JobArray &jobs, ResumeArray &resumes) {
    auto startAll = high_resolution_clock::now();
    cout << "\n=== Matching Results (Top 3 Candidates for Each Job) [Array] ===\n";

    for (int i = 0; i < jobs.getSize(); ++i) {
        const Job &job = jobs.getJob(i);
        cout << "\nJob [" << i + 1 << "]: " << job.title << "\n";

        int jobKeywordCount = 0;
        for (int k = 0; k < 10; ++k)
            if (!job.keywords[k].empty()) jobKeywordCount++;
        if (jobKeywordCount == 0) {
            cout << "No keywords found.\n";
            continue;
        }

        struct Match { int resumeIndex; double rate; string desc; };
        Match matches[100]; int matchCount = 0;

        for (int j = 0; j < resumes.size() && matchCount < 100; ++j) {
            const Resume &resume = resumes.getResume(j);
            int matchKeywords = 0;
            for (int a = 0; a < 10; ++a) {
                if (job.keywords[a].empty()) continue;
                string jobKey = toLower(job.keywords[a]);
                for (int b = 0; b < 10; ++b) {
                    if (resume.keywords[b].empty()) continue;
                    if (jobKey == toLower(resume.keywords[b])) {
                        matchKeywords++;
                        break;
                    }
                }
            }
            matches[matchCount++] = { j, (double)matchKeywords / jobKeywordCount * 100.0, resume.description };
        }

        sort(matches, matches + matchCount, [](auto &a, auto &b){ return a.rate > b.rate; });

        for (int t = 0; t < min(3, matchCount); ++t)
            cout << t + 1 << ". Resume [" << matches[t].resumeIndex + 1 << "] ("
                 << matches[t].rate << "%)\n" << matches[t].desc << "\n";
    }

    auto endAll = high_resolution_clock::now();
    cout << "\n[Performance] Total Matching execution time (Array): "
         << duration_cast<microseconds>(endAll - startAll).count()
         << " microseconds\n";
}

void Matcher::matchTop3ForJobArray(int jobIndex, JobArray &jobs, ResumeArray &resumes) {
    auto start = high_resolution_clock::now();

    if (jobIndex < 0 || jobIndex >= jobs.getSize()) {
        cout << "Invalid job index.\n";
        return;
    }

    const Job &job = jobs.getJob(jobIndex);
    cout << "\n=== Matching Top 3 Resumes for Job [" << jobIndex + 1 << "] [Array] ===\n";

    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i)
        if (!job.keywords[i].empty()) jobKeywordCount++;
    if (jobKeywordCount == 0) {
        cout << "No keywords found.\n";
        return;
    }

    struct Match { int resumeIndex; double rate; string desc; };
    Match matches[100]; int matchCount = 0;

    for (int j = 0; j < resumes.size() && matchCount < 100; ++j) {
        const Resume &resume = resumes.getResume(j);
        int matchKeywords = 0;
        for (int a = 0; a < 10; ++a) {
            if (job.keywords[a].empty()) continue;
            string jobKey = toLower(job.keywords[a]);
            for (int b = 0; b < 10; ++b) {
                if (resume.keywords[b].empty()) continue;
                if (jobKey == toLower(resume.keywords[b])) {
                    matchKeywords++;
                    break;
                }
            }
        }
        matches[matchCount++] = { j, (double)matchKeywords / jobKeywordCount * 100.0, resume.description };
    }

    sort(matches, matches + matchCount, [](auto &a, auto &b){ return a.rate > b.rate; });

    for (int i = 0; i < min(3, matchCount); ++i)
        cout << i + 1 << ". Resume [" << matches[i].resumeIndex + 1 << "] ("
             << matches[i].rate << "%)\n" << matches[i].desc << "\n";

    auto end = high_resolution_clock::now();
    cout << "[Performance] Matching execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}
