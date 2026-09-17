#include <span/agent.hpp>
#include <span/core.hpp>
#include <span/astar.hpp>

#include <string>

namespace span {
    Agent::Agent(Position position) : start_(position), position_(position) {}

    std::vector<Team> create_teams(std::vector<Agent> agents) {
        std::vector<Team> teams;

        for (std::size_t i = 0; i < agents.size(); ++i) {
            if (i % MAX_TEAM_SIZE == 0) teams.emplace_back();

            teams.back().agents.push_back(std::move(agents[i]));
        }
        return teams;
    }
    
    // TODO
    void Agent::update(const Grid& grid) {
        // check for free neighbors 
        if (path_index_ >= path_.size()) return;
        // decide next position
        // record visited cell
        visited_.insert(position_);
        
        // update position_
        position_ = path_[path_index_++];
    }

    void Agent::set_path(std::vector<Position> path) {
        path_ = std::move(path);
        
        if (!path_.empty() && path_.front() == position_) {
            path_index_ = 1;
        } else {
            path_index_ = 0;
        }
    }

}