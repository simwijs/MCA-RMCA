//
// Created by Zhe Chen on 10/6/20.
//

#include "Agent.h"

Agent::Agent(int id, int initial_loc, int capacity)
{
    this->agent_id = id;
    this->initial_location = initial_loc;
    this->capacity = capacity;
}

AgentLoader::AgentLoader(const std::string fname, const MapLoaderCost &ml)
{
    string line;

    ifstream myfile(fname.c_str());

    if (myfile.is_open())
    {
        getline(myfile, line);
        char_separator<char> sep(",");
        tokenizer<char_separator<char>> tok(line, sep);
        tokenizer<char_separator<char>>::iterator beg = tok.begin();
        this->num_of_agents = atoi((*beg).c_str());
        //    cout << "#AG=" << num_of_agents << endl;
        for (int i = 0; i < num_of_agents; i++)
        {
            getline(myfile, line);
            tokenizer<char_separator<char>> col_tok(line, sep);
            tokenizer<char_separator<char>>::iterator c_beg = col_tok.begin();
            // read start [row,col] for agent i
            int row = atoi((*c_beg).c_str());
            c_beg++;
            int col = atoi((*c_beg).c_str());
            int loc = row * ml.cols + col;
            c_beg++;
            int capacity = atoi((*c_beg).c_str());
            agents.push_back(new Agent(i, loc, capacity));
        }
        myfile.close();
    }
}

void AgentLoader::loadKiva(const std::string fname, int capacity, const MapLoaderCost &ml)
{
    string line, value;
    ifstream myfile(fname.c_str());
    if (myfile.is_open())
    {
        stringstream ss;

        getline(myfile, line);
        ss << line;
        int rows, cols;
        ss >> rows >> cols;
        // Adjust for border
        // Endpoints
        getline(myfile, line);
        ss << line;

        // Agents
        ss.clear();
        getline(myfile, line);
        ss << line;
        int agents_in_file = 0;
        ss >> agents_in_file;

        // Unused
        ss.clear();
        getline(myfile, line);
        ss << line;
        ss.clear();

        // read map (and start/goal locations)
        num_of_agents = 0;
        for (int i = 0; i < rows; i++)
        {
            getline(myfile, line);
            for (int j = 0; j < cols; j++)
            {
                if (line[j] == 'r') // robot rest
                {
                    agents.push_back(new Agent(num_of_agents, (i + 1) * ml.cols + (j + 1), capacity));
                    num_of_agents++;
                }
            }
        }

        myfile.close();
        if (num_of_agents != agents_in_file)
        {
            cerr << "Agents specified in map=" << agents_in_file << " found 'r' agents in map=" << num_of_agents << " remember to not put agents on the border" << std::endl;
            exit(1);
        }
    }
    else
    {
        cerr << "file " << fname << " not found." << std::endl;
        exit(10);
    }
}