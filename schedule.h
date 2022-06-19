#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <ctime>
#include <queue>
#include <chrono>

#include "job.h"

using sctime = std::chrono::time_point<std::chrono::steady_clock>;

class JobQueue {
   public:
    void clear();
    int size();
    void insert(const Job&, sctime t);
    Job top() const;
    void pop();

   private:
    struct JobNode {
        Job job;
        sctime t;
        JobNode(Job job = {}, sctime t = {}) : job(job), t(t) {}
        bool operator<(const JobNode& rhs) const;
    };
    std::priority_queue<JobNode> q;
};

#endif
