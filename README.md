# BRT_C_SE_V2

This is a single-station BRT simulator, it can be used to evaluate the effect on the BRT station operation of changes in the service frequencies, dwell times or door assignment.

The simulator runs on C++, it is suggested to run it on a Linux machine or on a Windows Linux Subsystem (WLS) in a Windows machine. In the following the instructions.

# The `/conf` folder

The `/conf` folder contains all the input information for the simulation. There are two different kinds of required information:

- The layout information: this is the information regarding the existence of lanes and the possibility of making lane changes.

- The service information: this is the information regarding all possible bus services in the system, their names, frequencies, dwell times, assigned doors and bus types.

- The stop information: this is the information regarding the station, the available doors and their locations.

# The `/src_cpp` folder

The `/src_cpp` folder contains the code to perform the simulation, to compile the code we must go to the folder and run:

`g++ -O2 simulation.cpp -o simulation`

This will create the `simulation` executable file.

To run a simulation we should run:

`simulation {seed} {detailed output}`

The `{seed}` argument is an integer number that corresponds to the seed for all the random generators in the simulation. Simulations run with the same seed will produce the same results. The `{detailed output}` is either 0 or 1, 1 should be used if the detailed bus information is required, this will create an additional file with all the bus parameters printed for each time-step. This file is useful for visualization processes but is not needed in optimization processes. When the detailed information file is not needed `{detailed ouput}` should be set to 0.

After the simulation is finished, the output files will be created in the `/src_cpp/sim_results` folder. A single file for each service detailing the headway and the dwell time for each bus will be created, in addition, a file with the total time spent at the simulation for all the buses will be created. If `{detailed output}` has been set to 1, an additional file with the detailed information will be created.