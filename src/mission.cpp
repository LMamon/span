#include <span/mission.hpp>
#include <span/agent.hpp>
#include <span/grid.hpp>
#include <span/core.hpp>
#include <span/astar.hpp>


namespace span {
    Mission::Mission(MissionType type) : type_(type) {}

    void Mission::configure(const Grid& grid, std::vector<Team>& teams) {
        goals_.clear();

        // generate objectives
        switch (type_) {
            case MissionType::Path: {
                // create 1 goal
                Position goal = random_position(grid);
                goals_.push_back(goal);
                // assign a goal to each teams
                for (std::size_t i = 0; i < teams.size(); ++i) {
                    teams[i].id = i + 1;
                    teams[i].assign_goal(goal);
                }
                // get planner a/r/s
                break;
            }

            case MissionType::Distribution: {
                // create 1goal/team
                for (int i = 0; i < teams.size(); ++i) {
                    Position goal = random_position(grid);
                    goals_.push_back(goal);

                    teams[i].id = i + 1;
                    // assign team goals
                    teams[i].assign_goal(goal);
                }
                /* code */
                break;
            }
            
            case MissionType::Coverage: {
                // TODO: coverage strategy
                // Treat every unoccupied traversable cell as a goal.
                // Remove goals as cells are visited until none remain.
                /* code */
                break;
            }

            default:
                break;
        }
    }

    void Team::route(const Grid& grid, PlannerType planner) {
        // use planner to create path plan
        // create path plan

        switch (planner) {
            case PlannerType::Astar:
                for (Agent& agent : agents) { 
                    Position start = agent.position();

                    Astar plan(grid, start, goal);
                    agent.set_path(plan.plan());                    
                }
                /* code */
                break;
            
            default:
                break;
        }

    }
}