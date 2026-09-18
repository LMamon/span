#pragma once

#include <span/grid.hpp>

#include <set>
#include <vector>

namespace span {
    static constexpr std::size_t MAX_TEAM_SIZE = 4;

    class Agent {
        public:
            explicit Agent(Position position);

            Position position() const { return position_; }
            Position start() const { return start_; }

            const std::set<Position>& visited() const { return visited_; }
            const std::vector<Position>& path() const { return path_; }

            bool finished() const { return path_index_ >= path_.size(); }
            void update(const Grid& grid);
            void set_path(std::vector<Position> path);

        private:
            Position position_;
            Position start_;
            Position goal_{0, 0, 0};
            std::vector<Position> path_;
            std::size_t path_index_ = 0;
            std::set<Position> visited_;
    };

    struct Team {
        std::size_t id = 0;
        std::vector<Agent> agents;
        Position goal{0, 0, 0};

        void assign_goal(Position position) {
            goal = position;
        }

        void route(const Grid& grid, PlannerType planner);
    };

    std::vector<Team> create_teams(std::vector<Agent> agents);
}
