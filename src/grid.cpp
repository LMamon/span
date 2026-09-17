#include <span/grid.hpp>
#include <span/mission.hpp>

#include <random>
#include <stdexcept>

namespace span {
    Grid::Grid(GridConfig config) {
        if (config.width < MIN_WIDTH || config.height < MIN_HEIGHT || 
            config.width > MAX_WIDTH || config.height > MAX_HEIGHT) {

            throw std::invalid_argument("grid dimensions must be between 1 and 99");
        }

        if (!(config.blocked_probability >= 0.0 && config.blocked_probability <= 1.0)) {
            throw std::invalid_argument("blocked probability must be between 0 and 1");
        }

        width_ = config.width;
        height_ = config.height;
        cells_.resize(width_ * height_);

        std::random_device rd;
        std::mt19937 rng(rd());
        std::bernoulli_distribution blocked(config.blocked_probability);

        for (Cell& cell : cells_) {
            cell.occupancy = blocked(rng) ? Occupancy::Blocked : Occupancy::Free;
        }


        if (!(config.mission_type == MissionType::Path || 
             config.mission_type == MissionType::Distribution || 
             config.mission_type == MissionType::Coverage)) {

            throw std::invalid_argument("mission must be p, d, or c");
        }

        agent_count_ = config.agent_count;

        if (!(config.planner_type == PlannerType::Astar || 
              config.planner_type == PlannerType::Rrt || 
              config.planner_type == PlannerType::Rrtstar)) {

            throw std::invalid_argument("planner must be astar[a], rrt[r], rrtstar[s]");
        }
        
        planner_type_ = config.planner_type;
    }

    std::size_t Grid::width() const { return width_; }
    std::size_t Grid::height() const { return height_; }

    std::size_t Grid::index(Position position) const {
        if (position.x >= width_ || position.y >= height_) {
            throw std::out_of_range("grid position out of bounds");
        }

        return position.y * width_ + position.x;
    }

    const Cell& Grid::at(Position position) const { return cells_[index(position)]; }
    Cell& Grid::at(Position position) { return cells_[index(position)]; }

}