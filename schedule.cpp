#include "schedule.h"

void JobQueue::clear() {
    while (!q.empty()) q.pop();
}

int JobQueue::size() { return q.size(); }

void JobQueue::insert(const Job& j, time_t t) { q.emplace(j, t); }

Job JobQueue::top() const { return q.top().job; }

void JobQueue::pop() { q.pop(); }

bool JobQueue::JobNode::operator<(const JobQueue::JobNode& rhs) const {
    return t > rhs.t;
}
