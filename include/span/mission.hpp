#pragma once

#include <span/agent.hpp>
#include <span/grid.hpp>
#include <span/types.hpp>

#include <vector>

namespace span {

    class Mission {
        public: 
            Mission(MissionType type);
        
            void configure(const Grid& grid, std::vector<Team>& teams);

            const std::vector<Position>& goals() const { return goals_; }
        
        private:
            MissionType type_;
            std::vector<Position> goals_;     
    };
}