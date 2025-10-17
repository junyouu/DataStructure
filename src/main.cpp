#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include "job_linkedlist.h"
#include "resume_linkedlist.h"
#include "job_array.h"
#include "resume_array.h"
#include "utility.h"
#include "matching.h"
using namespace std;

int main() {
    cout << "========================================\n";
    cout << "              Loading Job               \n";
    cout << "========================================\n";

    string jobFile = "../data/job_description.csv";
    string resumeFile = "../data/resume.csv";
    string tempFolder = "../data/temp";

    // Create temp folder
    string command = "mkdir \"" + tempFolder + "\"";
    system(command.c_str());

    // Copy files into temp folder
    string jobCopy = tempFolder + "/job_description.csv";
    string resumeCopy = tempFolder + "/resume.csv";

    if (!copyFile(jobFile, jobCopy)) {
        cout << "Failed to copy job file!" << endl;
        return 1;
    }

    if (!copyFile(resumeFile, resumeCopy)) {
        cout << "Failed to copy resume file!" << endl;
        return 1;
    }

    // --- Load Data ---
    JobLinkedList jobLinkedList;
    jobLinkedList.loadFromCSV(jobCopy);

    JobArray jobArray;
    jobArray.loadFromCSV(jobCopy);

    ResumeLinkedList resumeLinkedList;
    resumeLinkedList.loadFromCSV(resumeCopy);

    ResumeArray resumeArray;
    resumeArray.loadFromCSV(resumeCopy);

    // --- Menu Loop ---
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

        if (choice >= 1 && choice <= 6) {
            int dataChoice;
            cout << "\nUse Data Structure:\n1. Linked List\n2. Array\nChoose: ";
            cin >> dataChoice;

            if (cin.fail() || (dataChoice != 1 && dataChoice != 2)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid selection.\n";
                continue;
            }

            switch (choice) {
                case 1: {
                    // Match top 3 resumes for a specific job ID
                    int jobID;
                    if (dataChoice == 1) {
                        cout << "\nEnter Job ID (1 to " << jobLinkedList.size() << "): ";
                        cin >> jobID;
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            cout << "Invalid input.\n";
                            break;
                        }
                        Matcher::matchTop3ForJobLinkedList(jobID, jobLinkedList, resumeLinkedList);
                    } else {
                        cout << "\nEnter Job ID (1 to " << jobArray.getSize() << "): ";
                        cin >> jobID;
                        if (cin.fail()) {
                            cin.clear();
                            cin.ignore(10000, '\n');
                            cout << "Invalid input.\n";
                            break;
                        }
                        Matcher::matchTop3ForJobArray(jobID - 1, jobArray, resumeArray);
                    }
                    break;
                }

                case 2: {
                    // Add new record
                    int type;
                    cout << "\nAdd:\n1. Job\n2. Resume\nChoose type: ";
                    cin >> type;
                    cin.ignore(10000, '\n');

                    if (dataChoice == 1) {
                        if (type == 1) jobLinkedList.addRecord();
                        else if (type == 2) resumeLinkedList.addRecord();
                        else cout << "Invalid choice.\n";
                    } else {
                        if (type == 1) jobArray.addRecord();
                        else if (type == 2) resumeArray.addRecord();
                        else cout << "Invalid choice.\n";
                    }
                    break;
                }

                case 3: {
                    // Delete record
                    int type, deleteChoice;
                    cout << "\nDelete from:\n1. Jobs\n2. Resumes\nChoose type: ";
                    cin >> type;

                    if (dataChoice == 1) {
                        cout << "\nDelete:\n1. From Head\n2. From Middle (position)\n3. From Tail\nChoose: ";
                        cin >> deleteChoice;

                        if (type == 1) {
                            if (deleteChoice == 1) jobLinkedList.deleteFromHead();
                            else if (deleteChoice == 2) {
                                int pos; cout << "Enter position (1 to " << jobLinkedList.size() << "): "; cin >> pos;
                                jobLinkedList.deleteFromMiddle(pos);
                            } else if (deleteChoice == 3) jobLinkedList.deleteFromTail();
                            else cout << "Invalid choice.\n";
                        } else if (type == 2) {
                            if (deleteChoice == 1) resumeLinkedList.deleteFromHead();
                            else if (deleteChoice == 2) {
                                int pos; cout << "Enter position (1 to " << resumeLinkedList.size() << "): "; cin >> pos;
                                resumeLinkedList.deleteFromMiddle(pos);
                            } else if (deleteChoice == 3) resumeLinkedList.deleteFromTail();
                            else cout << "Invalid choice.\n";
                        } else cout << "Invalid type.\n";
                    } else {
                        cout << "\nDelete:\n1. From Head\n2. From Middle (position)\n3. From Tail\nChoose: ";
                        cin >> deleteChoice;
                        if (type == 1) {
                            if (deleteChoice == 1) jobArray.deleteFromHead();
                            else if (deleteChoice == 2) {
                                int pos; cout << "Enter position (1 to " << jobArray.getSize() << "): "; cin >> pos;
                                jobArray.deleteFromMiddle(pos);
                            } else if (deleteChoice == 3) jobArray.deleteFromTail();
                            else cout << "Invalid choice.\n";
                        } else if (type == 2) {
                            if (deleteChoice == 1) resumeArray.deleteFromHead();
                            else if (deleteChoice == 2) {
                                int pos; cout << "Enter position (1 to " << resumeArray.size() << "): "; cin >> pos;
                                resumeArray.deleteFromMiddle(pos);
                            } else if (deleteChoice == 3) resumeArray.deleteFromTail();
                            else cout << "Invalid choice.\n";
                        } else cout << "Invalid type.\n";
                    }
                    break;
                }

                case 4: {
                    // Display all jobs
                    if (dataChoice == 1) jobLinkedList.display();
                    else jobArray.display();
                    break;
                }

                case 5: {
                    // Display all resumes
                    if (dataChoice == 1) resumeLinkedList.display();
                    else resumeArray.display();
                    break;
                }

                case 6: {
                    // Full matching
                    if (dataChoice == 1)
                        Matcher::findTopMatchesLinkedList(jobLinkedList, resumeLinkedList);
                    else
                        Matcher::findTopMatchesArray(jobArray, resumeArray);
                    break;
                }
            }
        } 
        else if (choice == 0) {
            cout << "\nThank you for using the Job & Resume Matching System!\n";
        } 
        else {
            cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
