#include "schedule.h"

JobQueue::JobQueue(int cycle) {
    c = cycle;
    rng.seed(rd());
    rdist = std::uniform_int_distribution<int>(0, c);
}

void JobQueue::clear() {
    while (!q.empty()) q.pop();
}

int JobQueue::size() { return q.size(); }

void JobQueue::insert(const Job& j, sctime t) {
    q.push({j, t, std::chrono::seconds(rdist(rng))});
}

std::pair<Job, sctime> JobQueue::top() const {
    return {q.top().job, q.top().t + q.top().d};
}

void JobQueue::pop() { q.pop(); }

void JobQueue::reenter() {
    auto t = q.top();
    q.pop();
    this->insert(t.job, t.t + std::chrono::seconds(c));
}

bool JobQueue::JobNode::operator<(const JobQueue::JobNode& rhs) const {
    return t + d > rhs.t + rhs.d;
}
