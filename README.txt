CUDA code for developmental biology

Hardware requirement: 

Nvidia video card that supports SM 2.0+ and CUDA 4.0 

Software requirement: 
Cmake -- build automation
Cuda -- to support accelerated computation
Cgal -- computational geometry library
Thrust -- build-in library of cuda, similar to STL of C++
Paraview -- visualization software for animation purpose.

To obtain source code copy: 
git clone https://github.com/laosunhust/SceCells.git

To compile:
 (1) In project root folder, type "cmake ."
 (2) type "make" 

To run unit test:
 (1) Simple run: type "make test"
 (2) See more details about unit test: in project root folder, type "./bin/*****unit_test"

To run simulation:
in project root folder, type "./bin/run***Simulation"

Location of configuration files:
./resources
