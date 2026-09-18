#include <span/astar.hpp>

#include <algorithm>
#include <cmath>
#include <iostream>

namespace span {
    Astar::Astar(const Grid& grid, Position start, Position goal)
        : grid_(grid),
          start_(start),
          goal_(goal),
          closed_(grid.width() * grid.height() * grid.depth(), false),
          nodes_(grid.width() * grid.height() * grid.depth()) {}

    double Astar::heuristic(Position a, Position b) const {
        const double dx = static_cast<double>(b.x - a.x);
        const double dy = static_cast<double>(b.y - a.y);
        const double dz = static_cast<double>(b.z - a.z);
        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    double Astar::movement_cost(Position a, Position b) const {
        int changed_axes = 0;
        if (a.x != b.x) ++changed_axes;
        if (a.y != b.y) ++changed_axes;
        if (a.z != b.z) ++changed_axes;
        return std::sqrt(static_cast<double>(changed_axes));
    }

    std::vector<Position> Astar::generate_neighbors(Position position) const {
        std::vector<Position> neighbors;
        neighbors.reserve(26);

        for (int dz = -1; dz <= 1; ++dz) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;

                    Position next{position.x + dx, position.y + dy, position.z + dz};
                    if (!grid_.traversable(next)) continue;

                    const int changed_axes = (dx != 0) + (dy != 0) + (dz != 0);

                    // Preserve the 2D rule's intent in 3D: diagonal motion cannot
                    // squeeze through a corner when every axis-adjacent escape is blocked.
                    if (changed_axes > 1) {
                        bool any_axis_open = false;
                        if (dx != 0 && grid_.traversable({position.x + dx, position.y, position.z})) {
                            any_axis_open = true;
                        }
                        if (dy != 0 && grid_.traversable({position.x, position.y + dy, position.z})) {
                            any_axis_open = true;
                        }
                        if (dz != 0 && grid_.traversable({position.x, position.y, position.z + dz})) {
                            any_axis_open = true;
                        }
                        if (!any_axis_open) continue;
                    }

                    neighbors.push_back(next);
                }
            }
        }

        return neighbors;
    }

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

        const double h = heuristic(start_, goal_);
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

            for (Position next : generate_neighbors(entry.position)) {
                const std::size_t next_index = grid_.index(next);
                if (closed_[next_index]) continue;

                const double g = current.g + movement_cost(entry.position, next);
                const double h_next = heuristic(next, goal_);
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
