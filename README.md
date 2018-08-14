# Timing performance
## Objectives ##
The aim of this project is to provide a simple C++ inteface to time as accurate as possible time execution of function code. It helps if you want to do very small (or big of course) optimization or compiler for instance.
## Details ##
Up to date details can be found here [http://fflayol.fr/timing-library-for-performance-test](http://fflayol.fr/timing-library-for-performance-test)
## Licence ##
This project is distributed under GPL see licence [https://www.gnu.org/licenses/gpl.html](https://www.gnu.org/licenses/gpl.html) for details.
## Usage ##
Clone the repository
```
git clone https://github.com/fflayol/timingperf.git
cd timingperf

;this command is optional, it will copy the file performancetiming.hpp to /usr/local/include
make install 

make
./ex1.out
```
Just include the header file performancetiming.hpp, call the needed function and that's it ;-).
