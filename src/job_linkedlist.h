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
    string csvFilename;  // store the CSV filename for saving

public:
    JobLinkedList();
    ~JobLinkedList();

    void insertAtEnd(const string &desc);  // only description
    void loadFromCSV(const string &filename);
    void saveToCSV(const string &filename);  // save current list to CSV
    void clear();
    int size() const;
    JobNode *getHead() const;
    void display() const;
    
    // New functions with confirmation and CSV update
    void addRecord(const string &desc);  // add new job record
    void deleteFromHead();               // delete first record
    void deleteFromMiddle(int position); // delete from middle (1-based index)
    void deleteFromTail();               // delete last record
    JobNode* findJobByID(int jobID);     // find job by ID
    
    // Helper functions
    bool confirmAction(const string &message);  // ask user confirmation
};

#endif
