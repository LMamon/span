#include <span/mission.hpp>
#include <span/astar.hpp>

#include <iostream>

namespace span {
    Mission::Mission(MissionType type) : type_(type) {}

    void Mission::configure(const Grid& grid, std::vector<Team>& teams) {
        goals_.clear();

        switch (type_) {
            case MissionType::Path: {
                Position goal = random_position(grid);
                goals_.push_back(goal);

                for (std::size_t i = 0; i < teams.size(); ++i) {
                    teams[i].id = i + 1;
                    teams[i].assign_goal(goal);
                }
                break;
            }

            case MissionType::Distribution: {
                for (std::size_t i = 0; i < teams.size(); ++i) {
                    Position goal = random_position(grid);
                    goals_.push_back(goal);
                    teams[i].id = i + 1;
                    teams[i].assign_goal(goal);
                }
                break;
            }

            case MissionType::Coverage: {
                for (std::size_t i = 0; i < teams.size(); ++i) {
                    teams[i].id = i + 1;
                }
                std::cout << "coverage mission is not implemented yet\n";
                break;
            }
        }
    }

    void Team::route(const Grid& grid, PlannerType planner) {
        switch (planner) {
            case PlannerType::Astar:
                for (Agent& agent : agents) {
                    Astar plan(grid, agent.position(), goal);
                    agent.set_path(plan.plan());
                }
                break;

            case PlannerType::Rrt:
                std::cout << "RRT planner is not implemented yet\n";
                break;

            case PlannerType::Rrtstar:
                std::cout << "RRT* planner is not implemented yet\n";
                break;
        }
    }
}
