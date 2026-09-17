#pragma once

#include <span/grid.hpp>
#include <span/agent.hpp>

#include <string>

namespace span {
    class Renderer {
        public:
            void render(const Grid& grid, const std::vector<Team>& teams, const std::vector<Position>& goals);
            
        private:
            static constexpr int SLOT_WIDTH = 3;
            const std::string& team_color(std::size_t id);

        };
}