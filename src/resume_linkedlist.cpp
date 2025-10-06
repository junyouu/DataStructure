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
    cout << "Loaded " << recordCount << " resumes from " << filename << endl;
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

// Add new resume record
void ResumeLinkedList::addRecord(const string &desc) {
    insertAtEnd(desc);
    cout << "New resume added successfully! Resume ID: " << count << endl;
}

// Delete from head
void ResumeLinkedList::deleteFromHead() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    ResumeNode *temp = head;
    head = head->next;
    
    if (!head) {
        tail = nullptr;
    }
    
    cout << "Deleted Resume ID: " << temp->resumeID << " from head\n";
    delete temp;
    count--;
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
    
    prev->next = curr->next;
    cout << "Deleted Resume ID: " << curr->resumeID << " from position " << position << endl;
    delete curr;
    count--;
}

// Delete from tail
void ResumeLinkedList::deleteFromTail() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    if (head == tail) {
        cout << "Deleted Resume ID: " << head->resumeID << " from tail\n";
        delete head;
        head = tail = nullptr;
        count--;
        return;
    }
    
    ResumeNode *curr = head;
    while (curr->next != tail) {
        curr = curr->next;
    }
    
    cout << "Deleted Resume ID: " << tail->resumeID << " from tail\n";
    delete tail;
    tail = curr;
    tail->next = nullptr;
    count--;
}