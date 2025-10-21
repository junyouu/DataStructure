#ifndef JOB_ARRAY_H
#define JOB_ARRAY_H

#include <string>
#include <stdexcept>
using namespace std;

struct Job {
    int jobID;                // auto ID (same as JobNode)
    string title;             // title before "needed with experience"
    string description;       // full text
    string keywords[10];      // max 10 keywords
};

class JobArray {
private:
    Job *jobs;                // dynamic array of Job
    int jobsCount;            // number of jobs currently stored
    int jobsCapacity;         // allocated capacity
    string csvFilename;       // store CSV filename for saving

    // Ensure capacity for at least minCapacity elements
    void ensureCapacity(int minCapacity);

public:
    JobArray();
    ~JobArray();

    // Insert new job at end (similar to insertAtEnd in linked list)
    void insertAtEnd(const string &desc);

    // Load & save
    void loadFromCSV(const string &filename);
    void saveToCSV(const string &filename);

    // Utility
    void clear();
    int getSize() const;
    const Job* getArray() const { return jobs; } // optional getter for raw array
    void display() const;
    const Job& getJob(int index) const;

    // Add/delete operations similar to linked list
    void addRecord();                 // add new job record (prompts user input)
    void deleteFromHead();            // delete first record
    void deleteFromMiddle(int position); // delete from middle (1-based index)
    void deleteFromTail();            // delete last record
    Job* findJobByID(int jobID);      // find job by ID

    // Helper functions
    bool confirmAction(const string &message);
};

#endif
