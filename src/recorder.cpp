#include <span/recorder.hpp>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace span {
    namespace {
        const char* mission_name(MissionType type) {
            switch (type) {
                case MissionType::Path: return "path";
                case MissionType::Distribution: return "distribution";
                case MissionType::Coverage: return "coverage";
            }
            return "unknown";
        }

        const char* planner_name(PlannerType type) {
            switch (type) {
                case PlannerType::Astar: return "astar";
                case PlannerType::Rrt: return "rrt";
                case PlannerType::Rrtstar: return "rrtstar";
            }
            return "unknown";
        }

        void write_position(std::ostream& out, Position p) {
            out << '[' << p.x << ',' << p.y << ',' << p.z << ']';
        }

        std::string output_path() {
            namespace fs = std::filesystem;
            fs::create_directories("runs");

            const auto now = std::chrono::system_clock::now();
            const std::time_t time = std::chrono::system_clock::to_time_t(now);
            std::tm tm{};
#if defined(_WIN32)
            localtime_s(&tm, &time);
#else
            localtime_r(&time, &tm);
#endif
            std::ostringstream name;
            name << "runs/run-" << std::put_time(&tm, "%Y%m%d-%H%M%S") << ".json";
            return name.str();
        }
    }

    RunRecorder::RunRecorder(GridConfig config) : config_(config) {}

    void RunRecorder::record(const std::vector<Team>& teams) {
        Frame frame;
        frame.step = frames_.size();

        for (const Team& team : teams) {
            for (std::size_t i = 0; i < team.agents.size(); ++i) {
                frame.agents.push_back({team.id, i, team.agents[i].position()});
            }
        }

        frames_.push_back(std::move(frame));
    }

    std::string RunRecorder::write(const Grid& grid,
                                   const std::vector<Team>& teams,
                                   const std::vector<Position>& goals) const {
        const std::string path = output_path();
        std::ofstream out(path);
        if (!out) throw std::runtime_error("could not open run output file");

        out << "{\n";
        out << "  \"schema\": \"span.run.v1\",\n";
        out << "  \"world\": {\"width\":" << grid.width()
            << ",\"height\":" << grid.height()
            << ",\"depth\":" << grid.depth()
            << ",\"blocked_probability\":" << config_.blocked_probability << "},\n";
        out << "  \"mission\": \"" << mission_name(config_.mission_type) << "\",\n";
        out << "  \"planner\": \"" << planner_name(config_.planner_type) << "\",\n";

        out << "  \"obstacles\": [";
        bool first = true;
        for (std::size_t z = 0; z < grid.depth(); ++z) {
            for (std::size_t y = 0; y < grid.height(); ++y) {
                for (std::size_t x = 0; x < grid.width(); ++x) {
                    Position p{static_cast<int>(x), static_cast<int>(y), static_cast<int>(z)};
                    if (grid.at(p).occupancy != Occupancy::Blocked) continue;
                    if (!first) out << ',';
                    write_position(out, p);
                    first = false;
                }
            }
        }
        out << "],\n";

        out << "  \"goals\": [";
        for (std::size_t i = 0; i < goals.size(); ++i) {
            if (i) out << ',';
            write_position(out, goals[i]);
        }
        out << "],\n";

        out << "  \"teams\": [";
        for (std::size_t t = 0; t < teams.size(); ++t) {
            if (t) out << ',';
            const Team& team = teams[t];
            out << "{\"id\":" << team.id << ",\"goal\":";
            write_position(out, team.goal);
            out << ",\"agents\":[";

            for (std::size_t a = 0; a < team.agents.size(); ++a) {
                if (a) out << ',';
                const Agent& agent = team.agents[a];
                out << "{\"id\":" << a << ",\"start\":";
                write_position(out, agent.start());
                out << ",\"path\":[";
                for (std::size_t p = 0; p < agent.path().size(); ++p) {
                    if (p) out << ',';
                    write_position(out, agent.path()[p]);
                }
                out << "]}";
            }

            out << "]}";
        }
        out << "],\n";

        out << "  \"frames\": [";
        for (std::size_t f = 0; f < frames_.size(); ++f) {
            if (f) out << ',';
            const Frame& frame = frames_[f];
            out << "{\"step\":" << frame.step << ",\"agents\":[";
            for (std::size_t a = 0; a < frame.agents.size(); ++a) {
                if (a) out << ',';
                const RecordedAgent& agent = frame.agents[a];
                out << "{\"team\":" << agent.team_id
                    << ",\"agent\":" << agent.agent_id
                    << ",\"position\":";
                write_position(out, agent.position);
                out << '}';
            }
            out << "]}";
        }
        out << "]\n";
        out << "}\n";

        return path;
    }
}
