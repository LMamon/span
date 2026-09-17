#include <span/astar.hpp>
#include <span/grid.hpp>

#include <cmath>
#include <iostream>
#include <algorithm>

namespace span {
    Astar::Astar(const Grid& grid, Position start, Position goal) : grid_(grid), 
                                                                        start_(start), 
                                                                        goal_(goal),
                                                                        closed_(grid.width() * grid.height(), false),
                                                                        nodes_(grid.width() * grid.height()) {}

    double Astar::heuristic(Position a, Position b) const {
        // euclidian sqrt(dx^2 + dy^2)
        return std::hypot(b.x - a.x, b.y - a.y);
    }

    double Astar::movement_cost(Position a, Position b) const {
        return (a.x != b.x && a.y != b.y) ? std::sqrt(2.0) : 1.0;
    } 

    std::vector<Position> Astar::generate_neighbors(Position position) const {
        std::vector<Position> neighbors;

        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;

                Position next{position.x + dx, position.y + dy};
                //filter out bad candidates
                if (!grid_.contains(next)) continue;
                if (!grid_.traversable(next)) continue;
                
                neighbors.push_back(next);        
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
            std::cout << "start or goal is not traverable\n";
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

            // mark current closed_
            closed_[current_index] = true;
                        
            // generate the 8 neighbors
            for (Position next : generate_neighbors(entry.position)) {
                const std::size_t next_index = grid_.index(next);
                if (closed_[next_index]) continue;

                // calculate g
                double g = current.g + movement_cost(entry.position, next);
                // calculate h
                double h = heuristic(next, goal_);
                // calculate f
                double f = g + h;

                // create Node
                Node& successor = nodes_[next_index];
                if (g >= successor.g) continue;

                successor.parent = entry.position;
                successor.g = g;
                successor.h = h;
                successor.f = f;
                // push Node into open_
                open_.push({next, f});
            }
        }
        std::cout << ">>>no path exists<<<\n"; // add agent/team id for specific path
        return {};
    }
}