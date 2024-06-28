# %%
# Creating the service definition file

import numpy as np
import pandas as pd
import os
from route_conf import *
import matplotlib.pyplot as plt
import seaborn as sns
# this script creates the lane availability files for the 6 possible different configurations#%%
#########################################################
##### THE CONFIGURATION FILES FOR CARS IN THE MAIN HIGHWAY
#########################################################
# now we set the allowed lane changes
# LNA: Lane not available, this is to close lanes
# 0, the code, 1, the lane, 2, the position (in m) where the lane first becomes unavailable, 3, the position (in m) where the lane last is unavailable
# LSB: This is a separator that forbids the change between lanes
# 0, the code, 1, the lane with the separator to the left, 2, the position where the separator first appears, 3, the position where the separator last appears
# LSL: This is a separator that forbids the change to the left
# 0, the code, 1, the lane with the separator to the left, 2, the position where the separator first appears, 3, the position where the separator last appears
# LSR: This is a separator that forbids the change to the right
# 0, the code, 1, the lane with the separator to the right, 2, the position where the separator first appears, 3, the position where the separator last appears
# VM: The maximal speed rule
# 0, the code, 1, the lane for which the speed applies, 3, the initial position where the speed applies, 4, the final position where the speed applies, 5, the speed

# creating the maximal speed file, this is a number with the information regarding the maximal speed per lane and per position
V = (vmax*Dt/Dx)*np.ones((2*L,Nmax))  # by default the maximal speed is the same along the entire corridor
# At the station, the speed at the stopping lane is reduced
# to the south
V[dLB:dLB+sL,1] = (vmaxSL*Dt/Dx)
# to the north
V[L+dLB:L+dLB+sL,1] = (vmaxSL*Dt/Dx)
dirname = os.path.dirname(__file__)
file = os.path.join(dirname,"../conf/V.txt")
np.savetxt(file,V, fmt ='%d')  # the speed

for dir in [0,1]:
    # 0 -> South - North
    # 1 -> North - South
    if dir==1:
        begS= dLB + L # the beginning of the station
        endL = 2*L # the end of the simulation
    else:
        begS = dLB
    for dest in [0,1]:
        # 0 -> Caracas
        # 1 -> NQS
        for door in np.arange(6):
            confname = f'door_{door}_dir_{dir}_or_{dest}' if dir==0 else f'door_{door}_dir_{dir}_dest_{dest}'

            # we define some rules on the lane availability
            LCR = []
            
            # Depending on the origin, only one lane is available to the south of the station
            # services to the north from caracas
            if (dir == 0) & (dest==0):
                LCR.append(['LNA',1,0,begS+DLBN])
            # services to the north from NQS
            if (dir == 0) & (dest==1):
                LCR.append(['LNA',0,0,begS+DLBS])
                
            # When moving to the south, buses going to NQS should move to the stopping lane
            if (dir==1) & (dest==1):
                LCR.append(['LNA', 0, begS+DLBS+DLLS, endL])
            
            # When moving to the south, buses going to Caracas should move to the main lane 
            if (dir==1) & (dest==0):
                LCR.append(['LNA', 1, begS+DLBS+DLLS, endL])
            
            # the stopping lane not available on the north side of the station
            if dir == 0: # to the north
                LCR.append(['LNA', 1, begS+DLBN+DLLN, L])
            else: # to the south
                LCR.append(['LNA', 1, L, begS+DLBS])
            
            # the main lane disappears in the vecinity of the docking bay
            LCR.append(['LNA', 0, begS+wN[dir,door]-Db[0]-cld, begS+wN[dir,door]+dA])

            # When moving to the north buses coming from NQS should move to the main lane, unless they go to the first wagon
            if (door>1) & (dir ==0) & (dest==1):
                LCR.append(['LNA', 1, begS+DMLN, begS])   
                
            # When moving to the south, buses going to NQS should move back to the main lane, unless they go to the last wagon
            if (door<4) & (dir==1) & (dest==1):
                LCR.append(['LNA', 1, begS+DMLS, begS+sL])   
                
            


            
            # creating the lane availability file, this is 0 when a lane is not available and 1 when the lane is available
            lanes = np.ones((2*L,Nmax))   # by default, all lanes are available among all the corridor
            # correcting for the information provided in route_conf
            for rule in LCR:
                if rule[0]=='LNA':
                    lanes[round(rule[2]/Dx):round(rule[3]/Dx)+1,rule[1]] = 0


            # Now we create the leftchange file, containing the information regarding the posibility to change the lane to the left, this is increasing the lane. 0 means change is not possible, 1 means change is possible
            LC = np.copy(lanes)
            # clearly for the leftmost lane it is always impossible to move to the left
            LC[:,-1]=0
            # in addition, it is impossible to move to the left when the lane in the left is not there
            mask = lanes-np.roll(lanes, -1, axis = 1)==1
            LC[mask] = 0

            # Now we create the rightchange file, containing the information regarding the posibility to change the lane to the right, this is decreasing the lane. 0 means change is not possible, 1 means change is possible
            RC = np.copy(lanes)
            # clearly for the rightmost lane it is always impossible to move to the left
            RC[:,0]=0
            # in addition, it is impossible to move to the left when the lane in the left is not there
            mask = np.where(lanes-np.roll(lanes, 1, axis = 1)==1)
            RC[mask] = 0
            
            # Finally we create the endoflane where a number marks the distance to the end of the lane, by default this is set as 1000

            EL = 1000*np.ones((2*L,Nmax))
            auxEL = np.zeros((2*L,Nmax))
            mask =  (np.roll(lanes, 1, axis = 0) - lanes) == 1
            auxEL[mask] = 1

            for i in range(Nmax):
                endF = False
                for j in np.arange(2*L-1,-1,-1):
                    if endF:
                        EL[j,i] = endPos-j
                    if auxEL[j,i] == 1:
                        endF = True
                        endPos = j - 1



            # Exporting the information
            dirname = os.path.dirname(__file__)
            file = os.path.join(dirname,"../conf/lanes_"+confname+".txt")
            np.savetxt(file,lanes, fmt ='%d')  # the lane availability
            file = os.path.join(dirname,"../conf/LC_"+confname+".txt")
            np.savetxt(file,LC, fmt ='%d')  # the left change
            file = os.path.join(dirname,"../conf/RC_"+confname+".txt")
            np.savetxt(file,RC, fmt ='%d')  # the left change
            file = os.path.join(dirname,"../conf/EL_"+confname+".txt")
            np.savetxt(file,EL, fmt ='%d')  # the left change
            fig, ax = plt.subplots(1,2, figsize=(4,5))
            plt.title(confname)
            sns.heatmap(V, ax = ax[0])
            sns.heatmap(lanes, ax = ax[1])
        


initST = np.around((np.where(lanes[:,1]-np.roll(lanes[:,1], 1)==1)[0])/Dx).astype(int)
endST =np.around((np.where(lanes[:,1]-np.roll(lanes[:,1], 1)==-1)[0])/Dx).astype(int)
print(initST)
print(endST)
# %%
# Creating a file with the definition of all stops in the system
stop_definition = ['Calle_100_N', 'Calle_100_S']

text = ""
for i, stop in enumerate(stop_definition):
    text = text + "%d "%i+ stop +"\n"
dirname = os.path.dirname(__file__)
filename = os.path.join(dirname,"../conf/stoplist.txt")
file = open(filename, "w")
file.write(text)
file.close()

# %%
# Creating a file with the list of all the available stops in the system

dirname = os.path.dirname(__file__)
filename = os.path.join(dirname,"../conf/stopdefinition.txt")
file = open(filename, "w")
text = '0'
for i in range(6):
    text = text +" %d %d"%(dLB+wN[0,i], (i)%2)
text = text + '\n1'
for i in range(6):
    text = text +" %d %d"%(dLB+L+wN[1,i], (i)%2)
file.write(text)
file.close()
# %%
# Creating a file with the configuration of each service:

serv_dict = {   'B27':  [bL   ,L+rbL  ,0 , 5],
                '8N':   [bL   ,L+rbL  ,0 , 4],
                'B75':  [bL   ,L+rbL  ,0 , 3],
                'B26':  [bL   ,L+rbL  ,0 , 3],
                'B10':  [bL   ,L+rbL  ,0 , 3],
                'B12':  [bL   ,L+rbL  ,0 , 1],
                'B46':  [bL   ,L+rbL  ,0 , 1],
                'G12':  [bL+L ,2*L+rbL,1, 5],
                'G46':  [bL+L ,2*L+rbL,1, 5],
                '8S':   [bL+L ,2*L+rbL,1, 3],
                'H27':  [bL+L ,2*L+rbL,1, 3],
                'F26':  [bL+L ,2*L+rbL,1, 3],
                'D10':  [bL+L ,2*L+rbL,1, 1],
                'H75':  [bL+L ,2*L+rbL,1, 1],
                'GenS': [bL+L ,2*L+rbL],
                'GenN': [bL   ,L+rbL]
             }

# We now print the file with the service list
text = ""
for i, service in enumerate(serv_dict.keys()):
    text = text + "%d "%i + service + "\n"
dirname = os.path.dirname(__file__)
filename = os.path.join(dirname,"../conf/servicelist.txt")
file = open(filename, "w")
file.write(text)
file.close()
    


# now we print the file with the service definition
text = ""
stoplist = pd.read_csv('../conf/stoplist.txt', names = ['ID', 'name'], sep = " ")
servicelist = pd.read_csv('../conf/servicelist.txt', names = ['ID', 'name'], sep = " ")
for service_name, stops in serv_dict.items():
    print(service_name, stops)
    serviceID = servicelist[servicelist['name']==service_name]['ID'].values[0]
    print(serviceID)
    try: # if the service is stop
        text = text + "%d %d %d %d %d"%(serviceID,stops[0], stops[1], stops[2], stops[3])
        text = text + "\n"
    except: # if the service does not stop
        text = text + "%d %d %d"%(serviceID,stops[0], stops[1])
        text = text + "\n"
dirname = os.path.dirname(__file__)
filename = os.path.join(dirname,"../conf/servicedefinition.txt")
file = open(filename, "w")
file.write(text)
file.close()

# %%
# Creating a file with the average line headways
dirname = os.path.dirname(__file__)
filename = os.path.join(dirname,"../conf/servicedata.txt")
file = open(filename, "w")
# the list of headways
headways = [
    181, #B27
    247, #8N
    212, #B75
    244, #B26
    130, #B10
    116, #B12
    130, #B46
    155, #G12
    130, #G46
    295, #8S
    210, #H27
    236, #F26
    176, #D10
    242, #H75
    30, #GenS
    30, #GenN
]
# the list of dwell times
dwelltimes = [
    25, #B27
    22, #8N
    25, #B75
    25, #B26
    27, #B10
    29, #B12
    46, #B46
    37, #G12
    37, #G46
    27, #8S
    28, #H27
    26, #F26
    35, #D10
    28, #H75
    0, #GenS
    0, #GenN
    ]
# the list of headway dependent dwell time
dwelltimes2 = [
    0.0102, #B27
    0.0411, #8N
    0.0156, #B75
    0.0125, #B26
    0.0490, #B10
    0.0722, #B12
    0.0130, #B46
    0.0156, #G12
    0.0229, #G46
    0.0186, #8S
    0.0105, #H27
    0.0143, #F26
    0.0073, #D10
    0.0110, #H75
    0, #GenS
    0, #GenN
]
# the list of base variance of the dwell time
variance = [
    4.554, #B27
    2.993, #8N
    5.840, #B75
    4.165, #B26
    6.549, #B10
    7.146, #B12
    19.53, #B46
    10.28, #G12
    9.492, #G46
    8.697, #8S
    8.639, #H27
    4.878, #F26
    8.644, #D10
    7.251, #H75
    0, #GenS
    0, #GenN
]
# the list of headway dependent variance of the dwell time
variance2 = [
    0, #B27
    0.0143, #8N
    0, #B75
    0, #B26
    0.0144, #B10
    0.0282, #B12
    -0.0387, #B46
    0, #G12
    0, #G46
    0, #8S
    0, #H27
    0, #F26
    0, #D10
    0, #H75
    0, #GenS
    0, #GenN
]

# this list of biarticulated values
biart = variance2 = [
    1, #B27
    0, #8N
    1, #B75
    1, #B26
    1, #B10
    1, #B12
    1, #B46
    1, #G12
    1, #G46
    0, #8S
    1, #H27
    1, #F26
    1, #D10
    1, #H75
    0.64, #GenS
    0.64, #GenN
]

text = ""
for i,_ in enumerate(serv_dict.keys()):
    text =text + "%d %d %d %f %f %f %d\n"%(i,headways[i], dwelltimes[i], dwelltimes2[i], variance[i], variance[2], biart[i])

file.write(text)
file.close()
#np.savetxt(filename, np.hstack( (np.arange(len(serv_dict)).reshape(len(serv_dict),1),headways,dwelltimes, dwelltimes2, widths, biart)), fmt='%.4f')


# %%
