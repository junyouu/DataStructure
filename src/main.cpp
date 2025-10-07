#include <iostream>
#include <chrono>  // for timing
#include <iomanip>
#include <cstdlib>
#include <string>
#include "job_linkedlist.h"
#include "resume_linkedlist.h"
#include "utility.h"
#include "matching.h"
using namespace std;
using namespace std::chrono;

int main() {
    cout << "=== Job & Resume Matching System ===\n";

    std::string jobFile = "../data/job_description.csv";
    std::string resumeFile = "../data/resume.csv";
    std::string tempFolder = "../data/temp";

    // Create temp folder s
    std::string command = "mkdir \"" + tempFolder + "\"";
    system(command.c_str());

    // Copy files into temp folder
    std::string jobCopy = tempFolder + "/job_description.csv";
    std::string resumeCopy = tempFolder + "/resume.csv";

    if (!copyFile(jobFile, jobCopy)) {
        std::cout << "Failed to copy job file!" << std::endl;
        return 1;
    }

    if (!copyFile(resumeFile, resumeCopy)) {
        std::cout << "Failed to copy resume file!" << std::endl;
        return 1;
    }

    // --- Load Jobs ---
    auto startJob = high_resolution_clock::now();
    JobLinkedList jobLinkedList;
    jobLinkedList.loadFromCSV(jobCopy);
    auto endJob = high_resolution_clock::now();
    auto jobDuration = duration_cast<microseconds>(endJob - startJob).count();

    cout << "\nLoaded " << jobLinkedList.size() << " job(s) in "
         << jobDuration << " microseconds.\n";

    // --- Load Resumes ---
    auto startResume = high_resolution_clock::now();
    ResumeLinkedList resumeLinkedList;
    resumeLinkedList.loadFromCSV(resumeCopy);
    auto endResume = high_resolution_clock::now();
    auto resumeDuration = duration_cast<microseconds>(endResume - startResume).count();

    cout << "Loaded " << resumeLinkedList.size() << " resume(s) in "
         << resumeDuration << " microseconds.\n";

    // Menu loop
    int choice;
    do {
        displayMenu();
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1: {
                // Match top 3 resumes for a specific job ID
                int jobID;
                cout << "\nEnter Job ID (1 to " << jobLinkedList.size() << "): ";
                cin >> jobID;
                
                if (cin.fail()) {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Invalid input.\n";
                    break;
                }
                
                auto start = high_resolution_clock::now();
                Matcher::matchTop3ForJob(jobID, jobLinkedList, resumeLinkedList);
                auto end = high_resolution_clock::now();
                auto duration = duration_cast<microseconds>(end - start).count();
                
                cout << "\n[Performance] Matching execution time: " << duration << " microseconds\n";
                break;
            }
            
            case 2: {
                // Add new record
                int type;
                cout << "\nAdd:\n1. Job\n2. Resume\nChoose type: ";
                cin >> type;
                cin.ignore(10000, '\n');
                
                if (type == 1) {
                    auto start = high_resolution_clock::now();
                    jobLinkedList.addRecord();
                    auto end = high_resolution_clock::now();
                    auto duration = duration_cast<microseconds>(end - start).count();
                    
                    cout << "[Performance] Add job execution time: " << duration << " microseconds\n";
                } else if (type == 2) {
                    auto start = high_resolution_clock::now();
                    resumeLinkedList.addRecord();
                    auto end = high_resolution_clock::now();
                    auto duration = duration_cast<microseconds>(end - start).count();
                    
                    cout << "[Performance] Add resume execution time: " << duration << " microseconds\n";
                } else {
                    cout << "Invalid choice.\n";
                }
                break;
            }
            
            case 3: {
                // Delete record
                int type, deleteChoice;
                cout << "\nDelete from:\n1. Jobs\n2. Resumes\nChoose type: ";
                cin >> type;
                
                if (type == 1) {
                    cout << "\nDelete:\n1. From Head\n2. From Middle (position)\n3. From Tail\nChoose: ";
                    cin >> deleteChoice;
                    
                    auto start = high_resolution_clock::now();
                    if (deleteChoice == 1) {
                        jobLinkedList.deleteFromHead();
                    } else if (deleteChoice == 2) {
                        int pos;
                        cout << "Enter position (1 to " << jobLinkedList.size() << "): ";
                        cin >> pos;
                        jobLinkedList.deleteFromMiddle(pos);
                    } else if (deleteChoice == 3) {
                        jobLinkedList.deleteFromTail();
                    } else {
                        cout << "Invalid choice.\n";
                    }
                    auto end = high_resolution_clock::now();
                    auto duration = duration_cast<microseconds>(end - start).count();
                    
                    cout << "[Performance] Delete job execution time: " << duration << " microseconds\n";
                    
                } else if (type == 2) {
                    cout << "\nDelete:\n1. From Head\n2. From Middle (position)\n3. From Tail\nChoose: ";
                    cin >> deleteChoice;
                    
                    auto start = high_resolution_clock::now();
                    if (deleteChoice == 1) {
                        resumeLinkedList.deleteFromHead();
                    } else if (deleteChoice == 2) {
                        int pos;
                        cout << "Enter position (1 to " << resumeLinkedList.size() << "): ";
                        cin >> pos;
                        resumeLinkedList.deleteFromMiddle(pos);
                    } else if (deleteChoice == 3) {
                        resumeLinkedList.deleteFromTail();
                    } else {
                        cout << "Invalid choice.\n";
                    }
                    auto end = high_resolution_clock::now();
                    auto duration = duration_cast<microseconds>(end - start).count();
                    
                    cout << "[Performance] Delete resume execution time: " << duration << " microseconds\n";
                } else {
                    cout << "Invalid choice.\n";
                }
                break;
            }
            
            case 4: {
                // Display all jobs
                jobLinkedList.display();
                break;
            }
            
            case 5: {
                // Display all resumes
                resumeLinkedList.display();
                break;
            }
            
            case 6: {
                // Run full matching
                auto startMatch = high_resolution_clock::now();
                Matcher::findTopMatches(jobLinkedList, resumeLinkedList);
                auto endMatch = high_resolution_clock::now();
                auto matchDuration = duration_cast<microseconds>(endMatch - startMatch).count();
                
                cout << "\n[Performance] Full matching execution time: " << matchDuration << " microseconds\n";
                break;
            }
            
            case 0: {
                cout << "\n=== Final Execution Time Summary ===\n";
                cout << "Initial Job Loading:    " << jobDuration << " microseconds\n";
                cout << "Initial Resume Loading: " << resumeDuration << " microseconds\n";
                cout << "Total Initial:          " << (jobDuration + resumeDuration) << " microseconds\n";
                cout << "\nThank you for using the Job & Resume Matching System!\n";
                break;
            }
            
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        
    } while (choice != 0);

    return 0;
}
