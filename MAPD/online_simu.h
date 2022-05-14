//
// Created by Zhe Chen on 1/12/20.
//
#include "TaskAssignment.h"

#ifndef MAPD_ONLINE_SIMU_H
#define MAPD_ONLINE_SIMU_H
#include <chrono>

struct AgentStatus
{
    Agent *agent;
    int currentLoc;
    std::unordered_set<Task *> currentLoads;
    int prevAction = -1;
};

class OnlineSimu
{
public:
    OnlineSimu(TaskAssignment *ta, TaskLoader *tl, AgentLoader *al, MapLoader *ml, bool is_batched);
    bool simulate(bool anytime = false);
    void initializeAssignments();
    void setTasks(int timestep);
    bool updateAgentStatus(int timestep);
    bool haveCollision();

    std::chrono::duration<double, std::milli> runtime;
    double runtime_ta = 0;
    std::chrono::time_point<std::chrono::steady_clock> run_start;

    double getRuntime()
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(runtime).count();
    }

    std::vector<Batch *> getFinishedBatches() { return finished_batches; }
    double getAverageBLE()
    {
        if (getFinishedBatches().size() == 0)
            return 0;
        int total_ble = 0;
        for (auto b : getFinishedBatches())
        {
            total_ble += b->ble;
        }
        double able = total_ble / (double)getFinishedBatches().size();
        return able;
    }

    double getAverageBatchServiceTime()
    {
        int total = 0;
        int size = getFinishedBatches().size();
        for (auto b : getFinishedBatches())
        {
            // Get a rolling batch service time
            total += b->get_service_time();
        }
        if (size == 0)
            return 0;

        return total / (double)size;
    }
    double getTotalBLE()
    {
        int total_ble = 0;
        for (auto b : getFinishedBatches())
        {
            total_ble += b->ble;
        }
        return total_ble;
    }
    double getBowe()
    {
        double bst = getAverageBatchServiceTime();
        double ble = getAverageBLE();
        return (ble + 1) * bst;
    }

private:
    TaskAssignment *taskAssignment;
    TaskLoader *taskLoader;
    AgentLoader *agentLoader;
    MapLoader *mapLoader;

    bool is_batched = false;
    int current_batch_index = 0;

    vector<AgentStatus> agentStatus;
    vector<vector<Task *>> taskQueue;
    std::unordered_set<int> unfinished_tasks;
    std::unordered_set<int> awaiting_tasks;
    std::unordered_set<int> ongoing_tasks;
    std::unordered_set<int> finished_tasks;
    std::vector<Batch *> finished_batches;
};

#endif // MAPD_ONLINE_SIMU_H
