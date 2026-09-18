#pragma once

namespace span {
    enum class MissionType {
        Path, Distribution, Coverage
    };

    enum class PlannerType {
        Astar, Rrt, Rrtstar
    };
}
