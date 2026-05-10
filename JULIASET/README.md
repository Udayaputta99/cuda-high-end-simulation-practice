# CUDA Accelerated Julia set render

Julia set refers to a complimentary set defined from a function which consists of values such that a small perturbation can generate drastic changes in the iterated function value. More in [Wikipedia.](https://en.wikipedia.org/wiki/Julia_set)

## Description

This directory contains both the CPU version and the CUDA accelerated versions of the code to generate both in color and greyscale the set, provided the suitable parameters.  
## Getting Started

### Environment

* The benchmark was performed on the computer lab machines at Friedrich-Alexander-Universität, Erlangen and was part of the assessments of the High End Simulation in Practice course. The underlying machine specifications are mentioned below
* Operating System: Debian GNU/Linux 13 (trixie)
* CPU: 12th Gen Intel(R) Core(TM) i7-12700
* GPU: NVIDIA GeForce RTX 3070
* Compiler used: g++ (Debian 14.2.0-19) 14.2.0
* CUDA Compiler used: V13.2.51

### Building & Running the program

* To run the CPU version of the code: ```make base```
* Modify parameters in the [Makefile](./Makefile) if necessary: Resolution of the output picture(NX x NY), value of complex number "c", RGB or Greyscale 
* To run the GPU version of the code: ```make cuda```
* The executables can be found in the build directory

## Authors

Contributors names and contact info

[Arjun Lenan Sandhya](arjun.lenan.sandhya@fau.de)

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