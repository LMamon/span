#pragma once

#include <span/grid.hpp>
#include <vector>

namespace span {

    double distance(Position a, Position b);
    double movement_cost(Position a, Position b);

    std::vector<Position> generate_neighbors(const Grid& grid, Position position);
    bool collision_free(const Grid& grid, Position a, Position b);

}