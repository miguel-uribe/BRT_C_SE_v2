import numpy as np
Dt=1.0     # This is the time unit
Dx=1.0     # This is the space unit meters/cell
Db=[18,27]     # The car length, in m, articulated and biarticulated
vmax=17          # the default maximal speed, in m/s
vmaxSL= 8 # the default maximal speed at the stopping lane, in m/s
acc = 1    # the acceleration in Dx/Dt^2
Nmax = 2  # the maximal number of lanes
sL = 236 # the length of the station, in meters
dlN = 60 # extension of the double lane after the end of the station, to the north
dlS = 100 # extension of the double lane after the end of the station, to the south
dLB = 1000 # The space before the station, in meters
dLA = 300 # The space after the station, in meters
DLBN = -100 # The position where the double lane begins, respect to the station beginning in meters, to the north
DLBS = -60  # The position where the double lane begins, respect to the station beginning in meters, to the south
DLLN = 396 # the length of the double lane, to the north
DLLS = 346 # the length of the double lane, to the south
bL = 100 # The position where buses will be introduced to the system, counted from the beginning of the simulation
rbL = -50 # The position where buses will be removed from the system, counted from the end of the simulation
dbL = 30 # The distance behind the last bus to introduce the next one, in case the position bl is occupied
L = dLB+dLA+sL # The total length of the system
dA = -15 # The distance after the stop where a bus can return to the main lane
DMLN = -20 # The maximum distance that buses to the north can be in the stopping lane before changing to the main lane, relative to the beginning of the station
DMLS = 188 # The maximum distance that buses to the south can be in the stopping lane before changing to the main lane, relative to the beginning of the station
cld = 6 # change lane distance, the distance required by a bus to change the lane, in meters
wN = np.zeros((2,6)) # the position of the stops
wN[0,:] = np.array([40,59,113,132,188,207]) # to the north
wN[1,:] = np.array([50,69,126,145,198,217]) # to the south






