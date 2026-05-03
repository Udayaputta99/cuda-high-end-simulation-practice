# STREAM Benchmark

This folder contains the required files for building and executing the Julia Set example

## Description

In this exercise we aim to implement a C++ code to generate [Julia Set](https://en.wikipedia.org/wiki/Julia_set) fractal images in both serial and GPU-accelerated paradigms. 
## Getting Started

### Environment

* The benchmark was performed on the cip machines at HuberCip. The underlying machine specifications are mentioned below
* Operating System: Debian GNU/Linux 13 (trixie)
* CPU: 12th Gen Intel(R) Core(TM) i7-12700
* GPU: NVIDIA GeForce RTX 3070
* Compiler used: g++ (Debian 14.2.0-19) 14.2.0
* CUDA Compiler used: V13.2.51

### Building & Running the program

* Change directory into src
* Change the üarameters in the [Makefile](src/juliaset/Makefile) if necessary
* Run ```make```
* The executables can be found in the build directory

## Authors

Contributors names and contact info

[Syed Usman Ahmed](usman.s.ahmed@fau.de) 
[Udaya Bhaskar Putta](udaya.putta@fau.de)
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