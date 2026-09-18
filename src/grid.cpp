#include <span/grid.hpp>

#include <random>
#include <stdexcept>

namespace span {
    Grid::Grid(GridConfig config) {
        if (config.width < MIN_WIDTH || config.width > MAX_WIDTH ||
            config.height < MIN_HEIGHT || config.height > MAX_HEIGHT ||
            config.depth < MIN_DEPTH || config.depth > MAX_DEPTH) {
            throw std::invalid_argument("grid dimensions must be between 1 and 99");
        }

        if (!(config.blocked_probability >= 0.0 && config.blocked_probability <= 1.0)) {
            throw std::invalid_argument("blocked probability must be between 0 and 1");
        }

        width_ = config.width;
        height_ = config.height;
        depth_ = config.depth;
        cells_.resize(width_ * height_ * depth_);

        std::random_device rd;
        std::mt19937 rng(rd());
        std::bernoulli_distribution blocked(config.blocked_probability);

        for (Cell& cell : cells_) {
            cell.occupancy = blocked(rng) ? Occupancy::Blocked : Occupancy::Free;
        }

        mission_type_ = config.mission_type;
        agent_count_ = config.agent_count;
        planner_type_ = config.planner_type;
    }

    std::size_t Grid::width() const { return width_; }
    std::size_t Grid::height() const { return height_; }
    std::size_t Grid::depth() const { return depth_; }

    std::size_t Grid::index(Position position) const {
        if (!contains(position)) {
            throw std::out_of_range("grid position out of bounds");
        }

        return static_cast<std::size_t>(position.z) * width_ * height_ +
               static_cast<std::size_t>(position.y) * width_ +
               static_cast<std::size_t>(position.x);
    }

    const Cell& Grid::at(Position position) const { return cells_[index(position)]; }
    Cell& Grid::at(Position position) { return cells_[index(position)]; }
}
