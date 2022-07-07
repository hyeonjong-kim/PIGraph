#include "../../include/modules/Communication.h"

bool Communication::client(map<string, string>& config){
    int shmId = ipc.createShm((key_t)3141592);
    if (shmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return false;
	}
    
    string msg = "";

    map<string, string>::iterator iter;
    for(iter = config.begin(); iter != config.end(); iter++){
        msg += iter->first + " " + iter->second + "\n";
    }

    if(!ipc.setData(shmId, msg)){
        cerr << "[ERROR]FAIL TO SET SHM" << endl;
        return false;
    }

    return true;
}

bool Communication::master(){

}
