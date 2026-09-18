#pragma once

#include <span/types.hpp>

#include <cstddef>
#include <vector>

namespace span {

    enum class Occupancy { Free, Blocked };

    struct Position {
        int x;
        int y;
        int z;

        bool operator<(const Position& other) const {
            if (x != other.x) return x < other.x;
            if (y != other.y) return y < other.y;
            return z < other.z;
        }

        bool operator==(const Position& other) const {
            return x == other.x && y == other.y && z == other.z;
        }
    };

    struct Cell {
        Occupancy occupancy = Occupancy::Free;
    };

    struct GridConfig {
        std::size_t width = 0;
        std::size_t height = 0;
        std::size_t depth = 0;
        double blocked_probability = 0.0;
        MissionType mission_type = MissionType::Path;
        int agent_count = 1;
        PlannerType planner_type = PlannerType::Astar;
    };

    class Grid {
        public:
            static constexpr std::size_t MAX_WIDTH = 99;
            static constexpr std::size_t MIN_WIDTH = 1;
            static constexpr std::size_t MAX_HEIGHT = 99;
            static constexpr std::size_t MIN_HEIGHT = 1;
            static constexpr std::size_t MAX_DEPTH = 99;
            static constexpr std::size_t MIN_DEPTH = 1;

            explicit Grid(GridConfig config);

            std::size_t width() const;
            std::size_t height() const;
            std::size_t depth() const;

            bool contains(Position position) const {
                return position.x >= 0 && position.y >= 0 && position.z >= 0 &&
                       static_cast<std::size_t>(position.x) < width_ &&
                       static_cast<std::size_t>(position.y) < height_ &&
                       static_cast<std::size_t>(position.z) < depth_;
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
            std::size_t depth_ = 0;
            std::vector<Cell> cells_;

            MissionType mission_type_ = MissionType::Path;
            int agent_count_ = 1;
            PlannerType planner_type_ = PlannerType::Astar;
    };

    Position random_position(const Grid& grid);
}
