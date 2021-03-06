#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <chrono>
#include <ctime>
#include <queue>
#include <random>
#include <utility>

#include "job.h"

using sctime = std::chrono::time_point<std::chrono::steady_clock>;

class JobQueue {
   public:
    JobQueue(int cycle);
    void clear();
    std::size_t size();
    void insert(const Job&, sctime t);
    std::pair<Job, sctime> top() const;
    void pop();
    void reenter();

   private:
    struct JobNode {
        Job job;
        sctime t;
        std::chrono::milliseconds d;
        bool operator<(const JobNode& rhs) const;
    };
    int c;
    std::priority_queue<JobNode> q;
    std::random_device rd;
    std::mt19937_64 rng;
    std::uniform_int_distribution<int> rdist;
};

#endif
