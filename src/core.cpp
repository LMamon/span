#include <span/core.hpp>
#include <span/agent.hpp>
#include <span/mission.hpp>
#include <span/recorder.hpp>

#include <iostream>
#include <random>
#include <stdexcept>

namespace span {
    void Core::run(const volatile std::sig_atomic_t& stop_requested) {
        try {
            const GridConfig config = getInput();
            Grid grid(config);

            std::vector<Agent> agents;
            agents.reserve(static_cast<std::size_t>(config.agent_count));

            for (int i = 0; i < config.agent_count; ++i) {
                agents.emplace_back(random_position(grid));
            }

            std::vector<Team> teams = create_teams(std::move(agents));

            Mission mission(config.mission_type);
            mission.configure(grid, teams);

            if (config.mission_type != MissionType::Coverage) {
                for (Team& team : teams) {
                    team.route(grid, config.planner_type);
                }
            }

            RunRecorder recorder(config);
            recorder.record(teams);

            while (!stop_requested) {
                bool moved = false;

                for (Team& team : teams) {
                    for (Agent& agent : team.agents) {
                        if (!agent.finished()) {
                            agent.update(grid);
                            moved = true;
                        }
                    }
                }

                if (!moved) break;
                recorder.record(teams);
            }

            const std::string output = recorder.write(grid, teams, mission.goals());
            std::cout << "wrote " << output << '\n';

        } catch (const std::exception& e) {
            std::cerr << "error: " << e.what() << '\n';
        }
    }

    GridConfig Core::getInput() {
        GridConfig config;

        std::cout << "columns/x (1-" << Grid::MAX_WIDTH << "): ";
        if (!(std::cin >> config.width)) throw std::invalid_argument("invalid column input");

        std::cout << "rows/y (1-" << Grid::MAX_HEIGHT << "): ";
        if (!(std::cin >> config.height)) throw std::invalid_argument("invalid row input");

        std::cout << "depth/z (1-" << Grid::MAX_DEPTH << "): ";
        if (!(std::cin >> config.depth)) throw std::invalid_argument("invalid depth input");

        std::cout << "probability cell is blocked (0-1): ";
        if (!(std::cin >> config.blocked_probability)) throw std::invalid_argument("invalid probability input");

        char mission_input;
        std::cout << "mission (path[p], distribution[d], coverage[c]): ";
        if (!(std::cin >> mission_input)) throw std::invalid_argument("invalid type input");

        switch (mission_input) {
            case 'p': config.mission_type = MissionType::Path; break;
            case 'd': config.mission_type = MissionType::Distribution; break;
            case 'c': config.mission_type = MissionType::Coverage; break;
            default: throw std::invalid_argument("mission must be p, d, or c");
        }

        std::cout << "agents (1-20): ";
        if (!(std::cin >> config.agent_count)) throw std::invalid_argument("invalid number of agents");
        if (config.agent_count < 1 || config.agent_count > 20) {
            throw std::invalid_argument("agents must be between 1 and 20");
        }

        if (config.mission_type == MissionType::Coverage) return config;

        char planner_type;
        std::cout << "planner astar[a], rrt[r], rrtstar[s]: ";
        if (!(std::cin >> planner_type)) throw std::invalid_argument("invalid planner input");

        switch (planner_type) {
            case 'a': config.planner_type = PlannerType::Astar; break;
            case 'r': config.planner_type = PlannerType::Rrt; break;
            case 's': config.planner_type = PlannerType::Rrtstar; break;
            default: throw std::invalid_argument("planner must be a, r, or s");
        }

        return config;
    }

    Position random_position(const Grid& grid) {
        static std::random_device rd;
        static std::mt19937 rng(rd());

        std::uniform_int_distribution<int> x_d(0, static_cast<int>(grid.width()) - 1);
        std::uniform_int_distribution<int> y_d(0, static_cast<int>(grid.height()) - 1);
        std::uniform_int_distribution<int> z_d(0, static_cast<int>(grid.depth()) - 1);

        Position position;
        do {
            position = {x_d(rng), y_d(rng), z_d(rng)};
        } while (!grid.traversable(position));

        return position;
    }
}
