#include "job_linkedlist.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

JobLinkedList::JobLinkedList() {
    head = tail = nullptr;
    count = 0;
}

JobLinkedList::~JobLinkedList() {
    clear();
}


void JobLinkedList::insertAtEnd(const string &id, const string &title, const string &desc) {
    JobNode *node = new JobNode();
    node->jobID = id;
    node->title = title;
    node->description = desc;
    node->next = nullptr;

    if (!head) { // first node
        head = tail = node;
    } else {
        tail->next = node;
        tail = node;
    }
    count++;
}

int JobLinkedList::size() const {
    return count;
}

void JobLinkedList::clear() {
    while (head) {
        JobNode *temp = head;
        head = head->next;
        delete temp;
    }
    tail = nullptr;
    count = 0;
}

void JobLinkedList::loadFromCSV(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Cannot open " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // skip header line

    while (getline(file, line)) {
        stringstream ss(line);
        string id, title, desc;

        getline(ss, id, ',');
        getline(ss, title, ',');
        getline(ss, desc); // remaining text

        // Remove potential quotation marks or whitespace
        if (!id.empty() && id.front() == '"') id.erase(0, 1);
        if (!id.empty() && id.back() == '"') id.pop_back();
        if (!title.empty() && title.front() == '"') title.erase(0, 1);
        if (!title.empty() && title.back() == '"') title.pop_back();
        if (!desc.empty() && desc.front() == '"') desc.erase(0, 1);
        if (!desc.empty() && desc.back() == '"') desc.pop_back();

        insertAtEnd(id, title, desc);
    }

    file.close();
}

JobNode *JobLinkedList::getHead() const {
    return head;
}