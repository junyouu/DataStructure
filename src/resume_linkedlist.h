#ifndef RESUME_LINKEDLIST_H
#define RESUME_LINKEDLIST_H

#include <string>
using namespace std;

struct ResumeNode {
    int resumeID;          // auto ID
    string description;    // full text
    string keywords[10];   // max 10 extracted keywords
    ResumeNode *next;
};

class ResumeLinkedList {
private:
    ResumeNode *head;
    ResumeNode *tail;
    int count;

public:
    ResumeLinkedList();
    ~ResumeLinkedList();

    void insertAtEnd(const string &desc);
    void loadFromCSV(const string &filename);
    void clear();
    int size() const;
    ResumeNode *getHead() const;
    void display() const;
    
    // New functions
    void addRecord(const string &desc);     // add new resume record
    void deleteFromHead();                  // delete first record
    void deleteFromMiddle(int position);    // delete from middle (1-based index)
    void deleteFromTail();                  // delete last record
};

#endif
