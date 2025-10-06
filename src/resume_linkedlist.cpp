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
