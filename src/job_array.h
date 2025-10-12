#ifndef JOB_ARRAY_H
#define JOB_ARRAY_H

#include <string>
#include <vector>
#include <stdexcept>
using namespace std;

struct Job {
    string id;
    string title;
    string description;
    string location;
    double salary; // optional salary field
};

class JobArray {
private:
    vector<Job> jobs;   // dynamic array

public:
    JobArray() = default;

    // Load jobs from CSV
    void loadJobs(const string& filename);

    // Match with other jobs (example for comparing efficiency)
    void matchJobs(const vector<Job>& otherJobs);

    // Input a job index/ID and match 3 resumes (simulation)
    void matchThreeResumes(int jobIndex);

    // Add new job record
    void addJob(const Job& newJob);

    // Delete job record (by index)
    void deleteJob(int position);

    // Utility
    int getSize() const { return static_cast<int>(jobs.size()); }

    // Returns reference to job at index; throws std::out_of_range when invalid
    const Job& getJob(int index) const {
        if (index < 0 || index >= getSize()) throw std::out_of_range("JobArray::getJob index out of range");
        return jobs[index];
    }

    void printJobs(int count = 5) const; // preview
};

#endif
