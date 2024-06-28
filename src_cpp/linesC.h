#ifndef LINE_C
#define LINE_C

#include "parameters.h"
#include "stationC.h"
#include <cstdlib>
#include <vector>
#include <random>

class lineC{
    public:
        std::string name;  // The name of the bus service
        std::vector<int> stationIDs; // A list of station IDs where the service stops
        std::vector<int> wagons; // A list with the wagon ID at each station where the service stops
        std::vector<int> stopx; // A list with the positions of the stops where the service stops
        std::vector<int> dts; // A list with the measured headways at the station
        std::vector<int> dwells; // A list with the computed dwell times at the station
        std::vector<int> times; // A list with the time spent at the simulation by each bus
        int origin; // The position where buses serving this service are introduced
        int end; // The position where the service ends
        int or_dest; // the origin or destination of the service, 0 -> Caracas, 1-> NQS
        int headway; // The average headway for the service
        int dwelltime; // The base dwell time for the service
        float dwellhway; // The d (dwelltime)/d(headway) ratio
        float variance; // the width of the dwell time distribution
        float variancehway; // the variation of the variance with the headway
        float biart; // percentage of biarticulated buses
        int lasttime = 0; // The last time a bus arrived at the station
        std::vector<std::normal_distribution<double>> distributions; // the vector of distributions
        // Dummy constructor
        lineC(){}
        // real constructor
        lineC(std::string NAME){
            name = NAME;
        }
        std::string display (void);
        void setstopx(std::vector<int> &STATIONIDS, std::vector<int> &WAGONS, stationC* STATIONS);
        void setorigin(int ORIGIN);
        void updatedistributions(void);
};

std::string lineC::display (void){
    std::string text = "Line "+ name;
    text = text + ". Headway: " + std::to_string(headway);
    text = text + ". Origin: " + std::to_string(origin);
    text = text + ". End: " + std::to_string(end);
    text = text + ". Dwelltime: " + std::to_string(dwelltime);
    text = text + ". Dwelltime/headway: " + std::to_string(dwellhway);
    text = text + ". Dwelltime variance: " + std::to_string(variance);
    text = text + ". Dwelltime variance/headway: " + std::to_string(variancehway);
    text = text + ". Biarticulated proportion: " + std::to_string(biart);
    text = text + ". Stopping at stations: ";
    for(int i =0; i<stationIDs.size(); i++){
        text = text + std::to_string(stationIDs[i]) +"("+std::to_string(wagons[i])+", "+std::to_string(stopx[i])+")\n";
    }
    return text;
}

void lineC::updatedistributions(void){
    // we define the number of distributions to create
    distributions.resize(ndist);
    for (int i = 0; i < distributions.size(); ++i) {
            double H = (i + 0.5) * 15;
            double mean = dwelltime + dwellhway * H;
            double stddev = variance + variancehway * H;
            distributions[i] = std::normal_distribution<double>(mean, stddev);
    }
}

void lineC::setstopx(std::vector<int> &STATIONIDS, std::vector<int> &WAGONS, stationC* STATIONS){
    stationIDs = STATIONIDS;
    wagons = WAGONS;
    for (int i = 0; i< stationIDs.size(); i++){
        stopx.push_back(STATIONS[stationIDs[i]].stop_pos[wagons[i]]);
    }
}




/*
def getdestinations(Stations,stationID,Lines):
    # The destinations, lines and origins lists    
    destinationIDs=[]
    lineIDs=[]
    originIDs=[]
    # The origin station
    origin=stationC.getstationbyID(Stations,stationID)
    
    # scanning all the lines stopping in origin
    for lineID in origin.lineIDs:
        # Retrieving the line from lineID
        line=getlinebyID(Lines,lineID)
        # Retrieving the corresponding index for the given station
        i=line.stationIDs.index(stationID)
        # Checking that the index is not the last one
        if i<(len(line.stationIDs)-1):
            # Adding the next station to the destination ID
            destinationIDs.append(line.stationIDs[i+1])
            # Adding the line information
            lineIDs.append(line.ID)
            # Adding the origin information
            originIDs.append(stationID)
    return [destinationIDs,lineIDs,originIDs]*/

/*

class lineC:
    "line class containing all information of a given line"
#    n=0  #the number of lines in the system
    def __init__(self,name,stationIDs,stops,Stations,acc,ID):
        if(len(stops)!=len(stationIDs)):
            print("LineC: The number of stations and the vector of their position do not have the same size")
        self.name=name
        self.stops=stops
        self.stationIDs=stationIDs
        #stationC.updatewagons(Stations,stationIDs,stops)
        self.stopx=[]
        self.acc=acc
        self.ID=ID
#        lineC.n=lineC.n+1
        self.setstopx(Stations)
                
    # Overrides the representation function
    def __repr__(self):
        text="%d: Line %s stopping in stations with ID: "%(self.ID,self.name)
        for i in range(len(self.stationIDs)):
            text=text+" %d(%d);"%(self.stationIDs[i],self.stops[i])
        return text
    
    # Sets the position of all stops of the line
    def setstopx(self,Stations):
        for i in range(len(self.stationIDs)):
            for station in Stations:
                if station.ID==self.stationIDs[i]:
                    stopx=station.x+(self.stops[i]-1)*parameters.Ds+parameters.Dw*(self.acc+abs(self.acc))/(2*abs(self.acc))
                    self.stopx.append(stopx)
                    # Adding the line ID to the station
                    station.lineIDs.append(self.ID)

                    
                    
def getlineindexbyID(Lines,ID):
    for i in range(len(Lines)):
        if Lines[i].ID==ID:
            return i
    print("The given line ID has not been found in the Lines list")
    
def getlinebyID(Lines,ID):
    for line in Lines:
        if line.ID==ID:
            return line
    print("The given line ID has not been found in the Lines list")
    
def getdestinations(Stations,stationID,Lines):
    # The destinations, lines and origins lists    
    destinationIDs=[]
    lineIDs=[]
    originIDs=[]
    # The origin station
    origin=stationC.getstationbyID(Stations,stationID)
    
    # scanning all the lines stopping in origin
    for lineID in origin.lineIDs:
        # Retrieving the line from lineID
        line=getlinebyID(Lines,lineID)
        # Retrieving the corresponding index for the given station
        i=line.stationIDs.index(stationID)
        # Checking that the index is not the last one
        if i<(len(line.stationIDs)-1):
            # Adding the next station to the destination ID
            destinationIDs.append(line.stationIDs[i+1])
            # Adding the line information
            lineIDs.append(line.ID)
            # Adding the origin information
            originIDs.append(stationID)
    return [destinationIDs,lineIDs,originIDs]
        
    
    */
#endif