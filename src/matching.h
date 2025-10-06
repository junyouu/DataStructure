#ifndef MATCHING_H
#define MATCHING_H

#include "job_linkedlist.h"
#include "resume_linkedlist.h"

class Matcher {
public:
    static void findTopMatches(JobLinkedList &jobs, ResumeLinkedList &resumes);
    
    // New function: match 3 resumes for a specific job ID
    static void matchTop3ForJob(int jobID, JobLinkedList &jobs, ResumeLinkedList &resumes);
};

#endif
