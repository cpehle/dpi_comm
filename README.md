# dpi_comm

Bidirectional dpi communication example code. Implements a 
system module which can send and receive 64bit words over DPI-C and a
a correponding c++ server that handles connections to those.

To build and test the example make sure that irun is in your path
open a tmux session and then execute
```
make run
```
in the `dpi_comm` directory in one window and in another
window in the same directory execute 
```
make test
```
