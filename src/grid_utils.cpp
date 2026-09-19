#include <span/grid_utils.hpp>

#include <cmath>
#include <algorithm>

namespace span {

    double distance(Position a, Position b) {
        const double dx = static_cast<double>(b.x - a.x);
        const double dy = static_cast<double>(b.y - a.y);
        const double dz = static_cast<double>(b.z - a.z);

        return std::sqrt(dx * dx + dy * dy + dz * dz);
    }

    double movement_cost(Position a, Position b) {
        int changed_axes = 0;
        if (a.x != b.x) ++changed_axes;
        if (a.y != b.y) ++changed_axes;
        if (a.z != b.z) ++changed_axes;

        return std::sqrt(static_cast<double>(changed_axes));
    }

    std::vector<Position> generate_neighbors(const Grid& grid, Position position) {
        std::vector<Position> neighbors;
        neighbors.reserve(26);

        for (int dz = -1; dz <= 1; ++dz) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (dx == 0 && dy == 0 && dz == 0) continue;

                    Position next{
                        position.x + dx,
                        position.y + dy,
                        position.z + dz
                    };

                    if (!grid.traversable(next)) continue;

                    const int changed_axes = (dx != 0) + (dy != 0) + (dz != 0);

                    if (changed_axes > 1) {
                        bool any_axis_open = false;

                        if (dx != 0 && grid.traversable({position.x + dx, position.y, position.z}))
                            any_axis_open = true;

                        if (dy != 0 && grid.traversable({position.x, position.y + dy, position.z}))
                            any_axis_open = true;

                        if (dz != 0 && grid.traversable({position.x, position.y, position.z + dz}))
                            any_axis_open = true;

                        if (!any_axis_open) continue;
                    }

                    neighbors.push_back(next);
                }
            }
        }

        return neighbors;
    }


    bool collision_free(const Grid& grid, Position a, Position b) {
        int dx = b.x - a.x;
        int dy = b.y - a.y;
        int dz = b.z - a.z;
        
        int steps = std::max({std::abs(dx), 
                              std::abs(dy), 
                              std::abs(dz)});

        if (steps == 0) return grid.traversable(a);

        for (int i = 0; i <= steps; ++i) {
            double t = static_cast<double>(i) / steps;

            double x = a.x + dx * t;
            double y = a.y + dy * t;
            double z = a.z + dz * t;

            Position cell{static_cast<int>(std::round(x)), 
                          static_cast<int>(std::round(y)), 
                          static_cast<int>(std::round(z))};

            if (!grid.traversable(cell)) return false;
        }

        return true;
    }
}