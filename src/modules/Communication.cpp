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

bool Communication::client(map<string, string> config){
    int argConfigShmId = this->ipc.createShm((key_t)3141592);
    if (argConfigShmId == -1){
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

    //Argument
    map<string, string>::iterator iter;
    for(iter = config.begin(); iter != config.end(); iter++){
        msg += iter->first + " " + iter->second + "\n";
    }
    
    if(!this->ipc.setData(argConfigShmId, msg)){
        cerr << "[ERROR]FAIL TO SET SHM" << endl;
        return false;
    }
    
    //Result
    msg = "";
    while(msg == ""){
        msg = this->ipc.getData(resultShmId);
        this_thread::sleep_for(chrono::milliseconds(50));
    }
    
    cerr << "[INFO]RESULT: " << msg << endl;

    return true;
}

map<string, string> Communication::master(){
    int argConfigShmId = this->ipc.createShm((key_t)3141592);
    if (argConfigShmId == -1){
		cerr << "[ERROR]FAIL TO CREATE OR GET SHM" << endl;
        return;
	}
    
    //wait
    this->ipc.setData(argConfigShmId, "");
    string msg = this->ipc.getData(argConfigShmId);
    
    while(msg == ""){
        msg = this->ipc.getData(argConfigShmId);
        this_thread::sleep_for(chrono::milliseconds(50));
    }

    //Argument
    map<string, string> argConfig;
    vector<string> splitMsg = split_simple(msg, '\n');
    for(int i = 0; i < splitMsg.size(); i++){
        vector<string>resultMsg = split_simple(splitMsg[i], ' ');
        argConfig.insert({resultMsg[0], resultMsg[1]});
    }   

    return argConfig;
}