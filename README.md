# Compiler

## Course Materials

The lecture slides, handouts and manuals can be found at [CS 143-Compilers](http://web.stanford.edu/class/cs143/). The lecture videos and the whole project package is available at [Lagunita: CS 143-Compilers](https://lagunita.stanford.edu/courses/Engineering/Compilers/Fall2014/course/) (you need to register first).

## Prerequisites

To use the C++ version on Mac OS, some packages need to be installed:
```
xcode-select --install
brew install flex
brew install bison
```
You need to `export LDFLAGS="-L/usr/local/opt/flex/lib"` before `make lexer` (or you can just put it in your bash_profile) otherwise error `ld: library not found for -lfl` will be reported.

In order to run your MIPS code, you have to set up a Linux virtual machine because the spim provided is a Linux binary executable. 32-bit libraries need to be installed as well:
```
sudo apt-get install lib32ncurses5 lib32z1
```
Then run your code with `$COOL_PATH/bin/spim`.

## Usage

All the handouts are in [Handouts](Handouts). All the executables are in [Executables](Executables).

In every assignment directory I put useful resources, my implementation ideas, proofs and key points of the assignment in a README note. If you have no idea how to get started, read the notes first and then the codes.

I have not fully tested my compiler so there may be some latent bugs. If you find any please let me know!

## My Words on the Project

I really love the course and this project! I have spent several months on it and it is really exciting to work out my own implementation of each assignment. Furthermore, it is also helpful to improve my code ability and project management ability.

However, the project environment is obsolete. It uses MIPS assembly, the executables are 32-bit and the virtual machine provided is a hard-to-use Bodhi system. I wish these can be improved to make the project a perfect one.