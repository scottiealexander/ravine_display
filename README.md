# ravine_display
Simple visual stimuli display routines for RaViNE system

## Contents
The final product of successful building (see below) are 2 executable programs that will display a series of sinewave gratings (`run_grating`) or a m-sequence checkerboard routine (`run_checkerboard`). Both take the following input arguments

* -f: set to use fullscreen mode, omit for windowed display
* <ini_path> the path to a correctly formatted .ini file (see below)

## Dependencies
* GLFW3 (e.g. on Ubuntu `sudo apt-get install libglfw3 libglfw3-dev`)
* GLEW (e.g. on Ubuntu `sudo apt-get install libglew2.0 and libglew-dev`)

## Building
In theory, building the two programs (`run_grating`, and `run_checkerboard`) should only require:

```bash
cmake .
make run_grating
make run_checkerboard
```

then, to test the two programs:

```bash
cd bin
./run_grating ../test/test3.ini
./run_checkerboard ../test_mseq.ini
```

**Note** that to kill the checkerboard program (i.e. during testing) press the [ESC] key at any time.

## INI driver format

A few examples can be found in the `test/` directory.

#### Gratings
Grating .ini file must have the following fields:

* `orientation`: units are degrees
* `spatial_frequency`: units are cycles per pixel
* `temporal_frequency`: units are cycles per second
* `contrast`: Michaelson contrast in decimal format (0-1)
* `radius`: units are pixels
* `duration`: units are seconds
* `blank_duration`: inter-trial interval, units are seconds
* `repeats`: number of times to repeat **EACH** stimulus
* `varying`: the parameter to vary across trials, **ONE OF**:
    * orientation
    * spatial_frequency
    * temporal_frequency
    * contrast
    * radius
* `values`: array of values, one per trial, that the varying parameter will take e.g. [0, 45, 90, 135]
* `triggers`: array of trigger / event codes, one for each value, e.g [1, 2, 3, 4]
* `host-ip`: ip address of data acquisition client
* `host-port`: port to use for tcp/ip connections, set to a number < 0 to ignore

#### Checkerboard
Checkerboard .ini file must have the following fields:

* `repeats`: number of time to repeat the **entire** sequence
* `frames-per-term`: duration of each sequence frame in untis of screen frames
* `element-size`: size length of each square, units are pixels
* `frame-trigger`: integer trigger/event code to use for frame events
* `host-ip`: ip address of data acquisition client
* `host-port`: port to use for tcp/ip connections, set to a number < 0 to ignore
