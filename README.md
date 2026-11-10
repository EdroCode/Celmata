# Celmata

<img width="976" height="403" alt="Celmata Ecosystem Simulator Logo" src="https://github.com/user-attachments/assets/11675f70-96d9-419b-955d-53350ce5587e" />


An ecosystem simulator in C, built on a grid of simple cells where complex behaviour emerges from local rules.

> **Status: early development.** This is a rewrite of the original terminal-based version, now drawn with SDL2.

<img width="343" height="315" alt="screenshot-2026-11-10_17-50-50" src="https://github.com/user-attachments/assets/14320837-ae95-4664-9a1c-ebc0dcc1a8cf" />


## Goal

Celmata starts from Conway's Game of Life but is not tied to its two rules. The goal is to **watch interesting patterns and situations appear**: population waves, boom-and-bust cycles, herds, refuges, and "breeds" of animals that evolve to fit different parts of the map.

The world has three kinds of cells:

| Cell | Role | Colour (planned) | Status |
|---|---|---|---|
| **Bunny** | Herbivore. Moves, eats, reproduces, flees. Carries heritable genes. | white | in progress |
| **Bush** | Food. Gets eaten, then slowly regrows. | mint | in progress |
| **Wolf** | Predator. Hunts bunnies. Same gene system as bunnies. | coral | planned |

Nothing about the outcome is scripted. There is no fitness function: survival and reproduction come out of the rules.

## Inspiration

**Cellular automata.** Systems like Conway's Game of Life show that global complexity can come purely from local interactions. Celmata applies that to the agents themselves, whose behaviour is decided per cell, per tick.

**Artificial life and evolutionary simulation.** Projects like Tierra and Avida showed that selection pressure on self-replicating entities produces real evolutionary dynamics without programming them in. Celmata tries a minimal version of that: bunnies carry heritable traits that mutate when they reproduce, and the environment is the selection pressure.

**Predator-prey dynamics.** Once wolves exist, the Lotka-Volterra cycle (bunnies boom, wolves boom, bunnies crash, wolves starve) should appear on its own and should not be hardcoded.

[Artificial Life](https://www.youtube.com/watch?v=2g-CrQfYNtE&t=1858s)

## Design principles

1. **Every benefit has a price.** More vision, more speed and more offspring all cost energy. Otherwise evolution maxes everything out and the simulation gets boring.
2. **Limits come from feedback, not caps.** Crowding lowers birth rates, food runs out, predators starve when prey is rare. The only hard cap is a safety limit on the number of bunnies.
3. **Behaviour is scored, not scripted.** A bunny weighs hunger, fear and herd instinct in one formula instead of following an if/else priority list, so behaviour changes smoothly.
4. **Traits are inherited and mutate.** Natural selection decides which ones survive.
5. **Fair update order.** The order of updates is shuffled every tick, so nobody gets a permanent advantage from its position in memory.

## Rules

Rules marked *planned* are designed but not in the code yet. All numbers are starting values to tune.
This README might be outdated, compare differences.

### Bunny

**State:** `energy`, `age`, `repro_cooldown`, `move_budget`, `alive`, `hp`
**Genes:** `vision`, `speed`, `fear`, `herd`

Each tick, for every living bunny, in shuffled order:

1. **Perceive** *(partly done)*. Look at a radius of `round(vision)` cells for wolves, bushes with food and other bunnies. Anything outside the radius doesn't exist for this bunny. Local density is the share of cells in the radius occupied by bunnies.
2. **Eat** *(planned)*. If a bush with food is adjacent and no wolf is in view, graze for the whole turn: `energy += min(EAT_GAIN, bush.food, MAX_ENERGY - energy)`, and the bush loses that much. A bunny that sees a wolf doesn't graze, it moves.
3. **Move** *(random for now, scoring planned)*. Each tick `move_budget += speed` and the bunny moves once per whole point. For each free neighbour cell:

   ```
   score = hunger * bush_pull            // seek food
         - fear   * wolf_threat          // flee
         + herd   * (IDEAL_DENSITY - density) * bunny_pull   // group up in open space, spread out when crowded
         + NOISE  * random(0..1)
   ```

   where `hunger = 1 - energy / MAX_ENERGY` and each pull or threat is the sum of `1 / (distance + 1)` over what the bunny can see. It moves to the highest score.
4. **Reproduce** *(experimental)*. Needs `age >= MATURITY`, `energy > 70%`, `repro_cooldown == 0`, an adult neighbour and a free adjacent cell. Chance is `BASE_P * (1 - local_density)`, so crowding lowers births. The parent pays `REPRO_COST`, the child starts with `NEWBORN_ENERGY` and genes copied from the parent with small mutations, and the parent waits `REPRO_COOLDOWN` ticks.
5. **Metabolism** *(done)*. Every trait costs energy:

   ```
   energy -= BASE_COST + 0.15 * vision + (moved ? 0.2 * speed^2 : 0)
   ```

   Speed costs quadratically. A bunny dies when `energy <= 0` or `age > MAX_AGE`.
6. **Wolf encounter** *(planned)*, resolved on the wolf's turn:

   ```
   escape         = clamp(0.1 + 0.2 * speed + (next to a bush ? 0.3 : 0), 0, 0.9)
   attack_success = wolf.hunger * (1 - escape)
   ```


| Constant | Start | Constant | Start | Constant | Start |
|---|---|---|---|---|---|
| `MAX_ENERGY` | 100 | `BASE_COST` | 0.5 | `BASE_P` | 0.3 |
| `NEWBORN_ENERGY` | 25 | `MATURITY` | 20 | `REPRO_COST` | 30 |
| `MAX_AGE` | 150 | `EAT_GAIN` | 10 | `REPRO_COOLDOWN` | 10 |
| `IDEAL_DENSITY` | 0.4 | `NOISE` | 0.2 | | |

### Genes

| Gene | What it gives | What it costs | Design range |
|---|---|---|---|
| `vision` | Sees food and wolves from farther away | 0.15 energy per point per tick, plus more scanning | 1 to 6 |
| `speed` | More moves per tick, better escape chance | 0.2 * speed² energy on ticks it moved | 0.5 to 2.5 |
| `fear` | Flees wolves earlier | Avoids good food near wolves and may starve | 0 to 4 |
| `herd` | Safer in groups, easier to find mates | Crowding lowers births, groups are easy prey | 0 to 2 |

Mutation (*planned*): `child = clamp(parent * (1 + gauss(0, 0.1)), min, max)`. Genes must start above zero, because multiplying zero by anything stays zero.

All genes are currently fixed at 1 and nothing mutates yet.

### Bush *(planned)*

- Holds `food` from 0 to a maximum. Bunnies eat it, so it runs out.
- Regrows each tick by `regrow_rate * fertility`.
- Depleted bushes are drawn dimmer, so overgrazed patches are visible.
- Optional: when a bunny or wolf dies next to a bush, its `fertility` rises, so the animals feed the grass.

### Wolf *(planned)*

- Same `Genes` struct and mutation code as the bunny. `fear` is unused and `herd` becomes pack instinct.
- Gains energy by catching a bunny and loses it every tick, so a hungry wolf attacks more (`hunger = 1 - energy / MAX_ENERGY`).
- Co-evolution is expected: faster bunnies select for faster wolves and the other way round. Balance carefully, because wolves that evolve too well wipe out the bunnies and then starve.

## What to look for

| Change | Expected effect |
|---|---|
| Higher birth rate (`BASE_P`) | Explosive booms followed by crashes |
| Faster bush regrowth | Stable populations, fewer cycles |
| Larger vision | Smarter, more coordinated herds |
| Stronger herd instinct | Tight clusters, easier prey for wolves |
| Bigger cover bonus | "Bunny cities" around bush patches |
| Higher wolf hunger drain | Wolf collapse, then bunnies rebound |

Likely emergent patterns: travelling waves of wolves chasing bunnies, population oscillations, regions that evolve different "breeds" of bunny.

## Building

Requires GCC, `make` and the SDL2 development files (for example `libsdl2-dev` on Debian or Ubuntu). `clang-format` and `bear` are optional.

```bash
make            # build ./celmata
make run        # build and run
make clean      # remove build/ and the binary
make format     # run clang-format on all sources
bear -- make    # also writes compile_commands.json for your editor
```

Close the window or press Ctrl+C to stop. The tick count and a summary of the surviving bunnies are printed on exit.

## Project layout

```
main.c                  window, main loop, cleanup
src/
  types.h               Position, Genes (shared by bunny and wolf)
  game_loop.[ch]        one simulation tick
  drawing/canvas.[ch]   pixel buffer and drawing
  utils/shuffle.[ch]    update-order shuffle
  entities/
    bunny.h             public API (BunnyList is opaque)
    bunny_internal.h    private: Bunny and BunnyList definitions
    bunny.c             create, update, reproduce, metabolism
    bunny_perception.c  what a bunny can see
    bunny_debug.c       print functions
```

## License

MIT, see [LICENSE](LICENSE).
