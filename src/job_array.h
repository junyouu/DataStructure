#ifndef JOB_ARRAY_H
#define JOB_ARRAY_H

#include <string>
#include <stdexcept>

struct Job {
    std::string id;
    std::string title;
    std::string description;
    std::string location;
    double salary; // optional salary field
    std::string keywords[10];
};

class JobArray {
private:
    Job *jobs;         // dynamic array of Job
    int jobsCount;     // number of jobs currently stored
    int jobsCapacity;  // allocated capacity
    std::string csvFilename; // stored filename for save operations

    // ensure capacity for at least minCapacity elements
    void ensureCapacity(int minCapacity);

public:
    JobArray();
    ~JobArray();

    // Load jobs from CSV file
    void loadJobs(const std::string& filename);

    // Match with other jobs (otherJobs is a raw array with otherCount elements)
    void matchJobs(const Job *otherJobs, int otherCount);

    // Input a job index/ID and match 3 resumes (simulation)
    void matchThreeResumes(int jobIndex);
    // New: match top 3 resumes for a given job using ResumeArray
    void matchTop3ForJobWithResumes(int jobIndex, const class ResumeArray &resumes);
    // Run full matching for all jobs and print top 3 resumes per job
    void findTopMatchesWithResumes(const class ResumeArray &resumes);

    // Add new job record
    void addJob(const Job& newJob);
    // Interactive add/delete/save operations (array-mode)
    void addRecord();
    void saveToCSV(const std::string &filename);
    bool confirmAction(const std::string &message);

    // Delete operations similar to linked list
    void deleteFromHead();
    void deleteFromMiddle(int position); // 1-based
    void deleteFromTail();

    // Delete job record (by index)
    void deleteJob(int position);

    // Utility
    int getSize() const { return jobsCount; }

    // Returns reference to job at index; throws std::out_of_range when invalid
    const Job& getJob(int index) const {
        if (index < 0 || index >= jobsCount) throw std::out_of_range("JobArray::getJob index out of range");
        return jobs[index];
    }

    void printJobs(int count = 5) const; // preview
};

#endif
