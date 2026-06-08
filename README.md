# Celmata

<img width="200" height="200" alt="celmata" src="https://github.com/user-attachments/assets/56a67342-afc9-4e46-9955-d88bd189a832" />

Celmata is a terminal-based ecosystem simulator written in C, inspired by cellular automata and artificial life research. The core idea being that complex emergent behavior can arise from a grid of simple, local rules.

## Inspiration

The project draws from two main fields:

**Cellular Automata** - systems like Conway's Game of Life show how global complexity emerges purely from local cell interactions. Cellmata applies this principle not just to grass but to the agents themselves, whose behavior is defined per-cell-per-tick.

**Artificial Life & Evolutionary Simulation** - projects like Tierra and Avida demonstrated that selection pressure applied to self-replicating entities produces genuine evolutionary dynamics without explicitly programming them. Celmata tries to touch this with a minimal model: rabbits have heritable traits (speed, max energy) that mutate on reproduction, and the environment acts as the selection pressure. Faster rabbits find food more often, higher max-energy rabbits survive lean patches longer.

No fitness function is written, survival and reproduction emerge from the rules.

All simulation parameters (grid size, mutation rates, reproduction thresholds, grass dynamics) are compile-time `#define` constants in `types.h`, making the model easy to reconfigure without touching logic code.

## Roadmap ( for the future )

### Map Generation - Cluster System
Currently grass initializes with a flat per-cell probability across the whole grid. The planned improvement is a hotspot/cluster-based generator: seed a set of attraction points at map init, then bias grass placement toward those centers using a distance-weighted probability. This would produce biomes - dense meadows separated by sparse terrain - creating natural pressure on rabbit populations to compete for territory rather than spreading uniformly.

### Predators
Introduce a second agent type (foxes or wolves) that hunts rabbits. Predators would share the same trait/mutation architecture as rabbits but with inverted energy dynamics: energy gained by catching a rabbit, lost each tick from movement. This creates a classic predator-prey feedback loop (Lotka-Volterra) that should emerge from the rules without being hardcoded - rabbit populations crash when predators thrive, predators starve when rabbits are scarce, populations oscillate.

### Survival Depth
Current rabbit behavior is purely reactive and direction-blind. Planned additions:
- **Vision-driven movement** - the `vision` field exists in the `Rabbit` struct but is unused. Rabbits should scan a radius of `vision` cells each step and steer toward the nearest grass rather than walking in a random cardinal direction.
- **Energy-cost differentiation** - movement cost should vary by action type (idle < walk < sprint), making `speed` a meaningful tradeoff rather than a free stat.
- **Age and lifespan** -
 rabbits currently live indefinitely unless starved. A maximum lifespan (also heritable and mutable) would add another axis of selection pressure.

### Graphs & Data Export
To study population dynamics and evolutionary trends, a data layer needs to be added alongside the simulation loop:
- Per-tick snapshot of population count, mean energy, mean speed, mean max\_energy, and generation distribution written to a CSV log.
- A companion Python script (or gnuplot config) to plot population curves, trait averages over time, and trait variance — making it possible to observe selection sweeps and genetic drift visually.
- Optional: real-time graph rendered in a second terminal pane using ANSI bar charts so the simulation and its statistics are visible simultaneously.

## Building

```bash
make
./cellmata
```

Requires GCC and a POSIX terminal.

## License

MIT — see [LICENSE](LICENSE).
