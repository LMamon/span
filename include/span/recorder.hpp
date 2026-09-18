#pragma once

#include <span/agent.hpp>
#include <span/grid.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace span {

    struct RecordedAgent {
        std::size_t team_id;
        std::size_t agent_id;
        Position position;
    };

    struct Frame {
        std::size_t step;
        std::vector<RecordedAgent> agents;
    };

    class RunRecorder {
        public:
            explicit RunRecorder(GridConfig config);

            void record(const std::vector<Team>& teams);

            std::string write(const Grid& grid,
                              const std::vector<Team>& teams,
                              const std::vector<Position>& goals) const;

        private:
            GridConfig config_;
            std::vector<Frame> frames_;
    };
}
