#ifndef JOB_LINKEDLIST_H
#define JOB_LINKEDLIST_H

#include <string>
using namespace std;

struct JobNode {
    int jobID;                 // auto ID
    string title;              // title before "needed with experience"
    string description;        // full text
    string keywords[10];       // max 10 keywords
    JobNode *next;
};

class JobLinkedList {
private:
    JobNode *head;
    JobNode *tail;
    int count;

public:
    JobLinkedList();
    ~JobLinkedList();

    void insertAtEnd(const string &desc);  // only description
    void loadFromCSV(const string &filename);
    void clear();
    int size() const;
    JobNode *getHead() const;
    void display() const;
};

#endif
