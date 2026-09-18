#pragma once

#include <span/grid.hpp>

#include <csignal>

namespace span {
    class Core {
        public:
            void run(const volatile std::sig_atomic_t& stop_requested);

        private:
            GridConfig getInput();
    };
}
