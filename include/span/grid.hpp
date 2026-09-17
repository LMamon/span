#pragma once

#include <span/types.hpp>

#include <cstddef>
#include <vector>

namespace span {

    enum class Occupancy { Free, Blocked };
    struct Position {
        int x;
        int y;

        bool operator<(const Position& other) const { 
            return x < other.x || (x == other.x && y < other.y);
        }
        
        bool operator==(const Position& other) const {
            return x == other.x && y == other.y;
        }
    };
    
    struct Cell {
        Occupancy occupancy = Occupancy::Free;
    };

    struct GridConfig {
        std::size_t width;
        std::size_t height;
        double blocked_probability;
        MissionType mission_type;
        int agent_count;
        PlannerType planner_type;
    };

    class Grid {
        public:
            static constexpr std::size_t MAX_WIDTH = 99;
            static constexpr std::size_t MIN_WIDTH = 1;
            
            static constexpr std::size_t MAX_HEIGHT = 99;
            static constexpr std::size_t MIN_HEIGHT = 1;

            Grid(GridConfig);

            std::size_t width() const;
            std::size_t height() const;

            bool contains(Position position) const {
                return position.x >= 0 && position.y >= 0 &&
                        static_cast<std::size_t>(position.x) < width_ &&
                        static_cast<std::size_t>(position.y) < height_;
            }

            bool traversable(Position position) const {
                return contains(position) && at(position).occupancy == Occupancy::Free;
            }

            const Cell& at(Position position) const;
            Cell& at(Position position);

            std::size_t index(Position position) const;

        private:
            std::size_t width_ = 0;
            std::size_t height_ = 0;
            std::vector<Cell> cells_;
            
            MissionType mission_type_;
            int agent_count_;
            PlannerType planner_type_;
    };
    Position random_position(const Grid& grid);

}