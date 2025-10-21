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

    string command = "mkdir \"" + tempFolder + "\"";
    system(command.c_str());

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

    JobLinkedList jobLinkedList;
    jobLinkedList.loadFromCSV(jobCopy);

    JobArray jobArray;
    jobArray.loadFromCSV(jobCopy);

    ResumeLinkedList resumeLinkedList;
    resumeLinkedList.loadFromCSV(resumeCopy);

    ResumeArray resumeArray;
    resumeArray.loadFromCSV(resumeCopy);

    int choice;
    do {
        cout << "\n========================================\n";
        cout << "     Job & Resume Matching System\n";
        cout << "========================================\n";
        cout << "1. Match Top 3 Resumes for a Job\n";
        cout << "2. Match Top 3 Jobs for a Resume\n";
        cout << "3. Search Resume by Keyword\n";
        cout << "4. Add Record (Job/Resume)\n";
        cout << "5. Delete Record (Job/Resume)\n";
        cout << "6. Display All Jobs\n";
        cout << "7. Display All Resumes\n";
        cout << "0. Exit\n";
        cout << "========================================\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice >= 1 && choice <= 7) {
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
                    int jobID;
                    if (dataChoice == 1) {
                        cout << "\nEnter Job ID (1 to " << jobLinkedList.size() << "): ";
                        cin >> jobID;
                        Matcher::matchTop3ForJobLinkedList(jobID, jobLinkedList, resumeLinkedList);
                    } else {
                        cout << "\nEnter Job ID (1 to " << jobArray.getSize() << "): ";
                        cin >> jobID;
                        Matcher::matchTop3ForJobArray(jobID - 1, jobArray, resumeArray);
                    }
                    break;
                }

                case 2: {
                    int resumeID;
                    if (dataChoice == 1) {
                        cout << "\nEnter Resume ID (1 to " << resumeLinkedList.size() << "): ";
                        cin >> resumeID;
                        Matcher::matchTop3ForResumeLinkedList(resumeID, jobLinkedList, resumeLinkedList);
                    } else {
                        cout << "\nEnter Resume ID (1 to " << resumeArray.size() << "): ";
                        cin >> resumeID;
                        Matcher::matchTop3ForResumeArray(resumeID - 1, jobArray, resumeArray);
                    }
                    break;
                }

                case 3: {
                    if (dataChoice == 1)
                        Matcher::searchByKeywordLinkedList(resumeLinkedList);
                    else
                        Matcher::searchByKeywordArray(resumeArray);
                    break;
                }

                case 4: {
                    int type;
                    cout << "\nAdd:\n1. Job\n2. Resume\nChoose type: ";
                    cin >> type;
                    cin.ignore(10000, '\n');

                    if (dataChoice == 1) {
                        if (type == 1) jobLinkedList.addRecord();
                        else if (type == 2) resumeLinkedList.addRecord();
                    } else {
                        if (type == 1) jobArray.addRecord();
                        else if (type == 2) resumeArray.addRecord();
                    }
                    break;
                }

                case 5: {
                    int type, delChoice;
                    cout << "\nDelete from:\n1. Jobs\n2. Resumes\nChoose type: ";
                    cin >> type;

                    cout << "\nDelete:\n1. From Head\n2. From Middle\n3. From Tail\nChoose: ";
                    cin >> delChoice;

                    if (dataChoice == 1) {
                        if (type == 1) {
                            if (delChoice == 1) jobLinkedList.deleteFromHead();
                            else if (delChoice == 2) {
                                int pos; cout << "Enter position: "; cin >> pos;
                                jobLinkedList.deleteFromMiddle(pos);
                            } else if (delChoice == 3) jobLinkedList.deleteFromTail();
                        } else if (type == 2) {
                            if (delChoice == 1) resumeLinkedList.deleteFromHead();
                            else if (delChoice == 2) {
                                int pos; cout << "Enter position: "; cin >> pos;
                                resumeLinkedList.deleteFromMiddle(pos);
                            } else if (delChoice == 3) resumeLinkedList.deleteFromTail();
                        }
                    } else {
                        if (type == 1) {
                            if (delChoice == 1) jobArray.deleteFromHead();
                            else if (delChoice == 2) {
                                int pos; cout << "Enter position: "; cin >> pos;
                                jobArray.deleteFromMiddle(pos);
                            } else if (delChoice == 3) jobArray.deleteFromTail();
                        } else if (type == 2) {
                            if (delChoice == 1) resumeArray.deleteFromHead();
                            else if (delChoice == 2) {
                                int pos; cout << "Enter position: "; cin >> pos;
                                resumeArray.deleteFromMiddle(pos);
                            } else if (delChoice == 3) resumeArray.deleteFromTail();
                        }
                    }
                    break;
                }

                case 6: {
                    if (dataChoice == 1) jobLinkedList.display();
                    else jobArray.display();
                    break;
                }

                case 7: {
                    if (dataChoice == 1) resumeLinkedList.display();
                    else resumeArray.display();
                    break;
                }
            }
        } else if (choice == 0) {
            cout << "\nThank you for using the Job & Resume Matching System!\n";
        } else {
            cout << "Invalid choice. Please try again.\n";
        }

    } while (choice != 0);

    return 0;
}
