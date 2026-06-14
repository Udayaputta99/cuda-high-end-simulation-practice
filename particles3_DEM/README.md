# Particle Simulations - Discrete Element Method

This project contains a CUDA implementation of a Discrete Element Method simulation for spherical particles. The simulation uses configurable particle data, gravity, fixed box boundaries, and spring-damper contact forces.

The simulation was run on an NVIDIA GeForce RTX 3070 GPU.

## Current simulation setup

The current setup is configured for DEM particle simulation:

- Particles are generated from `config.json`.
- Initial particle positions and velocities are written to `data.csv`.
- The simulation writes VTK files for visualization.
- Output files are written to the `vtk_output/` folder.

## Running the simulation

Inside the `src/` directory, run:

```bash
python3 run.py
```

The script will:

1. Build the project using `make`.
2. Read `config.json`.
3. Generate `data.csv`.
4. Run the CUDA DEM simulation.
5. Create the `vtk_output/` folder.
6. Write VTK simulation files into `vtk_output/`.

## Simulation output

The generated VTK files are written in the following format:

```text
vtk_output/particles_000000.vtk
vtk_output/particles_000100.vtk
vtk_output/particles_000200.vtk
...
```

The exact numbering depends on the `write_interval` value in `config.json`.

## Input files

The simulation uses:

```text
config.json
data.csv
```

The `data.csv` file contains:

```csv
x,y,z,vx,vy,vz,mass,radius
```

## Changing the simulation case

To change the number of particles or simulation parameters, edit `config.json`.

For example:

```json
{
    "number_of_particles": 1000,
    "mass": 1.0,
    "radius": 0.22,
    "temperature": 0.0,
    "kB": 1.0,
    "time_step": 0.00004,
    "number_of_steps": 80000,
    "cell_size": 0.55,
    "no_of_cells": 40,
    "spring_constant": 30000.0,
    "dissipation_constant": 120.0,
    "gravity": 9.81,
    "write_interval": 250
}
```

## Authors

Contributors names and contact info:

[Arjun Lenan Sandhya](arjun.lenan.sandhya@fau.de)  
[Udaya Bhaskar Putta](udaya.putta@fau.de)  
[Syed Usman Ahmed](usman.s.ahmed@fau.de)
