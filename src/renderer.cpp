#include <span/renderer.hpp>
#include <span/agent.hpp>

#include <algorithm>
#include <iomanip>
#include <iostream>

namespace span {
    void Renderer::render(const Grid& grid, 
                          const std::vector<Team>& teams, 
                          const std::vector<Position>& goals, 
                          const std::map<Position, std::size_t>& visited_by) {
                            
        const std::string red = "\033[38;2;255;0;102m";
        const std::string green = "\033[38;2;57;255;20m";
        const std::string blue = "\033[38;2;0;246;255m";
        const std::string reset = "\033[0m";


        std::cout << '|' <<std::setw(SLOT_WIDTH) << "" << "|";

        for (std::size_t x = 0; x < grid.width(); ++x) {
            std::cout << std::setw(SLOT_WIDTH) << x << '|';
        }
        std::cout << "\n";

        for (std::size_t y = 0; y < grid.height(); ++y) {
            std::cout << '|' << std::setw(SLOT_WIDTH) << y << '|';
// 
            for (std::size_t x = 0; x < grid.width(); ++x) {
                Position position{static_cast<int>(x), static_cast<int>(y)};

                const Cell& cell = grid.at(position);

                // Blocked cells are environment state.
                // Nothing else may occupy, visit, or path through them.
                if (cell.occupancy == Occupancy::Blocked) {
                    std::cout << std::setw(SLOT_WIDTH) << "X" << '|';
                    continue;
                }

                const char* symbol = "";
                std::string symbol_color;
                bool on_path = false;
                bool is_start = false;

                // visited
                for (const Team& team : teams) {
                    const std::string& color = team_color(team.id);
                
                    for (const Agent& agent : team.agents) {
                        if (agent.visited().find(position) != agent.visited().end()) {
                            symbol = "■";
                            symbol_color = color;
                        }

                        if (std::find(agent.path().begin(),
                            agent.path().end(),
                            position) != agent.path().end()) {

                                on_path = true;
                        }

                        if (position == agent.start()) {
                            is_start = true;
                        }
                    }
                }
    
                // occupied
                for (const Team& team : teams) {
                    const std::string& color = team_color(team.id);
                
                    for (const Agent& agent : team.agents) {
                        if (position == agent.position()) {
                            symbol = "○";
                            symbol_color = color;
                        }
                    }
                }

                if (*symbol) {
                    std::cout << symbol_color << "  " << symbol << reset;
                } else {
                    std::cout << "   ";
                }

                // Goal overrides planned-path color.
                bool is_goal = std::find(goals.begin(), goals.end(), position) != goals.end();

                if (is_goal) {
                    std::cout << red << '|' << reset;
                } else if (is_start) {
                    std::cout << green << '|' << reset;
                } else if (on_path) {
                    std::cout << blue << '|' << reset;
                } else {
                    std::cout << '|';
                }
            }

            std::cout << '\n';
        }
    }

    const std::string& Renderer::team_color(std::size_t id) {
        static const std::vector<std::string> colors = {
            "\033[38;2;94;80;63m", // #5e503f
            "\033[38;2;131;56;236m", // #8338ec
            "\033[38;2;252;163;17m", // #fca311
            "\033[38;2;179;146;172m", // #b392ac
            "\033[38;2;224;122;95m" // #e07a5f
        };

        return colors[(id - 1) % colors.size()];
    }
}