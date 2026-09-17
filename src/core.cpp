#include <span/core.hpp>
#include <span/grid.hpp>
#include <span/renderer.hpp>
#include <span/astar.hpp>
#include <span/agent.hpp>
#include <span/mission.hpp>

#include <iostream>
#include <thread>
#include <random>
#include <chrono>
#include <map>

namespace span {
    void Core::run(const volatile std::sig_atomic_t& stop_requested) {
        try {
            const GridConfig config = getInput();
            Grid grid(config);

            // Create agents.
            std::vector<Agent> agents;

            for (int i = 0; i < config.agent_count; ++i) {
                agents.emplace_back(random_position(grid));
            }

            // Group agents into teams.
            std::vector<Team> teams = create_teams(std::move(agents));
            std::map<Position, std::size_t> visited_by;

            // Configure mission and assign goals.
            Mission mission(config.mission_type);
            mission.configure(grid, teams);

            // Calculate routes.
            for (Team& team : teams) {
                team.route(grid, config.planner_type);
            }

            Renderer renderer;

            // Clear terminal once before animation.
            std::cout << "\033[2J\033[H";

            // Render initial state before any agents move.
            renderer.render(grid, teams, mission.goals(), visited_by);
            std::cout << std::flush;

            std::this_thread::sleep_for(std::chrono::milliseconds(200));

            while (!stop_requested) {
                // Advance every agent one step.
                for (Team& team : teams) {
                    for (Agent& agent : team.agents) {
                        if (!agent.finished()) {
                            Position visited = agent.position();

                            agent.update(grid);

                            visited_by[visited] = team.id;
                        }
                    }
                }

                // Redraw the same terminal frame.
                std::cout << "\033[H";
                renderer.render(grid, teams, mission.goals(), visited_by);
                std::cout << std::flush;

                // Check whether every agent has completed its route.
                bool all_finished = true;

                for (const Team& team : teams) {
                    for (const Agent& agent : team.agents) {
                        if (!agent.finished()) {
                            all_finished = false;
                            break;
                        }
                    }

                    if (!all_finished) break;
                }

                if (all_finished) break;

                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }

        } catch (const std::invalid_argument& e) {
            std::cerr << "error: " << e.what() << '\n';
        }
        
        (void)stop_requested;
    }

    GridConfig Core::getInput() {
        GridConfig config;

        std::cout << "columns (1-" << Grid::MAX_WIDTH << "): ";
        if (!(std::cin >> config.width)) throw std::invalid_argument("invalid column input"); 

        std::cout << "rows (1-" << Grid::MAX_HEIGHT << "): ";
        if (!(std::cin >> config.height)) throw std::invalid_argument("invalid row input");

        std::cout << "probability cell is blocked (0-1): ";
        if (!(std::cin >> config.blocked_probability)) throw std::invalid_argument("invalid probability input");

        char mission_input;
        std::cout << "mission (path[p], distribution[d], coverage[c]): ";
        if (!(std::cin >> mission_input)) throw std::invalid_argument("invalid type input");

        switch (mission_input) {
            case 'p':
                config.mission_type = MissionType::Path;
                break;

            case 'd':
                config.mission_type = MissionType::Distribution;
                break;

            case 'c':
                config.mission_type = MissionType::Coverage;
                break;

            default:
                throw std::invalid_argument("mission must be p, d, or c");
        }
        
        std::cout << "agents (1-20): ";
        if (!(std::cin >> config.agent_count)) throw std::invalid_argument("invalid number of agents");
        
        if (config.mission_type == MissionType::Coverage) return config;
            
        char planner_type;
        std::cout << "planner astar[a], rrt[r], rrtstar[s]: ";
        if(!(std::cin >> planner_type)) throw std::invalid_argument("invalid planner input");

        switch (planner_type) {
        case 'a':
            config.planner_type = PlannerType::Astar;
            break;
        case 'r':
            config.planner_type = PlannerType::Rrt;
            break;
        case 's':
            config.planner_type = PlannerType::Rrtstar;
            break;
        
        default:
            throw std::invalid_argument("planner must be a, r, or s");
        }
        
        return config;
    }

    Position random_position(const Grid& grid) {
        static std::random_device rd;
        static std::mt19937 rng(rd());

        std::uniform_int_distribution<int> x_d(0, static_cast<int>(grid.width()) - 1);
        std::uniform_int_distribution<int> y_d(0, static_cast<int>(grid.height()) - 1);

        Position position;

        do {
            position = {x_d(rng), y_d(rng)};
        } while (!grid.traversable(position));

        return position;
    }
}