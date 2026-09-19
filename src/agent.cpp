#include <span/agent.hpp>

namespace span {
    Agent::Agent(Position position) : position_(position), start_(position) {}

    std::vector<Team> create_teams(std::vector<Agent> agents) {
        std::vector<Team> teams;

        for (std::size_t i = 0; i < agents.size(); ++i) {
            if (i % MAX_TEAM_SIZE == 0) {
                Team team;
                team.id = teams.size() + 1;
                teams.push_back(std::move(team));
            }

            teams.back().agents.push_back(std::move(agents[i]));
        }

        return teams;
    }

    void Agent::update(const Grid& grid) {
        (void)grid;
        if (path_index_ >= path_.size()) return;

        visited_.insert(position_);
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
