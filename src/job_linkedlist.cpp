#include "job_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
using namespace std;

JobLinkedList::JobLinkedList() {
    head = tail = nullptr;
    count = 0;
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
    cout << "Loaded " << recordCount << " records from " << filename << endl;
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
}

// ---------------- New Functions ----------------

// Add new job record
void JobLinkedList::addRecord(const string &desc) {
    insertAtEnd(desc);
    cout << "New job added successfully! Job ID: " << count << endl;
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
    
    JobNode *temp = head;
    head = head->next;
    
    if (!head) {
        tail = nullptr;
    }
    
    cout << "Deleted Job ID: " << temp->jobID << " from head\n";
    delete temp;
    count--;
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
    
    prev->next = curr->next;
    cout << "Deleted Job ID: " << curr->jobID << " from position " << position << endl;
    delete curr;
    count--;
}

// Delete from tail
void JobLinkedList::deleteFromTail() {
    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    if (head == tail) {
        cout << "Deleted Job ID: " << head->jobID << " from tail\n";
        delete head;
        head = tail = nullptr;
        count--;
        return;
    }
    
    JobNode *curr = head;
    while (curr->next != tail) {
        curr = curr->next;
    }
    
    cout << "Deleted Job ID: " << tail->jobID << " from tail\n";
    delete tail;
    tail = curr;
    tail->next = nullptr;
    count--;
}