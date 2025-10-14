#ifndef RESUME_ARRAY_H
#define RESUME_ARRAY_H

#include <string>
#include <vector>
using namespace std;

struct Resume {
    string id;
    string name;
    string skills;
    string experience;
};

class ResumeArray {
private:
    vector<Resume> resumes;

public:
    // 1. Load resumes from CSV
    void loadResumes(const string& filename);

    // 2. Match with job data (dummy version)
    void matchResumes(const vector<Resume>& otherResumes);

    // 3. Input resume number and match 3 jobs
    void matchThreeJobs(int resumeIndex);

    // 4. Add new resume
    void addResume(const Resume& newResume);

    // 5. Delete resume (head, middle, tail)
    void deleteResume(int position);

    // Utility
    int getSize() const { return resumes.size(); }
    const Resume& getResume(int index) const { return resumes[index]; }
    void printResumes(int count = 5) const;
};

#endif