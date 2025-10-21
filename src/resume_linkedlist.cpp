#include "resume_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <windows.h>
#include <psapi.h>
#include "utility.h"
using namespace std;
using namespace std::chrono;

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

    size_t startPos = desc.find("skilled in");
    if (startPos == string::npos) return;

    startPos += string("skilled in").length();

    size_t endPos = desc.find('.', startPos);
    if (endPos == string::npos)
        endPos = desc.length();

    string skills = node->description.substr(startPos, endPos - startPos);

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

    for (int i = idx; i < 10; ++i)
        node->keywords[i] = "";
}


void printMemoryUsage() {
    PROCESS_MEMORY_COUNTERS memInfo;
    GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo));
    SIZE_T memUsedKB = memInfo.WorkingSetSize / 1024;
    cout << "[Memory Usage] memory usage: " << memUsedKB << " KB\n\n";
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
    double memBefore = getCurrentMemoryKB();  // record before loading
    auto start = high_resolution_clock::now();
    
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }
    
    csvFilename = filename;
    string line;
    int recordCount = 0;

    getline(file, line); // Skip header

    while (getline(file, line)) {
        if (line.empty()) continue;
        insertAtEnd(line);
        recordCount++;
    }

    file.close();
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start).count();

    double memAfter = getCurrentMemoryKB();  // record after loading

    cout << "[Performance] loadFromCSV [Resume Linked List] execution time: "
         << duration << " microseconds\n";

    cout << "[Memory] loadFromCSV [Resume Linked List] memory usage: "
         << (memAfter - memBefore) << " KB\n\n";
}


// ---------------- save file ----------------
void ResumeLinkedList::saveToCSV(const string &filename) {
    auto start = high_resolution_clock::now();

    ofstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << " for writing" << endl;
        return;
    }

    file << "resume" << endl;

    ResumeNode *curr = head;
    while (curr) {
        file << "\"" << curr->description << "\"" << endl;
        curr = curr->next;
    }

    file.close();
    cout << "Successfully saved " << count << " records to " << filename << endl;
    
    auto end = high_resolution_clock::now();
    cout << "[Performance] saveToCSV execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
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
    auto start = high_resolution_clock::now();
    
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
    
    auto end = high_resolution_clock::now();
    cout << "[Performance] display execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
}

// ---------------- confirm action ----------------
bool ResumeLinkedList::confirmAction(const string &message) {
    cout << "\n" << message << endl;
    cout << "Do you want to save this change to the CSV file? (y/n): ";
    char response;
    cin >> response;
    cin.ignore();
    return (response == 'y' || response == 'Y');
}

// ---------------- add record ----------------
void ResumeLinkedList::addRecord() {
    string skills;
    cout << "\n=== ADD NEW RESUME ===\n";
    cout << "Enter your skills (comma-separated, e.g., Python, Java, SQL): ";
    getline(cin, skills);

    string description = "Experienced professional skilled in " + skills + ".";
    auto start = high_resolution_clock::now();
    insertAtEnd(description);
    auto end = high_resolution_clock::now();
    cout << "[Performance] addRecord execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

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

// void ResumeLinkedList::addRecord() {
//     string skills;
//     cout << "\n=== ADD NEW RESUME ===\n";
//     cout << "Enter your skills (comma-separated, e.g., Python, Java, SQL): ";
//     getline(cin, skills);

//     string description = "Experienced professional skilled in " + skills + ".";

//     // === Benchmark Mode ===
//     const int TEST_COUNT = 100000;  // Number of insertions for timing
//     auto start = high_resolution_clock::now();
//     for (int i = 0; i < TEST_COUNT; ++i) {
//         insertAtEnd(description);
//     }
//     auto end = high_resolution_clock::now();

//     double avg_ns = duration_cast<nanoseconds>(end - start).count();
//     cout << "[Performance] addRecord execution time: " << avg_ns << " ns per insert (avg over "
//          << TEST_COUNT << " inserts)\n";

//     // === Normal Mode Output ===
//     if (confirmAction("A new resume record has been added to the list.")) {
//         if (!csvFilename.empty()) {
//             saveToCSV(csvFilename);
//         } else {
//             cout << "Warning: No CSV filename stored. Cannot save to file.\n";
//         }
//     } else {
//         cout << "Change saved in memory only (not written to CSV file).\n";
//     }
// }


// ---------------- delete from head ----------------
void ResumeLinkedList::deleteFromHead() {
    auto start = high_resolution_clock::now();

    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }
    
    ResumeNode *temp = head;
    cout << "\n=== DELETING RESUME FROM HEAD ===\n";
    cout << "Resume ID: " << temp->resumeID << endl;
    cout << "Description: " << temp->description << endl;
    cout << "=================================\n";
    
    head = head->next;
    if (!head) tail = nullptr;
    delete temp;
    count--;

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromHead execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";
    
    if (confirmAction("Resume record has been deleted from the list.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
        else cout << "Warning: No CSV filename stored.\n";
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// ---------------- delete from middle ----------------
void ResumeLinkedList::deleteFromMiddle(int position) {
    auto start = high_resolution_clock::now();

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

    cout << "\n=== DELETING RESUME FROM POSITION " << position << " ===\n";
    cout << "Resume ID: " << curr->resumeID << endl;
    cout << "Description: " << curr->description << endl;
    cout << "=================================\n";

    prev->next = curr->next;
    delete curr;
    count--;

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromMiddle execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (confirmAction("Resume record has been deleted from the list.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
        else cout << "Warning: No CSV filename stored.\n";
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

// ---------------- delete from tail ----------------
void ResumeLinkedList::deleteFromTail() {
    auto start = high_resolution_clock::now();

    if (!head) {
        cout << "List is empty. Nothing to delete.\n";
        return;
    }

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
        while (curr->next != tail)
            curr = curr->next;
        delete tail;
        tail = curr;
        tail->next = nullptr;
        count--;
    }

    auto end = high_resolution_clock::now();
    cout << "[Performance] deleteFromTail execution time: "
         << duration_cast<microseconds>(end - start).count()
         << " microseconds\n";

    if (confirmAction("Resume record has been deleted from the list.")) {
        if (!csvFilename.empty()) saveToCSV(csvFilename);
        else cout << "Warning: No CSV filename stored.\n";
    } else {
        cout << "Change saved in memory only (not written to CSV file).\n";
    }
}

ResumeNode* ResumeLinkedList::findResumeByID(int id) {
    ResumeNode* current = head;
    while (current) {
        if (current->resumeID == id)
            return current;
        current = current->next;
    }
    return nullptr; // not found
}
