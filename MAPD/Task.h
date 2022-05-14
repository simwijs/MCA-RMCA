//
// Created by Zhe Chen on 10/6/20.
//

#include <vector>
#include "map_loader_with_cost.h"
#include "basic.h"
#include <boost/tokenizer.hpp>
#include <utility>
#include <iostream>
#include <fstream>

#ifndef LIFELONG_MAPF_TASK_H
#define LIFELONG_MAPF_TASK_H
using namespace ::std;
using namespace ::boost;
extern int map_cols;

class Task
{
public:
    Task(int id, int init_t, int init_loc, int goal_loc, int batch_id);

    int initial_time;
    int initial_location;
    int goal_location;
    int task_id;
    int batch_id;
    int ideal_end_time;
    int started_time; // not the same as initial time, as it may not have been picked up yet
    int finished_time;
    bool finished;
    TaskState state = TaskState::NONE;

    struct compare_task
    {
        // returns true if t1 > t2 (note -- this gives us *min*-heap).
        bool operator()(const Task *t1, const Task *t2) const
        {
            if (t1->initial_time >= t2->initial_time)
                return true;
            else
                return false;
        }
    };

    int get_service_time() const
    {
        return finished_time - initial_time;
    }

    void finish(int timestep)
    {
        finished_time = timestep;
        finished = true;
    }

    typedef boost::heap::fibonacci_heap<Task *, boost::heap::compare<Task::compare_task>>::handle_type TaskHeap_handle;
    TaskHeap_handle heap_handle;
};

typedef boost::heap::fibonacci_heap<Task *, boost::heap::compare<Task::compare_task>> TaskHeap;

class Batch
{
public:
    Batch(int id);

    int initial_time;
    int batch_id;
    int ideal_end_time;
    int finished_time;
    int ble;
    vector<Task *> tasks;

    struct compare_Batch
    {
        // returns true if t1 > t2 (note -- this gives us *min*-heap).
        bool operator()(const Batch *t1, const Batch *t2) const
        {
            if (t1->initial_time >= t2->initial_time)
                return true;
            else
                return false;
        }
    };

    void add_task(Task *t);
    bool is_finished();
    void try_finish();
    int get_service_time();
};

class TaskLoader
{
public:
    TaskLoader(const std::string fname, MapLoaderCost &ml);
    TaskLoader(){};
    void loadKiva(const std::string fname, MapLoaderCost &ml, bool is_batched);
    int num_of_tasks;
    int last_release_time = 0;
    TaskHeap all_tasks;
    vector<Task *> all_tasks_vec;
    vector<Batch *> all_batches;
    void printTasks()
    {
        for (auto t : all_tasks)
        {
            cout << "Task:" << t->task_id << ", Batch: " << t->batch_id << ", release_time:" << t->initial_time
                 << ", Initial:(" << t->initial_location / map_cols << "," << t->initial_location % map_cols << "),"
                 << " Goal :(" << t->goal_location / map_cols << "," << t->goal_location % map_cols << ")," << endl;
        }
    }

    ~TaskLoader()
    {
        for (auto t : all_tasks)
        {
            delete t;
        }

        for (auto b : all_batches)
        {
            delete b;
        }
    }
};

#endif // LIFELONG_MAPF_TASK_H
