#include "utility.h"
#include <fstream>
#include <iostream>
#include <windows.h>
#include <psapi.h>

using namespace std;

bool copyFile(const std::string &src, const std::string &dst) {
    std::ifstream in(src, std::ios::binary);
    if (!in) return false;

    std::ofstream out(dst, std::ios::binary);
    if (!out) return false;

    out << in.rdbuf();

    in.close();
    out.close();
    return true;
}

void displayMenu() {
    cout << "\n========================================\n";
    cout << "   Job & Resume Matching System\n";
    cout << "========================================\n";
    cout << "1. Match Top 3 Resumes for a Job ID\n";
    cout << "2. Add New Record (Job/Resume)\n";
    cout << "3. Delete Record (Head/Middle/Tail)\n";
    cout << "4. Display All Jobs\n";
    cout << "5. Display All Resumes\n";
    cout << "6. Run Full Matching (All Jobs)\n";
    cout << "0. Exit\n";
    cout << "========================================\n";
    cout << "Enter your choice: ";
}

double getCurrentMemoryKB() {
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024.0;
    }
    return 0.0;
}