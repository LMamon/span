#include <span/astar.hpp>
#include <span/grid_utils.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace span {
    Astar::Astar(const Grid& grid, Position start, Position goal) : grid_(grid),
                                                                    start_(start),
                                                                    goal_(goal),
                                                                    closed_(grid.width() * grid.height() * grid.depth(), false),
                                                                    nodes_(grid.width() * grid.height() * grid.depth()) {}

    std::vector<Position> Astar::reconstruct_path() const {
        std::vector<Position> path;
        Position current = goal_;

        while (!(current == start_)) {
            path.push_back(current);
            current = nodes_[grid_.index(current)].parent;
        }

        path.push_back(start_);
        std::reverse(path.begin(), path.end());
        return path;
    }

    std::vector<Position> Astar::plan() {
        if (!grid_.traversable(start_) || !grid_.traversable(goal_)) {
            std::cout << "start or goal is not traversable\n";
            return {};
        }

        const double h = distance(start_, goal_);
        Node& start = nodes_[grid_.index(start_)];
        start.parent = start_;
        start.g = 0.0;
        start.h = h;
        start.f = h;
        open_.push({start_, start.f});

        while (!open_.empty()) {
            OpenNode entry = open_.top();
            open_.pop();

            const std::size_t current_index = grid_.index(entry.position);
            if (closed_[current_index]) continue;

            Node& current = nodes_[current_index];
            if (entry.position == goal_) return reconstruct_path();

            closed_[current_index] = true;

            for (Position next : generate_neighbors(grid_, entry.position)) {
                const std::size_t next_index = grid_.index(next);
                if (closed_[next_index]) continue;

                const double g = current.g + movement_cost(entry.position, next);
                const double h_next = distance(next, goal_);
                const double f = g + h_next;

                Node& successor = nodes_[next_index];
                if (g >= successor.g) continue;

                successor.parent = entry.position;
                successor.g = g;
                successor.h = h_next;
                successor.f = f;
                open_.push({next, f});
            }
        }

        std::cout << ">>>no path exists<<<\n";
        return {};
    }
}
