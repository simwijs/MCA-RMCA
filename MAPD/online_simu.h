//
// Created by Zhe Chen on 1/12/20.
//
#include "TaskAssignment.h"

#ifndef MAPD_ONLINE_SIMU_H
#define MAPD_ONLINE_SIMU_H
#include <chrono>

struct AgentStatus{
    Agent* agent;
    int currentLoc;
    std::unordered_set<Task *> currentLoads;
    int prevAction = -1;

};

class OnlineSimu{
public:
    OnlineSimu(TaskAssignment* ta, TaskLoader* tl, AgentLoader* al, MapLoader* ml, bool is_batched);
    bool simulate(bool anytime = false);
    void initializeAssignments();
    void setTasks(int timestep);
    bool updateAgentStatus(int timestep);
    bool haveCollision();

    std::chrono::duration<double, std::milli> runtime;
    double runtime_ta = 0;
    std::chrono::time_point<std::chrono::steady_clock>  run_start;

    double getRuntime() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(runtime).count();
    }

private:
    TaskAssignment* taskAssignment;
    TaskLoader* taskLoader;
    AgentLoader* agentLoader;
    MapLoader* mapLoader;

    bool is_batched = false;

    vector<AgentStatus> agentStatus;
    vector<vector<Task*>> taskQueue;
    std::unordered_set<int> unfinished_tasks;
    std::unordered_set<int> awaiting_tasks;
    std::unordered_set<int> ongoing_tasks;
    std::unordered_set<int> finished_tasks;

};

#endif //MAPD_ONLINE_SIMU_H
