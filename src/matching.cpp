#include "matching.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;
using namespace std::chrono;


string toLower(const string &s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}


struct MatchNode {
    int resumeID;
    double rate;
    string desc;
    MatchNode *next;
    MatchNode(int id, double r, string d) : resumeID(id), rate(r), desc(d), next(nullptr) {}
};

// Append node to linked list
void appendMatchNode(MatchNode* &head, int id, double rate, const string &desc) {
    MatchNode *newNode = new MatchNode(id, rate, desc);
    if (!head) head = newNode;
    else {
        MatchNode *temp = head;
        while (temp->next) temp = temp->next;
        temp->next = newNode;
    }
}

// Sort linked list (descending by rate)
void sortMatchLinkedList(MatchNode* &head) {
    if (!head || !head->next) return;
    bool swapped;
    do {
        swapped = false;
        MatchNode *curr = head;
        while (curr->next) {
            if (curr->rate < curr->next->rate) {
                swap(curr->resumeID, curr->next->resumeID);
                swap(curr->rate, curr->next->rate);
                swap(curr->desc, curr->next->desc);
                swapped = true;
            }
            curr = curr->next;
        }
    } while (swapped);
}

// Print top N matches
void printTopNLinkedList(MatchNode *head, int n) {
    int count = 0;
    while (head && count < n) {
        cout << count + 1 << ". Resume [" << head->resumeID << "] (" << head->rate << "%)\n"
             << head->desc << "\n";
        head = head->next;
        count++;
    }
}


void Matcher::matchTop3ForJobLinkedList(int jobID, JobLinkedList &jobs, ResumeLinkedList &resumes) {
    auto totalStart = high_resolution_clock::now(); // start total timer

    JobNode *job = jobs.findJobByID(jobID);
    if (!job) {
        cout << "Job ID " << jobID << " not found.\n";
        return;
    }

    cout << "\n=== Matching Resumes for Job [" << jobID << "] [Linked List] ===\n";
    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i)
        if (!job->keywords[i].empty())
            jobKeywordCount++;

    if (jobKeywordCount == 0) {
        cout << "No keywords found for this job.\n";
        return;
    }

    // Measure performance for unsorted matching
    auto unsortedStart = high_resolution_clock::now();

    MatchNode *head = nullptr;
    ResumeNode *resume = resumes.getHead();
    while (resume) {
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

        double rate = (double)matchKeywords / jobKeywordCount * 100.0;
        appendMatchNode(head, resume->resumeID, rate, resume->description);
        resume = resume->next;
    }

    auto unsortedEnd = high_resolution_clock::now();

    cout << "\n=== Unsorted Match Results ===\n";
    MatchNode *curr = head;
    int index = 1;
    while (curr) {
        cout << index++ << ". Resume [" << curr->resumeID << "] (" 
             << curr->rate << "%)\n" << curr->desc << "\n";
        curr = curr->next;
    }

    cout << "[Performance] Matching (Unsorted) execution time: "
         << duration_cast<microseconds>(unsortedEnd - unsortedStart).count()
         << " microseconds\n";

    // Ask if user wants sorted results
    char sortChoice;
    cout << "\nWould you like to view the sorted top 3 resumes? (y/n): ";
    cin >> sortChoice;

    if (tolower(sortChoice) == 'y') {
        auto sortStart = high_resolution_clock::now();
        sortMatchLinkedList(head);
        auto sortEnd = high_resolution_clock::now();

        cout << "\n=== Top 3 Sorted Matches ===\n";
        printTopNLinkedList(head, 3);

        cout << "[Sorting Performance] (Linked List): "
             << duration_cast<microseconds>(sortEnd - sortStart).count()
             << " microseconds\n";
    }

    auto totalEnd = high_resolution_clock::now();
    cout << "[Total Performance] (Linked List Matching + Optional Sorting): "
         << duration_cast<microseconds>(totalEnd - totalStart).count()
         << " microseconds\n";

    // cleanup
    while (head) { MatchNode *temp = head; head = head->next; delete temp; }
}


void Matcher::searchByKeywordLinkedList(ResumeLinkedList &resumes) {
    string keyword;
    cout << "\nEnter a keyword to search resumes: ";
    cin.ignore();
    getline(cin, keyword);
    keyword = toLower(keyword);

    auto start = high_resolution_clock::now();
    bool found = false;

    ResumeNode *resume = resumes.getHead();
    while (resume) {
        for (int i = 0; i < 10; ++i) {
            if (toLower(resume->keywords[i]) == keyword) {
                cout << "Resume [" << resume->resumeID << "] matched: " << resume->description << "\n";
                found = true;
                break;
            }
        }
        resume = resume->next;
    }

    auto end = high_resolution_clock::now();
    cout << "[Search Performance] (Linked List): "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!found)
        cout << "No resumes found with the keyword \"" << keyword << "\".\n";
}



void Matcher::matchTop3ForJobArray(int jobIndex, JobArray &jobs, ResumeArray &resumes) {
    if (jobIndex < 0 || jobIndex >= jobs.getSize()) {
        cout << "Invalid job index.\n";
        return;
    }

    const Job &job = jobs.getJob(jobIndex);
    cout << "\n=== Matching Resumes for Job [" << jobIndex + 1 << "] [Array] ===\n";

    int jobKeywordCount = 0;
    for (int i = 0; i < 10; ++i)
        if (!job.keywords[i].empty())
            jobKeywordCount++;

    if (jobKeywordCount == 0) {
        cout << "No keywords found for this job.\n";
        return;
    }

    struct Match { int resumeIndex; double rate; string desc; };
    int totalResumes = resumes.size();
    Match* matches = new Match[totalResumes];  // dynamically allocate enough for all resumes
    int matchCount = 0;

    // Measure performance for unsorted matching
    auto unsortedStart = high_resolution_clock::now();

    for (int j = 0; j < totalResumes; ++j) {
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

    auto unsortedEnd = high_resolution_clock::now();

    cout << "\n=== Unsorted Match Results ===\n";
    for (int m = 0; m < matchCount; ++m)
        cout << m + 1 << ". Resume [" << matches[m].resumeIndex + 1 << "] ("
             << matches[m].rate << "%)\n" << matches[m].desc << "\n";

    cout << "[Performance] Matching (Unsorted) execution time: "
         << duration_cast<microseconds>(unsortedEnd - unsortedStart).count()
         << " microseconds\n";

    // Ask user if they want sorting
    char sortChoice;
    cout << "\nWould you like to view the sorted top 3 resumes? (y/n): ";
    cin >> sortChoice;

    if (tolower(sortChoice) == 'y') {
        auto sortStart = high_resolution_clock::now();

        // Sort descending by rate (using bubble sort for educational clarity)
       for (int i = 0; i < matchCount - 1; ++i) {
            bool swapped = false;
            for (int j = 0; j < matchCount - i - 1; ++j) {
                if (matches[j].rate < matches[j + 1].rate) {
                    swap(matches[j].resumeIndex, matches[j + 1].resumeIndex);
                    swap(matches[j].rate, matches[j + 1].rate);
                    swap(matches[j].desc, matches[j + 1].desc);
                    swapped = true;
                }
            }
            if (!swapped) break; // optional optimization
        }

        auto sortEnd = high_resolution_clock::now();

        cout << "\n=== Top 3 Sorted Matches ===\n";
        for (int t = 0; t < min(3, matchCount); ++t)
            cout << t + 1 << ". Resume [" << matches[t].resumeIndex + 1 << "] ("
                 << matches[t].rate << "%)\n" << matches[t].desc << "\n";

        cout << "[Sorting Performance] (Array): "
             << duration_cast<microseconds>(sortEnd - sortStart).count()
             << " microseconds\n";
    }

    delete[] matches;

    auto totalEnd = high_resolution_clock::now();
    cout << "[Total Performance] (Array Matching + Optional Sorting): "
         << duration_cast<microseconds>(totalEnd - unsortedStart).count()
         << " microseconds\n";
}



void Matcher::searchByKeywordArray(ResumeArray &resumes) {
    string keyword;
    cout << "\nEnter a keyword to search resumes: ";
    cin.ignore();
    getline(cin, keyword);
    keyword = toLower(keyword);

    auto start = high_resolution_clock::now();
    bool found = false;

    for (int i = 0; i < resumes.size(); ++i) {
        const Resume &resume = resumes.getResume(i);
        for (int k = 0; k < 10; ++k) {
            if (toLower(resume.keywords[k]) == keyword) {
                cout << "Resume [" << i + 1 << "] matched: " << resume.description << "\n";
                found = true;
                break;
            }
        }
    }

    auto end = high_resolution_clock::now();
    cout << "[Search Performance] (Array): "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (!found)
        cout << "No resumes found with the keyword \"" << keyword << "\".\n";
}

void Matcher::matchTop3ForResumeLinkedList(int resumeID, JobLinkedList &jobs, ResumeLinkedList &resumes) {
    auto totalStart = high_resolution_clock::now();

    ResumeNode *resume = resumes.findResumeByID(resumeID);
    if (!resume) {
        cout << "Resume ID " << resumeID << " not found.\n";
        return;
    }

    cout << "\n=== Matching Jobs for Resume [" << resumeID << "] [Linked List] ===\n";
    cout << "Resume Description:\n" << resume->description << "\n\n";

    MatchNode *head = nullptr;
    auto matchStart = high_resolution_clock::now();

    JobNode *job = jobs.getHead();
    while (job) {
        int jobKeywordCount = 0, matchKeywords = 0;
        for (int j = 0; j < 10; ++j)
            if (!job->keywords[j].empty())
                jobKeywordCount++;

        if (jobKeywordCount == 0) {
            job = job->next;
            continue;
        }

        for (int j = 0; j < 10; ++j) {
            if (job->keywords[j].empty()) continue;
            string jobKey = toLower(job->keywords[j]);
            for (int i = 0; i < 10; ++i) {
                if (resume->keywords[i].empty()) continue;
                if (jobKey == toLower(resume->keywords[i])) {
                    matchKeywords++;
                    break;
                }
            }
        }

        double rate = (double)matchKeywords / jobKeywordCount * 100.0;
        appendMatchNode(head, job->jobID, rate, job->title + " | " + job->description);
        job = job->next;
    }

    auto matchEnd = high_resolution_clock::now();
    auto sortStart = high_resolution_clock::now();
    sortMatchLinkedList(head);
    auto sortEnd = high_resolution_clock::now();

    cout << "\n=== Top 3 Sorted Job Matches ===\n";
    printTopNLinkedList(head, 3);

    cout << "[Performance] Matching (Linked List): "
         << duration_cast<microseconds>(matchEnd - matchStart).count() << " microseconds\n";
    cout << "[Sorting Performance] (Linked List): "
         << duration_cast<microseconds>(sortEnd - sortStart).count() << " microseconds\n";

    auto totalEnd = high_resolution_clock::now();
    cout << "[Total Performance]: "
         << duration_cast<microseconds>(totalEnd - totalStart).count() << " microseconds\n";

    while (head) { MatchNode *temp = head; head = head->next; delete temp; }
}

void Matcher::matchTop3ForResumeArray(int resumeIndex, JobArray &jobs, ResumeArray &resumes) {
    if (resumeIndex < 0 || resumeIndex >= resumes.size()) {
        cout << "Invalid resume index.\n";
        return;
    }

    const Resume &resume = resumes.getResume(resumeIndex);
    cout << "\n=== Matching Jobs for Resume [" << resumeIndex + 1 << "] [Array] ===\n";
    cout << "Resume Description:\n" << resume.description << "\n\n";

    struct Match { int jobIndex; double rate; string title; };
    int totalJobs = jobs.getSize();
    Match* matches = new Match[totalJobs];
    int matchCount = 0;

    auto matchStart = high_resolution_clock::now();

    for (int j = 0; j < totalJobs; ++j) {
        const Job &job = jobs.getJob(j);
        int jobKeywordCount = 0, matchKeywords = 0;

        for (int b = 0; b < 10; ++b)
            if (!job.keywords[b].empty())
                jobKeywordCount++;

        if (jobKeywordCount == 0)
            continue;

        for (int b = 0; b < 10; ++b) {
            if (job.keywords[b].empty()) continue;
            string jobKey = toLower(job.keywords[b]);
            for (int a = 0; a < 10; ++a) {
                if (resume.keywords[a].empty()) continue;
                if (jobKey == toLower(resume.keywords[a])) {
                    matchKeywords++;
                    break;
                }
            }
        }

        double rate = (double)matchKeywords / jobKeywordCount * 100.0;
        matches[matchCount++] = { j, rate, job.title + " | " + job.description };
    }

    auto matchEnd = high_resolution_clock::now();

    auto sortStart = high_resolution_clock::now();
    for (int i = 0; i < matchCount - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < matchCount - i - 1; ++j) {
            if (matches[j].rate < matches[j + 1].rate) {
                swap(matches[j].jobIndex, matches[j + 1].jobIndex);
                swap(matches[j].rate, matches[j + 1].rate);
                swap(matches[j].title, matches[j + 1].title);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
    auto sortEnd = high_resolution_clock::now();

    cout << "\n=== Top 3 Sorted Job Matches ===\n";
    for (int t = 0; t < min(3, matchCount); ++t) {
        cout << t + 1 << ". Job [" << matches[t].jobIndex + 1 << "] (" 
             << matches[t].rate << "%)\n" << matches[t].title << "\n\n";
    }

    cout << "[Performance] Matching (Array): "
         << duration_cast<microseconds>(matchEnd - matchStart).count() << " microseconds\n";
    cout << "[Sorting Performance] (Array): "
         << duration_cast<microseconds>(sortEnd - sortStart).count() << " microseconds\n";

    auto totalEnd = high_resolution_clock::now();
    cout << "[Total Performance]: "
         << duration_cast<microseconds>(totalEnd - matchStart).count() << " microseconds\n";

    delete[] matches;
}

