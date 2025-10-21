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
    string csvFilename;  // store the CSV filename for saving

public:
    ResumeLinkedList();
    ~ResumeLinkedList();

    void insertAtEnd(const string &desc);
    void loadFromCSV(const string &filename);
    void saveToCSV(const string &filename);  // save current list to CSV
    void clear();
    int size() const;
    ResumeNode *getHead() const;
    void display() const;
    
    // New functions with confirmation and CSV update
    void addRecord();     // add new resume record (prompts user for input)
    void deleteFromHead();                  // delete first record
    void deleteFromMiddle(int position);    // delete from middle (1-based index)
    void deleteFromTail();                  // delete last record
    
    // Helper functions
    bool confirmAction(const string &message);  // ask user confirmation
    ResumeNode* findResumeByID(int id);
};

#endif
