//
// Created by Zhe Chen on 10/6/20.
//

#ifndef LIFELONG_MAPF_TaskAssignment_H
#define LIFELONG_MAPF_TaskAssignment_H

#include "Agent.h"
#include "Task.h"
#include "PathPlanning.h"
#include "Assignment.h"
#include "ConstraintTable.h"
#include <unordered_set>
#include <unordered_map>
#include "common.h"
#include "SinglePlanning.h"
#include <unordered_set>

extern int map_cols;
extern int screen;

struct ite_log
{
    int iteration;
    int g_size;
    double runtime;
    int final_cost;
};

class TaskAssignment
{
public:
    virtual void initialize_heaps(){};
    virtual Assignment *get_best_assignment(){};
    virtual void delete_top(int task_id){};
    virtual void updateAllAssignmentHeap(Agent *updatedAgent, Task *assignedTask){};
    virtual void logAssignmentHandle(Task *task, Agent *a, AssignmentHeap *new_assignment_heap){};
    virtual bool updateRealPath(Assignment *min_cost_assign, Agent *a, int next_action = 1){};
    virtual void buildAssignmentHeap();
    virtual void printTaskHeap(){};

    TaskAssignment(AgentLoader *agents, TaskLoader *tasks, MapLoaderCost *map, TA_Options ta_opt, options pl_option, bool real_cost, float pl_time_limit);
    bool assignTasks();
    void initializeOneShot();
    void initializeOnline();
    void initializeAssignments();
    void addToUnassigned(Task *t)
    {
        t->heap_handle = unassigned_tasks.push(t);
    }
    void setStartTimestep(int timestep)
    {
        start_timestep = timestep;
    }

    // optimize part
    std::unordered_set<Task *> remove_hist;

    bool optimize(std::unordered_set<int> awaiting_tasks = std::unordered_set<int>());
    bool selectRemoveRepair(std::unordered_set<int> &awaiting_tasks);
    bool removeRandom(std::unordered_set<int> &awaiting_tasks);
    bool removeFromMax(std::unordered_set<int> &awaiting_tasks);
    bool removeMultiMax(std::unordered_set<int> &awaiting_tasks);

    bool isValidActions(vector<ActionEntry> &actions, Agent *a);
    bool updateActions(vector<ActionEntry> &actions, Agent *agent, int pick = 0, int drop = -1);
    void printAssignments();
    int get_num_agents_with_tasks();

    Assignment *insertTask(Agent *a, Task *task, bool real_cost = false);
    int haveConflict(int agent_id, vector<PathEntry> &path);
    int prioritized_planning(vector<PathEntry> &path, vector<ActionEntry> &actions, Agent *a, int next_action);

    vector<Assignment> assignments;

    std::unordered_map<int, vector<AssignmentHeap_handle>> handleTable;
    std::unordered_set<Task *> assignedTasks;
    ConstraintTable constraintTable;

    bool first_build = true;

    double runtime = 0;
    float planning_time_limit;
    bool real_cost;
    int current_makespan = 0;
    int current_total_delay = 0;
    int current_total_service_time = 0;
    int current_total_batch_service_time = 0;
    int current_min_batch_service_time = INT_MAX;
    int current_max_batch_service_time = 0;
    int start_timestep = 0;

    double runtime_building_heap = 0;
    double runtime_update_heap = 0;
    double runtime_pp = 0;
    double runtime_update_conflict = 0;
    double runtime_update_changed_agent = 0;

    int num_conflict_updates = 0;
    int num_task_assign_updates = 0;

    int num_of_pp = 0;


    void writePath(std::string filepath="output.yaml") const {
        // Output the agent paths on yaml format
        fstream file;
        file.open(filepath, fstream::out);
        file << "schedule:" << endl;
        for (int agent = 0; agent < agents->agents.size(); agent++) {
            file << "  agent" << agent<< ":" << endl;

            for (auto &entry : assignments[agent].path)
            {
                // Output, removing the border (-1)
                file << "    - x: " << entry.location / (map->cols) - 1 << endl;
                file << "      y: " << entry.location % (map->cols) - 1 << endl;
                file << "      t: " << entry.timeStep << endl;

            }
        }
    }

    void writeTasks(std::string filepath="task_output.yaml") const {
        fstream file;
        file.open(filepath, fstream::out);
        file << "tasks:" << endl;
        for (int t = 0; t < tasks->all_tasks_vec.size(); t++) {
            Task* task = tasks->all_tasks_vec[t];
            file << "   task" << t << ":" << endl;

            file << "   - start: " << task->started_time << endl;
            file << "     finish: " << task->finished_time << endl;
            file << "     batch: " << task->batch_id << endl;
        }
    }

    void printPath()
    {
        for (int agent_id = 0; agent_id < agents->agents.size(); agent_id++)
        {
            cout << "Agent: " << agent_id << ", Delay: " << assignments[agent_id].current_total_delay << " , Cost: " << assignments[agent_id].path.size() - 1 << ", plan: ";
            for (auto &entry : assignments[agent_id].path)
            {
                cout << entry.timeStep << "(" << entry.location / map->cols << "," << entry.location % map->cols << ")->";
            }
            cout << endl;
        }
    }

    vector<ite_log> iteration_log;

protected:
    AgentLoader *agents;
    TaskLoader *tasks;
    MapLoaderCost *map;
    TaskHeap unassigned_tasks;
    TA_Options ta_option;
    options pl_option;
    bool no_task_loop = false;
    bool improving = false;
    std::unordered_set<Task *> selected;

    std::clock_t start;
};

#endif //LIFELONG_MAPF_TaskAssignment_H
