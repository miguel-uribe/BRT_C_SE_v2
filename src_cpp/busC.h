#ifndef BUS_C
#define BUS_C
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "parameters.h"
#include "createsystem.h"
#include "linesC.h"
//#include "passengerC.h"
#include <array>
#include <cmath>
#include <deque>
#include <random>



// creating a bus
void createbus(std::vector<int>& Parked, int LINEID, System& SYSTEM, std::vector<int> BUSESPAR[Nparam], std::vector<bool> BUSESBOOL[Nbool], int TIME){
    // we check there are buses available
    if(!Parked.empty()){  
        // we append the bus parameters at the end 
        BUSESPAR[0].push_back(SYSTEM.Lines[LINEID].origin); //position
        // setting the direction of the bus
        if (BUSESPAR[0].back()<L/2){ // going to the north
            BUSESPAR[17].push_back(0); // the bus direction
        }
        else{ // going to the south
            BUSESPAR[17].push_back(1); // the bus direction
        }
        // setting the origin lane
        if (BUSESPAR[17].back()==0){ // if going to the north the origin lane depends on the or_dest parameter
            BUSESPAR[1].push_back(SYSTEM.Lines[LINEID].or_dest); // lane
        }
        else{ // if going to the south all buses start at the main lane
            BUSESPAR[1].push_back(0); // lane
        }
        BUSESPAR[18].push_back(SYSTEM.Lines[LINEID].or_dest); // the origin or destination
        BUSESPAR[2].push_back(0); // speed
        BUSESPAR[3].push_back(0); // gapf
        BUSESPAR[4].push_back(0); // gapfl
        BUSESPAR[5].push_back(0); // gapbl
        BUSESPAR[6].push_back(0); //vbef
        BUSESPAR[10].push_back(LINEID); //lineID
        BUSESPAR[11].push_back(0); // stoptime
        BUSESPAR[12].push_back(0); // dwell time
        BUSESPAR[13].push_back(Parked.back()); // The bus ID
        BUSESPAR[14].push_back(TIME); //the initial time
        
        if (SYSTEM.Lines[LINEID].biart == 0){
            BUSESPAR[15].push_back(busL[0]); // The size of the bus
        }
        else if (SYSTEM.Lines[LINEID].biart == 1)
        {
            BUSESPAR[15].push_back(busL[1]); // the size of the bus
        }
        else{
            float r = ((float) rand() / (RAND_MAX));
            if (r > SYSTEM.Lines[LINEID].biart){
                BUSESPAR[15].push_back(busL[0]);}
            else{
                BUSESPAR[15].push_back(busL[1]);}
        }
        // setting the kind of route
        if(SYSTEM.Lines[LINEID].wagons.size() > 0){ // if there is a kind assigned to the service
        BUSESPAR[16].push_back(SYSTEM.Lines[LINEID].wagons[0]); // the kind of route
        }
        else{BUSESPAR[16].push_back(6);}; // if there are no stops, the kind is 6

        //BUSESPAR[17].push_back(0); // the time spent by the bus at the measurement window
        BUSESBOOL[0].push_back(true); //advancing

            
        // we retrieve the first stop information
        if (SYSTEM.Lines[LINEID].stopx.size()>0){ // in case there are stops
            BUSESPAR[7].push_back(SYSTEM.Lines[LINEID].stopx[0]); // the position of the next stop
            BUSESPAR[8].push_back(SYSTEM.Lines[LINEID].stationIDs[0]); // the ID of the next station
            BUSESPAR[9].push_back(0); // next station index
            
        }
        else{ // the nonstop buses
            BUSESPAR[7].push_back(1e6); // the position of the next stop
            BUSESPAR[8].push_back(-1); // the position of the next stop
            BUSESPAR[9].push_back(-1); // next station index    
        }

        BUSESPAR[19].push_back(0); // whether the bus is willing or looking to change lanes
        /*
        for (int i=0; i<Nparam; i++){
            std::cout<<BUSESPAR[i].back()<<" ";
        }
        std::cout<<std::endl;*/
        // We remove the bus from the parked list
        Parked.pop_back();
    }
    // If there are no buses, we must throw a warning
    else{
        std::cout<<"WARNING! There entire fleet is used, please increase the fleet number"<<std::endl; 
    }
} 




void sortbuses(std::vector<int> BUSESPAR[Nparam], std::vector<bool> BUSESBOOL[Nbool], std::vector<int> &index){

    // buses are only sorted if there are more than one buses
    if (BUSESPAR[0].size()>1){
        //std::cout<<"Sorting "<<BUSESPAR[0].size()<<std::endl;
        std::vector<int> idx(BUSESPAR[0].size());
        std::iota(idx.begin(), idx.end(),0);
        /*for (int i=0; i<idx.size(); i++){
            std::cout<<idx[i]<<" ";
        }*/
        //sorting indexes
        
        std::sort(idx.begin(),idx.end(),
        [&BUSESPAR](int i1, int i2){
            if (BUSESPAR[0][i1]==BUSESPAR[0][i2]){ //if the buses are in the same position, the order is given according to the index i2<i1 
                return i1>i2;
            }
            else{
                return BUSESPAR[0][i1]<BUSESPAR[0][i2]; // if they are not in the same position, the order is given according to the position
            }
        }
        );


        // we now check whether there is a change in order
        if (index!=idx){ // in case there is a change in order
            //std::cout<<"there is a change"<<std::endl;
            // once sorted, we proceed to update all the arrays
            for (int j =0; j<Nparam; j++){
                std::vector<int> aux;
                for (int i =0; i<idx.size(); i++){
                    aux.push_back(BUSESPAR[j][idx[i]]);
                }
                BUSESPAR[j]=aux; 
            }
            // std::cout<<"there is a change"<<std::endl;
            for (int j =0; j<Nbool; j++){
                std::vector<bool> aux;
                for (int i =0; i<idx.size(); i++){
                    aux.push_back(BUSESBOOL[j][idx[i]]);
                    }
                BUSESBOOL[j]=aux; 
            }

        }
        // We finally update the index
        index = idx;
    }
}

void updatestop(int index, std::vector<int> BUSESPAR[Nparam], System & SYSTEM){
    int line=BUSESPAR[10][index];
    int i=BUSESPAR[9][index]+1; // updating the index to the next station
    if (i>=SYSTEM.Lines[line].stopx.size()){    // in case the final station is reached
        BUSESPAR[7][index]=1e6; // we set the next stop to out of bounds
        BUSESPAR[8][index]=-1;  // the next station ID
        BUSESPAR[9][index]=-1;   // the next station index
    }
    // Otherwise all paraneters are updated
    else{
        BUSESPAR[7][index]=SYSTEM.Lines[line].stopx[i];
        BUSESPAR[8][index]=SYSTEM.Lines[line].stationIDs[i];
        BUSESPAR[9][index]=i;
    }
}

// This function calculates the forward gaps for all buses
void calculategaps(std::vector<int> BUSESPAR[Nparam], std::vector<bool> BUSESBOOL[Nbool], std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & EL,  std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & LC,  std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & RC){
    int x,y,kind, lineID, dir, ordest;
    int lme=-1; // last main east
    int lse=-1; // last stopping east

    // now we proceed to scan all buses from east to west
    for (int i=0; i<BUSESPAR[0].size(); i++){
        x = BUSESPAR[0][i];
        y = BUSESPAR[1][i];
        kind = BUSESPAR[16][i];
        dir = BUSESPAR[17][i];
        ordest = BUSESPAR[18][i];

        // Buses in the main lane
        if(BUSESPAR[1][i]==0){
            //the default gap corresponds to the next end of lane
            BUSESPAR[3][i] = EL[dir][ordest][kind][y][x];
            // If the bus is in the main lane, the gap is reduced by a term related to the bus' length
            if (y==0){ BUSESPAR[3][i]=std::max(BUSESPAR[3][i]-(BUSESPAR[15][i]-busL[0]),0);}

            // now we check for possible buses
            if (lme>=0){ // if already a bus has been found in the main lane to the east
                // we update the gap for the bus behind, cannot be negative
                BUSESPAR[3][lme]=std::max(std::min(BUSESPAR[3][lme],BUSESPAR[0][i]-BUSESPAR[0][lme]-BUSESPAR[15][i]),0);
            }
            
            // now checking if gaps must be changed to surrender priority
            if ((BUSESPAR[3][i]==0) && (BUSESPAR[19][i]==1)){ // if the bus is willing to change, but it's stuck
                // We find the distance to the last bus in the stopping lane 
                if(lse>=0){ //only if there are already buses found
                    int gap = BUSESPAR[0][i]-BUSESPAR[0][lse]-BUSESPAR[15][i];
                    if((gap>0) && (gap<=BUSESPAR[15][i]) && (BUSESPAR[2][lse]<=vsurr)){
                        BUSESPAR[3][lse]=0;
                    }
                }
            }
            
            // now we update the last index
            lme=i;
        }
        else { // Buses in the stopping lane
            // The default gap corresponds to the next station or next lane end, cannot be negative
            BUSESPAR[3][i]=std::max(std::min(BUSESPAR[7][i]-BUSESPAR[0][i],EL[dir][ordest][kind][y][x]),0);
            // If it is not the first bus found in the stopping lane
            if (lse>=0){
                // we set the gap for the bus behind
                BUSESPAR[3][lse]=std::max(std::min(BUSESPAR[3][lse],BUSESPAR[0][i]-BUSESPAR[0][lse]-BUSESPAR[15][i]),0);
            }
            
            // now checking if gaps must be changed to surrender priority
            if ((BUSESPAR[3][i]==0) && (BUSESPAR[19][i]==1)){ // if the bus is willing to change, but it's stuck
                // We find the distance to the last bus in the main lane
                if(lme>=0){ //only if there are already buses found
                    int gap = BUSESPAR[0][i]-BUSESPAR[0][lme]-BUSESPAR[15][i];
                    if((gap>0) && (gap<=BUSESPAR[15][i]) && (BUSESPAR[2][lme]<=vsurr)){
                        BUSESPAR[3][lme]=0;
                    }
                }
            }
            // we finally update the index
            lse=i;
        }
    }
   // std::cout<<"calculated gaps"<<std::endl;
}


// the function to be called when attempting to change lane to calculate the lateral gaps
void gapsl(int index, std::vector<int> BUSESPAR[Nparam]){

    // the default gaps
    int gaps[]={1000,-1000}; // forward, backward
    int vbef =0;

    // We determine the lane of the bus and the opposite:
    int lane=BUSESPAR[1][index];
    int opplane=(lane+1)%2;   // The opposite lane

    // GAP FORWARD
    int checks=1;
    while((checks<=ncheck) && ((index+checks)<BUSESPAR[0].size())){    
        
        // we get the gap
        int gap = BUSESPAR[0][index+checks]-BUSESPAR[0][index]-BUSESPAR[15][index+checks]; // this is positive
        // only if the gap is lower than two times the length of the biarticulated buses
        if(gap <= maxgap){
            if(BUSESPAR[1][index+checks]==opplane){
                // the bus is in the opposite plane
                gaps[0]=gap;
                break;
            }  
            
        }
        else{ // the rest of buses are further away, we stop
            break;
        }
        checks++;
    }

    // GAP bakcwards
    checks=-1;
    while((checks>=-ncheck) && ((index+checks)>=0)){
        // we get the gap
        int gap = BUSESPAR[0][index+checks]-BUSESPAR[0][index]+BUSESPAR[15][index]; // this is negative
        // only if the gap is lower than two times the length of the biarticulated buses
        if (gap>=-(maxgap)){
            // the bus is in the opposite plane
            if(BUSESPAR[1][index+checks]==opplane){
                gaps[1]=gap;
                // in addition, we set vbef
                vbef = BUSESPAR[2][index+checks];
                break;
            }  
        }
        else{ // the rest of buses are further away, we stop
            break;
        }
        checks--;
    }
    BUSESPAR[4][index]=gaps[0];
    BUSESPAR[5][index]=gaps[1];
    BUSESPAR[6][index]=vbef;
}


// This function creates all the line changes in the system   
void buschangelane(std::vector<int> BUSESPAR[Nparam], std::vector<bool> BUSESBOOL[Nbool], System& SYSTEM, std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & LC, std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & RC,
std::array<std::array<std::array<std::array<std::array<int, L>, Nmax>, nkind>, nordest>, ndir> & EL){
    int x, y, kind, dir, ordest, lane;
    std::vector<int> newy = BUSESPAR[1]; // by default, the bus stays in the same lane
    for (int i=0; i<BUSESPAR[0].size(); i++){
        // the bus parameters
        x = BUSESPAR[0][i];
        y = BUSESPAR[1][i];
        kind = BUSESPAR[16][i];
        dir = BUSESPAR[17][i];
        ordest = BUSESPAR[18][i];
        // Change to the stopping lane, we check both the origin and the end of the bus are free to change
        if (y == 0){
            // we check the change is allowed
            if (std::accumulate(LC[dir][ordest][kind][y].begin() + x-BUSESPAR[15][i]+1, LC[dir][ordest][kind][y].begin() + x + 1, 0)==BUSESPAR[15][i]){
                // we check whether the bus is willing to change, the EL is larger in the destination lane
                if (EL[dir][ordest][kind][y][x]<EL[dir][ordest][kind][1][x]){
                    // There are three possibilities to move to the stopping lane
                    // 1) The bus is close to its stop
                    // 2) The bus is far from its stop but there are no obstacles to their stop
                    // 3) The bus is going south and it is close to the end of lane
                    // 4) The bus is going south, far from the end of lane, but there are no obstacles
                    // we calculate the gaps
                    gapsl(i, BUSESPAR);
                    // 1) The bus is close to its stop
                    if ((BUSESPAR[7][i]-x)<(2*busL[1]+2*cld-BUSESPAR[15][i])){
                        // we update the change willing parameter
                        BUSESPAR[19][i] = 1;
                        // we calculate the gaps
                        // gapsl(i, BUSESPAR);
                        // If the gaps satisfy the following conditions
                        if ((-BUSESPAR[5][i]>BUSESPAR[6][i]) // -gapbl > vbef
                            && (BUSESPAR[4][i]>BUSESPAR[2][i]) // gapfl > v
                            && (BUSESPAR[4][i]>=cld) // gapfl >= cld
                            ){ 
                                // the lane is changed
                                newy[i] = 1;
                                // the change willing is returned to zero
                                BUSESPAR[19][i]=0;
                                /*if (BUSESPAR[13][i]==470){
                                    std::cout<<"cambio a stop por regla 1 en "<<BUSESPAR[14][i]<<std::endl;
                                }*/
                        }
                         
                    }
                    // 2) The bus is far but there are no obstacles to their stop
                    else if (BUSESPAR[4][i]>=(BUSESPAR[7][i]-busL[1]))
                    {
                        // we update the change willing parameter
                        BUSESPAR[19][i] = 1;
                        // we calculate the gaps
                        // gapsl(i, BUSESPAR);
                        // If the gaps satisfy the following conditions
                        if (-BUSESPAR[5][i]>BUSESPAR[6][i] // -gapbl > vbef
                            ){ 
                                // the lane is changed
                                newy[i] = 1;
                                // the change willing is returned to zero
                                BUSESPAR[19][i]=0;
                                /*if (BUSESPAR[13][i]==470){
                                    std::cout<<"cambio a stop por regla 2 en"<<BUSESPAR[14][i]<<std::endl;
                                }*/
                         }
                    }

                    // 3) The bus is going south, with destination NQS, and it is close to the end of lane, it has already stopped
                    else if ((ordest==1) && (dir==1) && (EL[dir][ordest][kind][y][x]<2*busL[1]) & (BUSESPAR[7][i]>10000)){
                        // we update the change willing parameter
                        BUSESPAR[19][i] = 1;
                        // we calculate the gaps
                        // gapsl(i, BUSESPAR);
                        // If the gaps satisfy the following conditions
                        if ((-BUSESPAR[5][i]>BUSESPAR[6][i]) // -gapbl > vbef
                            && (BUSESPAR[4][i]>BUSESPAR[2][i]) // gapfl > v
                            && (BUSESPAR[4][i]>=cld) // gapfl >= cld
                            ){ 
                                // the lane is changed
                                newy[i] = 1;
                                // the change willing is returned to zero
                                BUSESPAR[19][i]=0;
                                /*if (BUSESPAR[13][i]==470){
                                    std::cout<<"cambio a stop por regla 3 en"<<BUSESPAR[14][i]<<std::endl;
                                }*/
                         }
                    }

                    // 4) The bus is going south, far from the end of lane, but there are no obstacles, it has already stopped
                    else if ((ordest==1) && (dir==1) & (BUSESPAR[4][i]>(EL[dir][ordest][kind][y][x])) & (BUSESPAR[7][i]>10000))
                    {
                         // we update the change willing parameter
                        BUSESPAR[19][i] = 1;
                        // we calculate the gaps
                        //gapsl(i, BUSESPAR);
                        // If the gaps satisfy the following conditions
                        if (-BUSESPAR[5][i]>BUSESPAR[6][i] // -gapbl > vbef
                            ){ 
                                // the lane is changed
                                newy[i] = 1;
                                // the change willing is returned to zero
                                BUSESPAR[19][i]=0;
                                /*if (BUSESPAR[13][i]==470){
                                    std::cout<<"cambio a stop por regla 4 en"<<BUSESPAR[14][i]<<std::endl;
                                }*/
                         }
                    }                    
                }
            }
        }
        
        // Change to the main lane
        else{
            // we check the change is allowed
            if (std::accumulate(RC[dir][ordest][kind][y].begin() + x-BUSESPAR[15][i]+1, RC[dir][ordest][kind][y].begin() + x + 1, 0)==BUSESPAR[15][i]){
            //if((RC[dir][ordest][kind][y][x]==1) && (RC[dir][ordest][kind][y][x-BUSESPAR[15][i]+1]==1)){
                // we calculate the gaps
                gapsl(i, BUSESPAR);
                // once the bus has stopped, the EL is larger in the main lane and the bus is willing to change
                if (EL[dir][ordest][kind][y][x]<EL[dir][ordest][kind][0][x]){
                    // we update the change willing parameter
                    BUSESPAR[19][i] = 1;
                    // we calculate the gaps
                    // gapsl(i, BUSESPAR);
                    // If the gaps satisfy the following conditions
                    if ((-BUSESPAR[5][i]>BUSESPAR[6][i]) // -gapbl > vbef
                    && (BUSESPAR[4][i]>BUSESPAR[2][i]) // gapfl > v
                    && (BUSESPAR[4][i]>=cld)){ // gapfl >= cld
                        // the lane is changed
                        newy[i] = 0;
                        // the change willing is returned to zero
                        BUSESPAR[19][i]=0;
                        /*if (BUSESPAR[13][i]==470){
                            std::cout<<"cambio a main por regla 1 en"<<BUSESPAR[14][i]<<std::endl;
                        }*/
                    }
                }
                // another possibility arises if the bus is in the stopping lane because it arrived in the stopping lane
                // the change here is driven by two conditions
                // the bus is still far from its stop
                // there is more space in the main lane than in the stopping lane
                else if((BUSESPAR[3][i]<BUSESPAR[4][i]) & (BUSESPAR[3][i]<EL[dir][ordest][kind][0][x]) & ((BUSESPAR[7][i]-x)>distmin))
                {
                    // we update the change willing parameter
                    BUSESPAR[19][i] = 1;
                    // we calculate the gaps
                    // gapsl(i, BUSESPAR);
                    // If the gaps satisfy the following conditions
                    if ((-BUSESPAR[5][i]>BUSESPAR[6][i]) // -gapbl > vbef
                    && (BUSESPAR[4][i]>BUSESPAR[2][i]) // gapfl > v
                    && (BUSESPAR[4][i]>=cld)){ // gapfl >= cld
                        // the lane is changed
                        newy[i] = 0;
                        // the change willing is returned to zero
                        BUSESPAR[19][i]=0;
                        /*if (BUSESPAR[13][i]==470){
                            std::cout<<"cambio a main por regla 2 en"<<BUSESPAR[14][i]<<std::endl;
                        }*/
                    }
                }
                // For buses going south, with destination NQS, there is also the possibility to move to the main lane in case they find an obstacle, only if they have already stopped and they are far from the end of lane
                else if((BUSESPAR[7][i]>100000) && (ordest==1) && (dir==1) && (BUSESPAR[3][i]<BUSESPAR[4][i]) && (BUSESPAR[3][i]<EL[dir][ordest][kind][0][x]) && (EL[dir][ordest][kind][0][x]>2*busL[1])){
                    // we update the change willing parameter
                    BUSESPAR[19][i] = 1;
                    // we calculate the gaps
                    // gapsl(i, BUSESPAR);
                    // If the gaps satisfy the following conditions
                    if ((-BUSESPAR[5][i]>BUSESPAR[6][i]) // -gapbl > vbef
                    && (BUSESPAR[4][i]>BUSESPAR[2][i]) // gapfl > v
                    && (BUSESPAR[4][i]>=cld)){ // gapfl >= cld
                        // the lane is changed
                        newy[i] = 0;
                        // the change willing is returned to zero
                        BUSESPAR[19][i]=0;
                        /*if (BUSESPAR[13][i]==470){
                            std::cout<<"cambio a main por regla 3 en"<<BUSESPAR[14][i]<<std::endl;
                        }*/
                    }
                }                
            }
        }
    }
    // Now we update all the lanes
    BUSESPAR[1]=newy;
    
    //std::cout<<"Changed lane"<<std::endl;
}


// making the buses move
void busadvance(std::vector<int> BUSESPAR[Nparam], std::vector<bool> BUSESBOOL[Nbool], System& SYSTEM, int TIME, std::vector<int> &PARKED, std::array<std::array<int, L>, Nmax> & V, std::default_random_engine &generator, bool measuring){
    int x,y, lineID, dt, intdt;
    float prand, mean, std;
    //std::cout<<"Bus advanced "<<BUSESPAR[0].size()<<std::endl;
    // we scan over all buses
    std::vector<int> toremove;
    for (int i =0; i<BUSESPAR[0].size(); i++){
        x = BUSESPAR[0][i];
        y = BUSESPAR[1][i];
        lineID = BUSESPAR[10][i];

        // buses advancing
        if (BUSESBOOL[0][i]==true){
            // we implement the VDR-TCA model of Maerivoet
            // we determine the randomization parameter depending on the speed
            if (BUSESPAR[2][i]==0)
                prand = p0;
            else
                prand = p;
            // we calculate the new speed, vnew=acc*min(acc*(v+acc),acc*gap,acc*vmax)
            BUSESPAR[2][i]=std::min((BUSESPAR[2][i]+1),std::min(BUSESPAR[3][i],V[y][x]));
            // we apply the randomization
            float r = ((float) rand() / (RAND_MAX));
            if (r<prand){BUSESPAR[2][i] = std::max(0,BUSESPAR[2][i]-d_speed);}
            // now we update the position
            BUSESPAR[0][i]=BUSESPAR[0][i]+BUSESPAR[2][i];

/*
            // checking whether the bus entered a station
            if (BUSESPAR[0][i]>=SYSTEM.Lines[lineID].measBeg){ //The new position is located after the beginning of the station
                    if(x<SYSTEM.Lines[lineID].measBeg){ // The last position was located before the beginning of the station
                        BUSESPAR[17][i] = TIME;
                    }
                }
        
            
            // checking whether the bus left a station
            if (BUSESPAR[0][i]>=SYSTEM.Lines[lineID].measEnd){ //The new position is located after the beginning of the station
                    if(x<SYSTEM.Lines[lineID].measEnd){ // The last position was located before the beginning of the station
                        SYSTEM.Lines[lineID].times.push_back(TIME - BUSESPAR[17][i]); // getting the information of the time spent at the station
                }
            }
*/
            // checking whether the bus reaches a stop
            if (BUSESPAR[0][i]==BUSESPAR[7][i]){
                // we set the speed, advancing and stoptime to zero
                BUSESPAR[2][i]=0;
                BUSESBOOL[0][i]=false;
                BUSESPAR[11][i]=0;
                // we board and alight passengers, and set the dwell time
                if (SYSTEM.Lines[lineID].lasttime>0){ // if this is not the first bus
                    dt = TIME - SYSTEM.Lines[lineID].lasttime; // we calculate the headway
                }
                else{ // For the first bus we do not count
                    dt = 0; // Default value
                }
                // we get the headway slot
                intdt = std::min(ndist-1, dt/step);
                // we generate the dwell time, can never be smaller than dtmin
                BUSESPAR[12][i] = std::max((int) SYSTEM.Lines[lineID].distributions[intdt](generator)+0.5, 1.0*dtmin);
                if(measuring==true){ // only if we are measuring we add the results
                        //std::cout<<"here"<<std::endl;
                        SYSTEM.Lines[lineID].dwells.push_back(BUSESPAR[12][i]); //we add the dwell time to the list
                        SYSTEM.Lines[lineID].dts.push_back(dt); //we add the headway to the list
                    }
                //std::cout<<lineID<<" "<<BUSESPAR[12][i]<<std::endl;
                //std::cout<<BUSESPAR[12][i]<<" "<<lineID<<std::endl;
                // we update the stop information
                updatestop(i,BUSESPAR,SYSTEM);
                // We update the last stop information
                SYSTEM.Lines[lineID].lasttime = TIME;
            }

            // checking whether the bus leaves the system
            else if (BUSESPAR[0][i]>SYSTEM.Lines[BUSESPAR[10][i]].end){
                // the time is added to the service's times
                if(measuring==true){ // only if we are measuring we add the time to the list
                     SYSTEM.Lines[lineID].times.push_back(TIME - BUSESPAR[14][i]);
                }
                //  the bus is removed and added to the parked list
                toremove.push_back(i);
                PARKED.push_back(BUSESPAR[13][i]);
            }
        }
        // if the bus is not advancing
        else{
            //the stop time is increased by one
            BUSESPAR[11][i]=BUSESPAR[11][i]+1;
            // we check that the stopping time is equal or larger than the dwell time
            if (BUSESPAR[11][i]>=BUSESPAR[12][i]){
                // twe update the changing and the advancing
                BUSESBOOL[0][i]=true;
            }
        }   
    }
    
    // we now remove the buses that left the system
    while(!toremove.empty()){
        int i=toremove.back();
        for (int j=0; j<Nparam; j++){
            BUSESPAR[j].erase(BUSESPAR[j].begin()+i);
        }
        for (int j=0; j<Nbool; j++){
            BUSESBOOL[j].erase(BUSESBOOL[j].begin()+i);
        }
        toremove.pop_back();
    }
}



// inserting the buses in the system
void populate(std::vector<int>& Parked, std::vector<int> BUSESPAR[Nparam], System& SYSTEM, std::vector<bool> BUSESBOOL[Nbool], std::default_random_engine &generator, std::vector<std::poisson_distribution<int>> & distr, int time){
    int nbus;
    // Now we check and see whether it is time to populate
    // we scan over the lines   
    for (int i=0; i<NLines; i++){
        // determining how many buses must be included for each line
        nbus = distr[i](generator);
        if (nbus>0){ // if there are buses to add
            //std::cout<<nbus<<" "<<i<<std::endl;
            createbus(Parked, i, SYSTEM, BUSESPAR, BUSESBOOL, time);
        }
    }
}

#endif