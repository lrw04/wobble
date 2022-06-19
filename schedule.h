#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <ctime>
#include <queue>

#include "job.h"

class JobQueue {
   public:
    void clear();
    int size();
    void insert(const Job&, time_t t);
    Job top() const;
    void pop();

   private:
    struct JobNode {
        Job job;
        time_t t;
        JobNode(Job job = {}, time_t t = 0) : job(job), t(t) {}
        bool operator<(const JobNode& rhs) const;
    };
    std::priority_queue<JobNode> q;
};

#endif
