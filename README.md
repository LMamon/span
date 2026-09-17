# Span

Data structures, algorithms and path planning for autonomous agents, visualized in the terminal.

Span is a small C++ demo and tutorial for implementing the data structures and
algorithms behind autonomous agents and the standard path planning algorithms without relying on a game engine or simulation framework.

The environment is intentionally simple: a fixed-width grid rendered directly
in the terminal. Agents, obstacles, paths, and algorithm state are represented
with basic symbols so the implementation stays focused on the underlying
concepts.

The repository builds up from the environment and its data structures into
path planning, optimization, and agent behavior, with each implementation
designed to be small enough to read, run, modify, and understand.

## Visualization

| Symbol | Meaning |
| --- | --- |
| ` ` | Free space |
| `X` | Blocked space |
| `●` | Selected path |
| `○` | Agent |

## Contents

- Grid and spatial data structures
- Autonomous agent behavior
- Graph search and path planning

### under construction
- Swarm algorithms
- Sampling-based planning
- Genetic and evolutionary algorithms