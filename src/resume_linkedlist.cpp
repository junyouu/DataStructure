#include "resume_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
using namespace std;

ResumeLinkedList::ResumeLinkedList() {
    head = tail = nullptr;
    count = 0;
    csvFilename = "";
}

ResumeLinkedList::~ResumeLinkedList() {
    clear();
}

// ---------------- keyword extraction ----------------
void extractResumeKeywords(ResumeNode *node) {
    string desc = node->description;
    transform(desc.begin(), desc.end(), desc.begin(), ::tolower);

    // Find "skilled in"
    size_t startPos = desc.find("skilled in");
    if (startPos == string::npos) return;

    startPos += string("skilled in").length();

    // Find the end of the sentence
    size_t endPos = desc.find('.', startPos);
    if (endPos == string::npos)
        endPos = desc.length();

    string skills = node->description.substr(startPos, endPos - startPos);

    // Trim spaces
    skills.erase(0, skills.find_first_not_of(" "));
    skills.erase(skills.find_last_not_of(" ") + 1);

    // Split by comma
    stringstream ss(skills);
    string token;
    int idx = 0;

    while (getline(ss, token, ',') && idx < 10) {
        token.erase(0, token.find_first_not_of(" "));
        token.erase(token.find_last_not_of(" ") + 1);
        if (!token.empty()) node->keywords[idx++] = token;
    }

    for (int i = idx; i < 10; ++i)
        node->keywords[i] = "";
}

// ---------------- insert node ----------------
void ResumeLinkedList::insertAtEnd(const string &desc) {
    ResumeNode *node = new ResumeNode();
    node->resumeID = count + 1;
    node->description = desc;
    node->next = nullptr;

    if (!head)
        head = tail = node;
    else {
        tail->next = node;
        tail = node;
    }

    count++;
    extractResumeKeywords(node);
}

// ---------------- load file ----------------
void ResumeLinkedList::loadFromCSV(const string &filename) {
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
}

// ---------------- save file ----------------
void ResumeLinkedList::saveToCSV(const string &filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << " for writing" << endl;
        return;
    }

    // Write header
    file << "resume" << endl;
    
    // Write all records
    ResumeNode *curr = head;
    while (curr) {
        file << "\"" << curr->description << "\"" << endl;
        curr = curr->next;
    }

    file.close();
    cout << "Successfully saved " << count << " records to " << filename << endl;
}

// ---------------- utility ----------------
void ResumeLinkedList::clear() {
    while (head) {
        ResumeNode *temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    count = 0;
}

int ResumeLinkedList::size() const {
    return count;
}

ResumeNode *ResumeLinkedList::getHead() const {
    return head;
}

// ---------------- display ----------------
void ResumeLinkedList::display() const {
    const ResumeNode *current = head;
    cout << "\n=== Resume List ===\n";

    while (current != nullptr) {
        cout << "ID: " << current->resumeID << "\n";
        cout << "Keywords: ";

        bool printed = false;
        for (int i = 0; i < 10; ++i) {
            if (!current->keywords[i].empty()) {
                if (printed) cout << ", ";
                cout << current->keywords[i];
                printed = true;
            }
        }
        if (!printed) cout << "(none)";
        cout << "\nOriginal Text: " << current->description << "\n\n";

        current = current->next;
    }

    if (!head)
        cout << "(No resumes loaded)\n";
}

// ---------------- New Functions ----------------

// Helper function to confirm action
bool ResumeLinkedList::confirmAction(const string &message) {
    cout << "\n" << message << endl;
    cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response;
    cin >> response;
    cin.ignore();  // Clear the newline from buffer
    return (response == 'y' || response == 'Y');
}

// Add new resume record
void ResumeLinkedList::addRecord() {
    // Build description from user input
    string skills;
    
    cout << "\n=== ADD NEW RESUME ===\n";
    cout << "Enter your skills (comma-separated, e.g., Python, Java, SQL): ";
    getline(cin, skills);
    
    // Format the description to match CSV format
    string description = "Experienced professional skilled in " + skills + ".";
    
    // Insert the new resume
    insertAtEnd(description);
    
    // Show what was added
    cout << "\n=== NEW RESUME ADDED ===\n";
    cout << "Resume ID: " << count << endl;
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
    cout << "========================\n";
    
    // Ask for confirmation
    if (confirmAction("A new resume record has been added to the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// Delete from head
void ResumeLinkedList::deleteFromHead() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    // Show what will be deleted
    ResumeNode *temp = head;
    cout << "\n=== DELETING RESUME FROM HEAD ===\n";
    cout << "Resume ID: " << temp->resumeID << endl;
    cout << "Description: " << temp->description << endl;
    cout << "=================================\n";
    
    // Perform deletion
    head = head->next;
    if (!head) {
        tail = nullptr;
    }
    
    delete temp;
    count--;
    
    // Ask for confirmation
    if (confirmAction("Resume record has been deleted from the list.")) {
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
void ResumeLinkedList::deleteFromMiddle(int position) {
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
    
    ResumeNode *curr = head;
    ResumeNode *prev = nullptr;
    
    for (int i = 1; i < position; i++) {
        prev = curr;
        curr = curr->next;
    }
    
    // Show what will be deleted
    cout << "\n=== DELETING RESUME FROM POSITION " << position << " ===\n";
    cout << "Resume ID: " << curr->resumeID << endl;
    cout << "Description: " << curr->description << endl;
    cout << "=================================\n";
    
    // Perform deletion
    prev->next = curr->next;
    delete curr;
    count--;
    
    // Ask for confirmation
    if (confirmAction("Resume record has been deleted from the list.")) {
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
void ResumeLinkedList::deleteFromTail() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    // Show what will be deleted
    cout << "\n=== DELETING RESUME FROM TAIL ===\n";
    cout << "Resume ID: " << tail->resumeID << endl;
    cout << "Description: " << tail->description << endl;
    cout << "=================================\n";
    
    if (head == tail) {
        delete head;
        head = tail = nullptr;
        count--;
    } else {
        ResumeNode *curr = head;
        while (curr->next != tail) {
            curr = curr->next;
        }
        
        delete tail;
        tail = curr;
        tail->next = nullptr;
        count--;
    }
    
    // Ask for confirmation
    if (confirmAction("Resume record has been deleted from the list.")) {
        if (!csvFilename.empty()) {
            saveToCSV(csvFilename);
        } else {
            cout << "Warning: No CSV filename stored. Cannot save to file.\n";
        }
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}