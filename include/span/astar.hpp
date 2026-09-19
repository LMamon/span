#pragma once

#include <span/grid.hpp>

#include <limits>
#include <queue>
#include <vector>

namespace span {

    class Astar {
        public:
            Astar(const Grid& grid, Position start, Position goal);

            std::vector<Position> plan();

        private:
            struct Node {
                Position parent{0, 0, 0};
                double g = std::numeric_limits<double>::infinity();
                double h = std::numeric_limits<double>::infinity();
                double f = std::numeric_limits<double>::infinity();
            };

            struct OpenNode {
                Position position;
                double f;
            };

            struct CompareNode {
                bool operator()(const OpenNode& a, const OpenNode& b) const {
                    return a.f > b.f;
                }
            };

            std::vector<Position> reconstruct_path() const;

            const Grid& grid_;
            Position start_;
            Position goal_;

            std::priority_queue<OpenNode, std::vector<OpenNode>, CompareNode> open_;
            std::vector<bool> closed_;
            std::vector<Node> nodes_;
    };
}
