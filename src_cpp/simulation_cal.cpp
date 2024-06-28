#include <iostream>
#include "linesC.h"
//#include "routeC.h"
#include "createsystem.h"
#include "busC.h"
//#include "passengerC.h"
//#include "fleetsize.h"
#include <array>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>
#include <thread>
#include <future>


using namespace std;

int main (int argc, char **argv){

    // The arguments list
    // 1 - seed
    // 2 - whether the animation data should be exported

    //cout<<"Read the input matrix and transfer matrix"<< endl;
    /////////////////////////////////////////////////////////////
    // Creating the system
    System SYSTEM;
    std::string stoplist = "../conf/stoplist.txt";
    std::string stopdefinition = "../conf/stopdefinition.txt";
    std::string routelist = "../conf/servicelist.txt";
    std::string routedefinition = "../conf/servicedefinition.txt";
    std::string routeheadways = "../conf/servicedata.txt";
    
    //cout<<servicefile<<endl;
    SYSTEM = createsystem(stoplist, stopdefinition, routelist, routedefinition, routeheadways);


    /*for (auto line: SYSTEM.Stations){
        cout<<line.display()<<endl;
    }*/

    //cout<<"Created the system"<< endl;  

    // Checking whether the configuration is valid
    int biart, stID, wagID, biartP;
    bool valid = true;
    for (auto line: SYSTEM.Lines){
        biart = line.biart;
        if (biart > 0){
            for (int i =0; i< line.stationIDs.size(); i++){
                stID = line.stationIDs[i];
                wagID = line.wagons[i];
                biartP = SYSTEM.Stations[stID].stop_biart[wagID];
                if (biartP<1){
                    //cout<<"WARNING! A service with biarticulated buses is assigned to a stop without biarticulated support"<<endl;
                    //cout<<line.name<<" "<<stID<<" "<<wagID<<" "<<SYSTEM.Stations[stID].display()<<" "<<SYSTEM.Stations[stID].stop_biart[wagID]<<endl;
                    valid = false;
                }
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////
    /////////// Loading the lane information
    ///////////////////////////////////////////
    // Reading the configuration files
    // INFO:: In all these arrays, the first element addresses the lane, the second one the x position
    

    array<array<int, 2*L>, Nmax> V;
    V = loadconffile("V", 1.0*Dt/Dx);
    

    array<array<array<int, 2*L>, Nmax>, nkind> lanes, LC, RC, EL;
    lanes = loadconffilekind("lanes", 1);
    LC = loadconffilekind("LC", 1);
    RC = loadconffilekind("RC", 1);
    EL = loadconffilekind("EL", 1.0/Dx);
    
    //cout<<"Loaded the configuration files"<<endl;
    
    /////////////////////////////////////////////////////////////
    // the seed
    int seed = stoi(argv[1]);
    int print = stoi(argv[2]);
    std::default_random_engine generator (seed);
    srand(seed);
    //cout<<"Defined the seed"<< endl;   
    
    /////////////////////////////////////////////////////////////////
    ///////////////// Creating the distributions
    std::vector<std::poisson_distribution<int>> distr;
    for(int i = 0; i<NLines ; i++){
        //cout<<SYSTEM.Lines[i].headway<<endl;
        distr.push_back(std::poisson_distribution<int> (1.0/SYSTEM.Lines[i].headway));
    }

    

    ///////////////////////////////////////////////////////////////
    // Creating the bus array

    vector<int> index;
    vector<int> BusesPar[Nparam];
    vector<bool> BusesBool[Nbool];
    vector<int> Parked(fleet); // The list of parked buses
    std::iota(Parked.begin(), Parked.end(),0); // we fill the list with numbers

    //cout<<"Created the bus array"<<endl;
    /////////////////////////////////////////////////////////
    // exporting the data
    string filename, fileroot;
    fileroot = "./sim_results/sim_results";
    // adding the configuration information
    for (auto line: SYSTEM.Lines){
        if(line.wagons.size()>0){
            fileroot = fileroot +"_"+ to_string(line.wagons[0]);
        }
    }
    

    /////////////////////////////////////////////////////////
    // performing the simulation
    for (int i=0; i<NLines; i++){


    ofstream printfile; // must be declared outside
    if (valid==true){ // if the configuration is valid
        if (print == 1){
            int print = stoi(argv[2]);
            filename = fileroot + "_cardata.txt";
            printfile.open(filename, ios_base::out);
            
        }
        int nbuses = 0;
        bool measuring = false;
        for (int TIME=0; TIME<3*3600;TIME++){      
            // inserting the buses
            //cout<<t<<endl;
            calculategaps(BusesPar,BusesBool, EL, LC, RC);
            buschangelane(BusesPar, BusesBool,SYSTEM, LC, RC);
            calculategaps(BusesPar,BusesBool, EL, LC, RC);
            busadvance(BusesPar,BusesBool,SYSTEM,TIME, Parked, V, generator, measuring);
            populate(Parked, BusesPar, SYSTEM, BusesBool, generator, distr, TIME);
            sortbuses(BusesPar,BusesBool, index);
            if(print==1){
                if (TIME >= 3600){
                    for (int i = 0; i<BusesPar[0].size(); i++){
                        printfile<<TIME<<" "<<BusesPar[0][i]<<" "<<BusesPar[1][i]<<" "<<BusesPar[10][i]<<" "<<BusesPar[13][i]<<endl;
                    }  
                }
            }

            if ((measuring ==false) && (TIME==3600)){
                measuring = true;
            }
        }

        printfile.close();
        //cout<<"Performed the simulation"<< endl;
    }
    else{ // if the configuration is not valid
        for (auto &line: SYSTEM.Lines){
            line.times.push_back(1e6);
            line.dts.push_back(1e6);
        }
        //cout<<"There is no simulation to perform"<<endl;
    }

    /////////////////////////////////////////////////////////
    // exporting the data
     
    fileroot = fileroot + "_seed_" + to_string(seed);
    
    // exporting the times at the stations for each service
    filename = fileroot + "_times.txt";

    int maxdata = 0;
    string text = "";
    for (auto line: SYSTEM.Lines){
        text = text + line.name +",";
        if (line.times.size()>maxdata){
            maxdata = line.times.size();
        }
    }
    text = text + "\n";
    for (int i=0; i<maxdata; i++){
        for (auto line: SYSTEM.Lines){
            if (line.times.size()>i){
                text = text + to_string(line.times[i])+",";
            }
            else{
                text = text + "nan,";
            }
        }
        text = text + "\n";
    }

    ofstream headfile;
    headfile.open(filename, ios_base::out);
    headfile<<text;
    headfile.close();


    // exporting the headways for each service
    filename = fileroot + "_headways.txt";
    maxdata = 0;
    text = "";
    for (auto line: SYSTEM.Lines){
        text = text + line.name +",";
        if (line.dts.size()>maxdata){
            maxdata = line.dts.size();
        }
    }
    text = text + "\n";
    for (int i=0; i<maxdata; i++){
        for (auto line: SYSTEM.Lines){
            if (line.dts.size()>i){
                text = text + to_string(line.dts[i])+",";
            }
            else{
                text = text + "nan,";
            }
        }
        text = text + "\n";
    }
    ofstream timesfile;
    timesfile.open(filename, ios_base::out);
    timesfile<<text;
    timesfile.close();


    
    return 0;
}