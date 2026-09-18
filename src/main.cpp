#include <span/core.hpp>

#include <csignal>
#include <cstdlib>

namespace {
    volatile std::sig_atomic_t stop_requested = 0;
    void signalHandler(int) { stop_requested = 1; }
}

int main() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    span::Core core;
    core.run(stop_requested);

    return EXIT_SUCCESS;
}
