#ifndef RESUME_ARRAY_H
#define RESUME_ARRAY_H

#include <string>

struct Resume {
    std::string id;
    std::string name;
    std::string skills;
    std::string experience;
};

class ResumeArray {
private:
    Resume *resumes;
    int resumesCount;
    int resumesCapacity;
    std::string csvFilename; // store filename used to load/save temp CSV

    void ensureCapacity(int minCapacity);

public:
    ResumeArray();
    ~ResumeArray();

    // 1. Load resumes from CSV
    void loadResumes(const std::string& filename);

    // Save resumes back to CSV (will write to the provided filename)
    void saveToCSV(const std::string &filename);

    bool confirmAction(const std::string &message);

    // 2. Match with other resumes (otherResumes is raw array)
    void matchResumes(const Resume *otherResumes, int otherCount);

    // 3. Input resume number and match 3 jobs
    void matchThreeJobs(int resumeIndex);

    // 4. Add new resume
    void addResume(const Resume& newResume);
    
    // Interactive add record (matching linked list interface)
    void addRecord();

    // 5. Delete resume (head, middle, tail)
    void deleteResume(int position);

    // Utility
    int getSize() const { return resumesCount; }
    const Resume& getResume(int index) const { return resumes[index]; }
    void printResumes(int count = 5) const;
};

#endif