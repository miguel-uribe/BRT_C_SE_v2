#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <array>

// This file defines all the system parameters
const int Dt=1;     // This is the time unit
const int Dx=1;     // This is the space unit
const  std::array<int, 2> busL = {17, 27};    // The bus length, equivalent to 30m
const float p=0.15;          // The random breaking probability
const float p0=0.5;         // The random breaking probability when stopped
const int d_speed = 1;      // The change in the speed when the random breaking occurs
const int Nparam = 20;    // Number of integer parameters for each bus
const int Nbool = 1;        // Number of boolean parameters for each bus
const int ncheck = 20; // the number of buses to check when looking for the lateral gaps
const int NStations = 2; // The total number of stations in the system
const int NLines = 18; // The total number of routes
const int L = 3072; // The length of the system
const int nkind = 7; // The different kind of station schemes
const int nordest = 2; // The different kind of origin and destinations
const int ndir = 2; // The different number of directions for buses to move
const int Nmax = 2; // The maximum number of lanes
const int fleet = 500; // The maximum numbers of moving buses
const int cld = 6; // The front distance required for a bus to change lanes
const int vsurr = (int) 9*Dt/Dx; // The speed to surrender priority
const int step = 15; // the headway binning width
const int maxheadway = 60*20; // above this headway the dwell time does not change
const int ndist = maxheadway/step+1; // the number of dwell time distributions per service
const int dtmin = 10; // the minimum dwell time
const int distmin = 150*Dx; // The minimal distance required for a bus in the stopping lane to change to the main lane
const int maxgap = 300*Dx; // the maximum gap size in calculategapsl
#endif