#include "job_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
using namespace std;
using namespace std::chrono;

JobLinkedList::JobLinkedList() {
    head = tail = nullptr;
    count = 0;
    csvFilename = "";
}

JobLinkedList::~JobLinkedList() {
    clear();
}

// ---------------- keyword + title extraction ----------------
void extractInfo(JobNode *node) {
    string desc = node->description;
    transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

    size_t phrasePos = desc.find("needed with experience");
    if (phrasePos == string::npos) {
        // no phrase found; use first few words as title
        stringstream ss(node->description);
        ss >> node->title;
        return;
    }

    // title = text before the phrase (capitalize later)
    node->title = node->description.substr(0, phrasePos);
    while (!node->title.empty() && isspace(node->title.back()))
        node->title.pop_back();

    // locate keyword section
    size_t startPos = phrasePos + string("needed with experience").length();
    size_t endPos = node->description.find('.', startPos);
    if (endPos == string::npos)
        endPos = node->description.length();

    string skills = node->description.substr(startPos, endPos - startPos);

    // remove leading " in "
    if (skills.find(" in ") == 0) skills.erase(0, 4);

    // trim spaces
    skills.erase(0, skills.find_first_not_of(" "));
    skills.erase(skills.find_last_not_of(" ") + 1);

    // split by comma
    stringstream ss(skills);
    string token;
    int idx = 0;
    while (getline(ss, token, ',') && idx < 10) {
        token.erase(0, token.find_first_not_of(" "));
        token.erase(token.find_last_not_of(" ") + 1);
        if (!token.empty()) node->keywords[idx++] = token;
    }
    for (int i = idx; i < 10; ++i) node->keywords[i] = "";
}

// ---------------- insert node ----------------
void JobLinkedList::insertAtEnd(const string &desc) {
    JobNode *node = new JobNode();
    node->jobID = count + 1;
    node->description = desc;
    node->next = nullptr;

    if (!head)
        head = tail = node;
    else {
        tail->next = node;
        tail = node;
    }

    count++;
    extractInfo(node);
}

// ---------------- load file ----------------
void JobLinkedList::loadFromCSV(const string &filename) {
    auto start = high_resolution_clock::now();
    
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    csvFilename = filename;  // store filename for later saving
    string line;
    int recordCount = 0;

    // Skip the header line
    getline(file, line);
    
    // while (getline(file, line) && recordCount < 100) {
    while (getline(file, line)) {
        if (line.empty()) continue;
        insertAtEnd(line);
        recordCount++;
    }

    file.close();
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "[Performance] Load execution time: " << duration << " microseconds\n";
}

// ---------------- save file ----------------
void JobLinkedList::saveToCSV(const string &filename) {
    auto start = high_resolution_clock::now();
    
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << " for writing" << endl;
        return;
    }

    // Write header
    file << "job_description" << endl;
    
    // Write all records
    JobNode *curr = head;
    while (curr) {
        file << "\"" << curr->description << "\"" << endl;
        curr = curr->next;
    }

    file.close();
    cout << "Successfully saved " << count << " records to " << filename << endl;
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "[Performance] Save execution time: " << duration << " microseconds\n";
}

// ---------------- utility methods ----------------
void JobLinkedList::clear() {
    while (head) {
        JobNode *tmp = head;
        head = head->next;
        delete tmp;
    }
    tail = nullptr;
    count = 0;
}

int JobLinkedList::size() const {
    return count;
}

JobNode *JobLinkedList::getHead() const {
    return head;
}

// ---------------- display ----------------
void JobLinkedList::display() const {
    auto start = high_resolution_clock::now();
    
    const JobNode *curr = head;
    cout << "\n=== Job List ===\n";
    while (curr) {
        cout << "ID: " << curr->jobID << "\n";
        cout << "Title: " << curr->title << "\n";
        cout << "Keywords: ";
        bool printed = false;
        for (int i = 0; i < 10; ++i) {
            if (!curr->keywords[i].empty()) {
                if (printed) cout << ", ";
                cout << curr->keywords[i];
                printed = true;
            }
        }
        if (!printed) cout << "(none)";
        cout << "\nOriginal Text: " << curr->description << "\n\n";
        curr = curr->next;
    }

    if (!head) cout << "(No jobs loaded)\n";
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "[Performance] Display execution time: " << duration << " microseconds\n";
}

// ---------------- New Functions ----------------

// Helper function to confirm action
bool JobLinkedList::confirmAction(const string &message) {
    cout << "\n" << message << endl;
    cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response;
    cin >> response;
    cin.ignore();  // Clear the newline from buffer
    return (response == 'y' || response == 'Y');
}

// Add new job record
void JobLinkedList::addRecord() {
    // Build description from user input
    string jobTitle, skills;
    
    cout << "\n=== ADD NEW JOB ===\n";
    cout << "Enter job title: ";
    getline(cin, jobTitle);
    
    cout << "Enter required skills (comma-separated, e.g., Python, Java, SQL): ";
    getline(cin, skills);
    
    // Format the description to match CSV format
    string description = jobTitle + " needed with experience in " + skills + ".";
    
    // Insert the new job
    insertAtEnd(description);
    
    // Show what was added
    cout << "\n=== NEW JOB ADDED ===\n";
    cout << "Job ID: " << count << endl;
    cout << "Title: " << tail->title << endl;
    cout << "Keywords: ";
    bool printed = false;
    for (int i = 0; i < 10; ++i) {
        if (!tail->keywords[i].empty()) {
            if (printed) cout << ", ";
            cout << tail->keywords[i];
            printed = true;
        }
    }
    if (!printed) cout << "(none)";
    cout << "\nDescription: " << tail->description << endl;
    cout << "====================\n";
    
    // Ask for confirmation
    if (confirmAction("A new job record has been added to the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// Find job by ID
JobNode* JobLinkedList::findJobByID(int jobID) {
    JobNode *curr = head;
    while (curr) {
        if (curr->jobID == jobID) {
            return curr;
        }
        curr = curr->next;
    }
    return nullptr;
}

// Delete from head
void JobLinkedList::deleteFromHead() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    // Show what will be deleted
    JobNode *temp = head;
    cout << "\n=== DELETING JOB FROM HEAD ===\n";
    cout << "Job ID: " << temp->jobID << endl;
    cout << "Title: " << temp->title << endl;
    cout << "Description: " << temp->description << endl;
    cout << "==============================\n";
    
    // Perform deletion
    head = head->next;
    if (!head) {
        tail = nullptr;
    }
    
    delete temp;
    count--;
    
    // Ask for confirmation
    if (confirmAction("Job record has been deleted from the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// Delete from middle (1-based position)
void JobLinkedList::deleteFromMiddle(int position) {
    if (position < 1 || position > count) {
        cout << "Invalid position. Must be between 1 and " << count << endl;
        return;
    }
    
    if (position == 1) {
        deleteFromHead();
        return;
    }
    
    if (position == count) {
        deleteFromTail();
        return;
    }
    
    JobNode *curr = head;
    JobNode *prev = nullptr;
    
    for (int i = 1; i < position; i++) {
        prev = curr;
        curr = curr->next;
    }
    
    // Show what will be deleted
    cout << "\n=== DELETING JOB FROM POSITION " << position << " ===\n";
    cout << "Job ID: " << curr->jobID << endl;
    cout << "Title: " << curr->title << endl;
    cout << "Description: " << curr->description << endl;
    cout << "==============================\n";
    
    // Perform deletion
    prev->next = curr->next;
    delete curr;
    count--;
    
    // Ask for confirmation
    if (confirmAction("Job record has been deleted from the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// Delete from tail
void JobLinkedList::deleteFromTail() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    // Show what will be deleted
    cout << "\n=== DELETING JOB FROM TAIL ===\n";
    cout << "Job ID: " << tail->jobID << endl;
    cout << "Title: " << tail->title << endl;
    cout << "Description: " << tail->description << endl;
    cout << "==============================\n";
    
    if (head == tail) {
        delete head;
        head = tail = nullptr;
        count--;
    } else {
        JobNode *curr = head;
        while (curr->next != tail) {
            curr = curr->next;
        }
        
        delete tail;
        tail = curr;
        tail->next = nullptr;
        count--;
    }
    
    // Ask for confirmation
    if (confirmAction("Job record has been deleted from the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}