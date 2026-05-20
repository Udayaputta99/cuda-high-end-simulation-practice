# CUDA Accelerated Particle Simulation

This directory contains the a C++ application to run a particle simulation with inter-particle force modeled using the [Lennard-Jones potential](https://en.wikipedia.org/wiki/Lennard-Jones_potential)

## Description

This directory contains both the CPU version and the CUDA accelerated versions of the code. However, some functionalities are yet to be realized, like periodic boundary condition, total energy conservation check. This would be updated in the future versions of the code. 
## Getting Started

### Environment

* The benchmark was performed on the computer lab machines at Friedrich-Alexander-Universität, Erlangen and was part of the assessments of the High End Simulation in Practice course. The underlying machine specifications are mentioned below
* Operating System: Debian GNU/Linux 13 (trixie)
* CPU: 12th Gen Intel(R) Core(TM) i7-12700
* GPU: NVIDIA GeForce RTX 3070
* Compiler used: g++ (Debian 14.2.0-19) 14.2.0
* CUDA Compiler used: V13.2.51
* Additional Info: No additional libraries or modules are required

### Building & Running the program

* To run the CPU version of the code: ```./bash.sh```
* To run the GPU version of the code: ```./bash_cuda.sh```
* In case the terminal returns "Permission denied" error, enter ```chmod +x ./bash.sh``` in the terminal and run again. 
* The executables can be found in the build directory.
* The output _.vtu_ and _.pvd_ files can be found in the respective _.tar.gz_ files (particle_base.tar.gz or particle_cuda.tar.gz) 

### Using PARAVIEW
* To view the result in Paraview, download and extract the respective _.tar.gz_ file.
* In Paraview, open the _.pvd_ file and not individual _.vtu_ files. The pvd file carries the indices to build the result inside Paraview. 
### WARNING

Due to periodic boundary conditions not implemented, the particles would shoot off to infinity if the end time is too high. The preset endtime in the code provides good enough results to view the output. 
## Authors

Contributors names and contact info:
[Arjun Lenan Sandhya](arjun.lenan.sandhya@fau.de)
[Udaya Bhaskar Putta](udaya.putta@fau.de)
[Syed Usman Ahmed](usman.s.ahmed@fau.de) 
<!-- ## Version History

* 0.2
    * Various bug fixes and optimizations
    * See [commit change]() or See [release history]()
* 0.1
    * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details

## Acknowledgments

Inspiration, code snippets, etc.
 -->