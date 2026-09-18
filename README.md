# Span

Data structures, algorithms and path planning for autonomous agents in a simple 3D grid world.

Span is a small C++ demo and tutorial for implementing the data structures and algorithms behind autonomous agents and standard path planning algorithms without relying on a game engine or simulation framework.

The simulation runs headlessly. A run records its world, obstacles, planned paths, goals, and agent positions to JSON. A small Plotly utility can turn that recording into a standalone animated 3D HTML file afterward.

## Build and run

```bash
./run.sh
```

Span prompts for:

- X / columns
- Y / rows
- Z / depth
- blocked-cell probability
- mission type
- agent count
- planner

A completed simulation writes a file under `runs/`:

```text
runs/run-YYYYMMDD-HHMMSS.json
```

The Nano does not need a desktop session or graphics stack.

## Render a run

On the machine where you want to view the run:

```bash
python3 -m venv .venv
source .venv/bin/activate
pip install -r tools/requirements.txt
python tools/render.py runs/run-YYYYMMDD-HHMMSS.json
```

The renderer produces a standalone HTML file beside the run and opens it in the default browser. The browser is only the offline display surface; Span itself has no web server, JavaScript application, or browser dependency.

The 3D view contains blocked cells, mission goals, planned paths, and animated agent positions. The camera can be rotated, panned, and zoomed without affecting the recorded simulation.

## Current planner status

- A*: implemented in 3D with 26-neighbor movement
- RRT: CLI placeholder; not implemented yet
- RRT*: CLI placeholder; not implemented yet
- Coverage mission: placeholder; not implemented yet

## Repository layout

```text
include/span/      C++ interfaces
src/               simulation, world, planner, recorder
tools/render.py    offline Plotly renderer
runs/              generated run recordings
```

## Contents

- 3D grid and spatial data structures
- autonomous agent behavior
- graph search and path planning
- offline run recording and replay visualization

### under construction

- swarm algorithms
- sampling-based planning
- genetic and evolutionary algorithms
