#ifndef RESUME_ARRAY_H
#define RESUME_ARRAY_H

#include <string>
#include <stdexcept>
using namespace std;

struct Resume {
    int resumeID;            // auto ID
    string description;      // full text
    string keywords[10];     // max 10 extracted keywords
};

class ResumeArray {
private:
    Resume *resumes;         // dynamic array of Resume
    int resumesCount;        // number of resumes currently stored
    int resumesCapacity;     // allocated capacity
    string csvFilename;      // store filename used to load/save temp CSV

    void ensureCapacity(int minCapacity);  // increase capacity when needed

public:
    ResumeArray();
    ~ResumeArray();

    // Load and Save (same logic as LL version)
    void loadFromCSV(const string &filename);
    void saveToCSV(const string &filename);

    // Clear array
    void clear();

    // Size and access
    int size() const { return resumesCount; }
    const Resume& getResume(int index) const {
        if (index < 0 || index >= resumesCount) throw out_of_range("ResumeArray::getResume index out of range");
        return resumes[index];
    }

    // Display preview
    void display() const;

    // CRUD (same naming as linked list)
    void addRecord();                    // interactive add
    void deleteFromHead();               // delete first
    void deleteFromMiddle(int position); // delete from middle (1-based)
    void deleteFromTail();               // delete last

    // Confirmation utility
    bool confirmAction(const string &message);
};

#endif
