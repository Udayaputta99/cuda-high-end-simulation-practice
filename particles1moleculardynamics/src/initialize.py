import json
import numpy as np
import csv
import math

with open("config.json", "r") as file:
    config = json.load(file)

N = config["number_of_particles"]
sigma = config["sigma"]
epsilon = config["epsilon"]
temperature = config["temperature"]
mass = config["mass"]
kB = config["kB"]

spacing = sigma*1.5
dim = math.ceil(N**(1/3))
start = 0
x = start + spacing * np.arange(dim)
grid = np.array(np.meshgrid(x, x, x)).T.reshape(-1, 3)

std = ((kB * temperature) / mass)**(1/2)
velocities = np.random.normal(loc=0, scale=std, size=N*3)

data = []
for i in range(N):
    
    data.append({
        "x": grid[i][2],
        "y": grid[i][0],
        "z": grid[i][1],
        "mass": mass,
        "vx": velocities[3*i],
        "vy": velocities[3*i+1],
        "vz": velocities[3*i+2]
    })

with open("data.csv", "w", newline="") as file:
    writer = csv.DictWriter(file, fieldnames=["x", "y", "z", "mass", "vx", "vy", "vz"])
    writer.writeheader()
    writer.writerows(data)