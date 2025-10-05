#include <string>
using namespace std;

struct JobNode {
    string jobID;
    string title;
    string description;
    string keywords[50]; 
    int keywordCount = 0;
    JobNode *next;
};

class JobLinkedList {
private:
    JobNode *head;
    JobNode *tail;
    int count;

public:
    JobLinkedList();
    ~JobLinkedList();

    void insertAtEnd(const string &id, const string &title, const string &desc);
    int size() const;
    void clear();
    void loadFromCSV(const string &filename);
    JobNode *getHead() const;
};