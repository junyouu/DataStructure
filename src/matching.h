#ifndef MATCHING_H
#define MATCHING_H

#include "job_linkedlist.h"
#include "resume_linkedlist.h"
#include "job_array.h"
#include "resume_array.h"

class Matcher {
public:
    /* ============================
       LINKED LIST VERSION
       ============================ */
    static void findTopMatchesLinkedList(JobLinkedList &jobs, ResumeLinkedList &resumes);
    static void matchTop3ForJobLinkedList(int jobID, JobLinkedList &jobs, ResumeLinkedList &resumes);
    static void searchByKeywordLinkedList(ResumeLinkedList &resumes);

    /* ============================
       ARRAY VERSION
       ============================ */
    static void findTopMatchesArray(JobArray &jobs, ResumeArray &resumes);
    static void matchTop3ForJobArray(int jobIndex, JobArray &jobs, ResumeArray &resumes);
    static void searchByKeywordArray(ResumeArray &resumes);

    static void matchTop3ForResumeLinkedList(int resumeID, JobLinkedList &jobs, ResumeLinkedList &resumes);
    static void matchTop3ForResumeArray(int resumeIndex, JobArray &jobs, ResumeArray &resumes);

};

#endif
