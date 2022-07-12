#include "../../include/modules/Communication.h"

vector<string> split_simple(string& input, char delimiter) {
	vector<string> answer;
	stringstream ss(input);
    string temp;

    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}

bool Communication::client(map<string, string>& config){
    int configShmId = this->ipc.createShm((key_t)3141592);
    if (configShmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return false;
	}
    
    int resultShmId = this->ipc.createShm((key_t)190507);
    if (resultShmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return false;
	}

    this->ipc.setData(resultShmId, "");

    string msg = "";

    map<string, string>::iterator iter;
    for(iter = config.begin(); iter != config.end(); iter++){
        msg += iter->first + " " + iter->second + "\n";
    }
    
    if(!this->ipc.setData(configShmId, msg)){
        cerr << "[ERROR]FAIL TO SET SHM" << endl;
        return false;
    }
    
    while(msg == ""){
        msg = this->ipc.getData(resultShmId);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    
    cerr << "[INFO]RESULT: " << msg << endl;

    return true;
}

map<string, string> Communication::master(){
    int shmId = this->ipc.createShm((key_t)3141592);
    this->ipc.setData(shmId, "");
    string msg = this->ipc.getData(shmId);
    
    while(msg == ""){
        msg = this->ipc.getData(shmId);
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    map<string, string> m;
    vector<string> splitMsg = split_simple(msg, '\n');
    for(int i = 0; i < splitMsg.size(); i++){
        vector<string>resultMsg = split_simple(splitMsg[i], ' ');
        m.insert({resultMsg[0], resultMsg[1]});
    }

    return m;
}