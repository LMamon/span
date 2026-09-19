#include <span/rrt.hpp>
#include <span/grid_utils.hpp>

#include <algorithm>
#include <iostream>
#include <random>

namespace span {
    Rrt::Rrt(const Grid& grid, Position start, Position goal, std::size_t K, double delta_q) : grid_(grid),
                                                                                               start_(start),
                                                                                               goal_(goal),
                                                                                               K_(K),
                                                                                               delta_q_(delta_q) {}
    
    std::vector<Position> Rrt::reconstruct_path() const {
        std::vector<Position> path;
        Position current = goal_;

        while (!(current == start_)) {
            path.push_back(current);
            
            auto node = std::find_if(tree_.begin(), tree_.end(), [&](const Node& n) {
                return n.position == current;
            });

            if (node == tree_.end()) return {};

            current = node->parent;
        }

        path.push_back(start_);
        std::reverse(path.begin(), path.end());

        return path;
    }

    Position Rrt::nearest_vertex(const std::vector<Node>& tree, Position qrand) const {
        Position nearest = tree[0].position;
        double smallest = distance(nearest, qrand);

        for (std::size_t i = 0; i < tree.size(); ++i) {
            double d = distance(tree[i].position, qrand);
            
            if (d <= smallest) {
                smallest = d;
                nearest = tree[i].position;
            } 
        }
        return nearest;
            
    }

    Position Rrt::steer(Position qnear, Position qrand, double delta_q) const {
        double length = distance(qnear, qrand);

        if (length <= delta_q) return qrand;
        double scale = delta_q / length;

        Position qnew{
            static_cast<int>(std::round(qnear.x + (qrand.x - qnear.x) * scale)),
            static_cast<int>(std::round(qnear.y + (qrand.y - qnear.y) * scale)),
            static_cast<int>(std::round(qnear.z + (qrand.z - qnear.z) * scale))
        };

        return qnew;
    }

    void connect(Position qnear, Position qnew) {
        
    }

    std::vector<Position> Rrt::plan() {
        tree_.clear();
        tree_.emplace_back(start_, start_);

        for (std::size_t i = 0; i < K_; ++i) {
            Position qrand = random_position(grid_);
            Position qnear = nearest_vertex(tree_, qrand);
            Position qnew = steer(qnear, qrand, delta_q_);

            if (!grid_.traversable(qnew)) continue;
            if (!collision_free(grid_, qnear, qnew)) continue;

            if (std::any_of(tree_.begin(), tree_.end(), [&](const Node& node) { 
                return node.position == qnew; 
            })) {

            continue;
            }

            tree_.emplace_back(qnew);
            
            if (collision_free(grid_, qnew, goal_)) {
                tree_.emplace_back(goal_, qnew);

                return reconstruct_path();
            }   
        }

        std::cout << ">>>“RRT exhausted "<< K_ << "iterations without finding a path.”<<<\n";
        return {};
    };    
}