//
// Created by Zhe Chen on 10/6/20.
//

#include "Task.h"
#include <sstream>

Task::Task(int id, int init_t, int init_loc, int goal_loc, int bid)
{
    task_id = id;
    initial_time = init_t;
    initial_location = init_loc;
    goal_location = goal_loc;
    batch_id = bid;
    finished = false;
    started_time = -1;
}

TaskLoader::TaskLoader(const std::string fname, MapLoaderCost &ml)
{
    string line;

    ifstream myfile(fname.c_str());

    if (myfile.is_open())
    {
        getline(myfile, line);
        char_separator<char> sep(",");
        tokenizer<char_separator<char>> tok(line, sep);
        tokenizer<char_separator<char>>::iterator beg = tok.begin();
        this->num_of_tasks = atoi((*beg).c_str());
        //    cout << "#AG=" << num_of_agents << endl;
        for (int i = 0; i < num_of_tasks; i++)
        {
            getline(myfile, line);
            tokenizer<char_separator<char>> col_tok(line, sep);
            tokenizer<char_separator<char>>::iterator c_beg = col_tok.begin();
            pair<int, int> curr_pair;
            // read start [row,col] for agent i
            curr_pair.first = atoi((*c_beg).c_str());
            c_beg++;
            curr_pair.second = atoi((*c_beg).c_str());
            int init_loc = curr_pair.first * ml.cols + curr_pair.second;

            //      cout << "AGENT" << i << ":   START[" << curr_pair.first << "," << curr_pair.second << "] ; ";
            // read goal [row,col] for agent i
            c_beg++;
            curr_pair.first = atoi((*c_beg).c_str());
            c_beg++;
            curr_pair.second = atoi((*c_beg).c_str());
            int goal_loc = curr_pair.first * ml.cols + curr_pair.second;

            c_beg++;
            int init_time = atoi((*c_beg).c_str());
            Task *new_task = new Task(i, init_time, init_loc, goal_loc, -1); // Last var batch id is not used here, hence -1
            new_task->ideal_end_time = ml.getDistance(init_loc, goal_loc) + init_time;
            all_tasks.push(new_task);
        }
        myfile.close();
    }
}

void TaskLoader::loadKiva(const std::string fname, MapLoaderCost &ml, bool is_batched)
{
    string line;
    ifstream myfile(fname.c_str());
    if (!myfile.is_open())
    {
        assert(false);
        cerr << "Task file not found." << endl;
        return;
    }
    // read file
    stringstream ss;
    int task_num;
    getline(myfile, line);
    ss << line;
    ss >> task_num; // number of tasks
    int current_batch = -1;
    this->num_of_tasks = task_num;
    for (int i = 0; i < task_num; i++)
    {
        int t, s, g, ts, tg, batch_id;
        getline(myfile, line);
        ss.clear();
        ss << line;
        ss >> t >> s >> g >> ts >> tg; // time+start+goal+time at start+time at goal+batch
        if (is_batched) {
            ss >> batch_id;
        } else {
            batch_id = 0;
        }
        s = s % ml.endpoints.size();
        g = g % ml.endpoints.size();

        assert(s < ml.endpoints.size());
        assert(g < ml.endpoints.size());
        Task *new_task = new Task(i, t, ml.endpoints[s], ml.endpoints[g], batch_id);
        new_task->ideal_end_time = ml.getDistance(ml.endpoints[s], ml.endpoints[g]) + t;

        if (t > last_release_time)
        {
            last_release_time = t;
        }
        all_tasks.push(new_task);
        all_tasks_vec.push_back(new_task);

        if (is_batched) {
            // Add batches only if there is no batch with that id yet, otherwise add task to the batch list
            if (batch_id > current_batch)
            {
                current_batch++; // If the batch ids are non-linear, this will still result in internal batch ids of 0, 1, 2, ..., n batches
                // Create a new batch
                all_batches.push_back(new Batch(batch_id));
            }
            // add task to batch list
            all_batches[current_batch]->add_task(new_task);
        }
    }
    myfile.close();
}

Batch::Batch(int id)
{
    batch_id = id;
    finished_time = -1;
    initial_time = -1;
}

void Batch::add_task(Task *t)
{
    if (initial_time == -1)
    {
        initial_time = t->initial_time;
    }
    tasks.push_back(t);
}

void Batch::try_finish()
{
    if (!is_finished())
        return; // Not finished yet
    // Get the finish time
    for (auto t : tasks)
    {
        if (t->finished_time > finished_time)
        {
            finished_time = t->finished_time;
        }
    }
}

bool Batch::is_finished()
{
    for (auto t : tasks)
    {
        if (!t->finished)
            return false;
    }
    return true;
}

int Batch::get_service_time()
{
    // Loop through all tasks
    int total = 0;
    for (auto t : tasks)
    {
        total += t->get_service_time();
    }
    return total;
}