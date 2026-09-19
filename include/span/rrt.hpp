#pragma once

#include <span/grid.hpp>

#include <cstddef>
#include <vector>

namespace span {
    class Rrt {
        public:
            Rrt(const Grid& grid, Position start, Position goal, std::size_t K, double delta_q);

            std::vector<Position> plan();

        private:
            struct Node {
                Position position{0, 0, 0};
                Position parent{0, 0, 0};
            };

            std::vector<Position> reconstruct_path() const;
            Position nearest_vertex (const std::vector<Node>& tree, Position qrand) const; 
            Position steer (Position qnear, Position qrand, double delta_q) const;

            void connect(Position qnear, Position qnew) const;

            std::vector<Node> tree_;
            const Grid& grid_;
            Position start_;
            Position goal_; 
            std::size_t K_;
            double delta_q_;
    };
}