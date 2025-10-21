#include "job_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <windows.h>
#include <psapi.h>
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
        stringstream ss(node->description);
        ss >> node->title;
        return;
    }

    node->title = node->description.substr(0, phrasePos);
    while (!node->title.empty() && isspace(node->title.back()))
        node->title.pop_back();

    size_t startPos = phrasePos + string("needed with experience").length();
    size_t endPos = node->description.find('.', startPos);
    if (endPos == string::npos)
        endPos = node->description.length();

    string skills = node->description.substr(startPos, endPos - startPos);

    if (skills.find(" in ") == 0) skills.erase(0, 4);
    skills.erase(0, skills.find_first_not_of(" "));
    skills.erase(skills.find_last_not_of(" ") + 1);

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


// ---------------- loadFromCSV ----------------
void JobLinkedList::loadFromCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    csvFilename = filename;
    string line;
    int recordCount = 0;

    getline(file, line); // skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        insertAtEnd(line);
        recordCount++;
    }

    file.close();

    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();
    cout << "[Performance] loadFromCSV [Job Linked List] execution time: " 
         << duration << " microseconds\n";

    // ===== MEMORY USAGE (Windows) =====
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        cout << "[Memory] loadFromCSV [Job Linked List] memory usage: "
             << pmc.WorkingSetSize / 1024.0 << " KB\n" << endl;
    } else {
        cout << "[Memory] Unable to retrieve memory usage info.\n";
    }
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
    cout << "[Performance] display execution time: " << duration << " microseconds\n";
}

// ---------------- addRecord ----------------
void JobLinkedList::addRecord() {
    string jobTitle, skills;
    cout << "\n=== ADD NEW JOB ===\n";
    cout << "Enter job title: ";
    getline(cin, jobTitle);
    cout << "Enter required skills (comma-separated, e.g., Python, Java, SQL): ";
    getline(cin, skills);

    string description = jobTitle + " needed with experience in " + skills + ".";

    auto start = high_resolution_clock::now();
    insertAtEnd(description);
    auto endBeforeSave = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endBeforeSave - start).count();
    cout << "[Performance] addRecord execution time (excluding save): " << duration << " microseconds\n";

    cout << "\n=== NEW JOB ADDED ===\n";
    cout << "Job ID: " << count << endl;
    cout << "Title: " << tail->title << endl;
    cout << "Description: " << tail->description << endl;

    if (confirmAction("A new job record has been added to the list.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
        else cout << "Warning: No CSV filename stored. Cannot save to file.\n";
    } else {
        cout << "Change saved in memory only.\n";
    }
}

// ---------------- deleteFromHead ----------------
void JobLinkedList::deleteFromHead() {
    auto start = high_resolution_clock::now();

    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }

    JobNode *temp = head;
    cout << "\nDeleting Job ID " << temp->jobID << " (Head)\n";
    head = head->next;
    if (!head) tail = nullptr;
    delete temp;
    count--;

    auto endBeforeSave = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endBeforeSave - start).count();
    cout << "[Performance] deleteFromHead execution time (excluding save): " << duration << " microseconds\n";

    if (confirmAction("Head job record deleted.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
    }
}

// ---------------- deleteFromTail ----------------
void JobLinkedList::deleteFromTail() {
    auto start = high_resolution_clock::now();

    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }

    if (head == tail) {
        delete head;
        head = tail = nullptr;
        count--;
    } else {
        JobNode *curr = head;
        while (curr->next != tail) curr = curr->next;
        delete tail;
        tail = curr;
        tail->next = nullptr;
        count--;
    }

    auto endBeforeSave = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endBeforeSave - start).count();
    cout << "[Performance] deleteFromTail execution time (excluding save): " << duration << " microseconds\n";

    if (confirmAction("Tail job record deleted.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
    }
}

// ---------------- deleteFromMiddle ----------------
void JobLinkedList::deleteFromMiddle(int position) {
    auto start = high_resolution_clock::now();

    if (position < 1 || position > count) {
        cout << "Invalid position.\n";
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
    delete curr;
    count--;

    auto endBeforeSave = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(endBeforeSave - start).count();
    cout << "[Performance] deleteFromMiddle execution time (excluding save): " << duration << " microseconds\n";

    if (confirmAction("Middle job record deleted.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
    }
}

// ---------------- utility ----------------
bool JobLinkedList::confirmAction(const string &message) {
    cout << "\n" << message << endl;
    cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response;
    cin >> response;
    cin.ignore();
    return (response == 'y' || response == 'Y');
}

void JobLinkedList::clear() {
    while (head) {
        JobNode *tmp = head;
        head = head->next;
        delete tmp;
    }
    tail = nullptr;
    count = 0;
}

void JobLinkedList::saveToCSV(const string &filename) const {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open " << filename << endl;
        return;
    }

    JobNode* current = head;
    while (current != nullptr) {
        file << current->jobID << "," << current->description << "\n";
        current = current->next;
    }

    file.close();
    cout << "[Info] Jobs saved to " << filename << endl;
}

// Find job node by ID
JobNode* JobLinkedList::findJobByID(int id) const {
    JobNode* current = head;
    while (current != nullptr) {
        if (current->jobID == id)
            return current;
        current = current->next;
    }
    return nullptr; // not found
}

int JobLinkedList::size() const { return count; }
JobNode *JobLinkedList::getHead() const { return head; }
