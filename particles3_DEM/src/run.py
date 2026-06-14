import json
import csv
import math
import random
import subprocess

def build_project():
    print("Building...")
    subprocess.run(["make"], check=True)
    print("Build completed.\n")

def run_executable(path: str):
    subprocess.run([path], check=True)

with open("config.json", "r") as file:
    config = json.load(file)

N = config["number_of_particles"]
temperature = config.get("temperature", 0.0)
mass = config.get("mass", 1.0)
kB = config.get("kB", 1.0)

radius = config.get("radius", 0.5)

spacing = config.get("initial_spacing", 2.2 * radius)

start_x = config.get("start_x", radius)
start_y = config.get("start_y", radius)
start_z = config.get("start_z", radius)

dim = math.ceil(N ** (1.0 / 3.0))

std = math.sqrt(kB * temperature / mass) if temperature > 0.0 else 0.0

data = []

for iz in range(dim):
    for iy in range(dim):
        for ix in range(dim):
            if len(data) >= N:
                break

            jitter = 0.05 * radius

            x = start_x + spacing * ix + random.uniform(-jitter, jitter)
            y = start_y + spacing * iy + random.uniform(-jitter, jitter)
            z = start_z + spacing * iz + random.uniform(-jitter, jitter)

            vx = random.gauss(0.0, std)
            vy = random.gauss(0.0, std)
            vz = random.gauss(0.0, std)

            data.append({
                "x": x,
                "y": y,
                "z": z,
                "vx": vx,
                "vy": vy,
                "vz": vz,
                "mass": mass,
                "radius": radius
            })

with open("data.csv", "w", newline="") as file:
    writer = csv.DictWriter(
        file,
        fieldnames=["x", "y", "z", "vx", "vy", "vz", "mass", "radius"]
    )
    writer.writeheader()
    writer.writerows(data)

print(f"Generated {len(data)} particles")

build_project()

run_executable("../build/simulator/simulator-cuda")