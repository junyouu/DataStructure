#include <iostream>
#include <chrono>  // for timing
#include "job_linkedlist.h"
#include "resume_linkedlist.h"
#include "matching.h"
using namespace std;
using namespace std::chrono;

int main() {
    cout << "=== Job & Resume Matching System ===\n";

    string jobFile = "../data/job_description.csv";
    string resumeFile = "../data/resume.csv";

    // --- Load Jobs ---
    auto startJob = high_resolution_clock::now();
    JobLinkedList jobLinkedList;
    jobLinkedList.loadFromCSV(jobFile);
    auto endJob = high_resolution_clock::now();
    auto jobDuration = duration_cast<milliseconds>(endJob - startJob).count();

    cout << "\nLoaded " << jobLinkedList.size() << " job(s) in "
         << jobDuration << " ms.\n";
    jobLinkedList.display();

    // --- Load Resumes ---
    auto startResume = high_resolution_clock::now();
    ResumeLinkedList resumeLinkedList;
    resumeLinkedList.loadFromCSV(resumeFile);
    auto endResume = high_resolution_clock::now();
    auto resumeDuration = duration_cast<milliseconds>(endResume - startResume).count();

    cout << "\nLoaded " << resumeLinkedList.size() << " resume(s) in "
         << resumeDuration << " ms.\n";
    resumeLinkedList.display();

    // --- Matching ---
    auto startMatch = high_resolution_clock::now();
    Matcher::findTopMatches(jobLinkedList, resumeLinkedList);
    auto endMatch = high_resolution_clock::now();
    auto matchDuration = duration_cast<milliseconds>(endMatch - startMatch).count();

    // --- Summary ---
    cout << "\n=== Execution Time Summary ===\n";
    cout << "Job Loading:    " << jobDuration << " ms\n";
    cout << "Resume Loading: " << resumeDuration << " ms\n";
    cout << "Matching:       " << matchDuration << " ms\n";
    cout << "Total:          " << (jobDuration + resumeDuration + matchDuration) << " ms\n";

    return 0;
}
