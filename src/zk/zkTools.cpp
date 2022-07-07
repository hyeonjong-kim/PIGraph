#include "../../include/zk/zkTools.h"

void watcher(zhandle_t *, int event, int state, const char *path, void*v) {
	if (state == ZOO_EXPIRED_SESSION_STATE)
		cout << "state:" << "ZOO_EXPIRED_SESSION_STATE" << endl;
	else if (state == ZOO_AUTH_FAILED_STATE)
		cout << "state:" << "ZOO_AUTH_FAILED_STATE" << endl;
	else if (state == ZOO_CONNECTING_STATE)
		cout << "state:" << "ZOO_CONNECTING_STATE" << endl;
	else if (state == ZOO_ASSOCIATING_STATE)
		cout << "state:" << "ZOO_ASSOCIATING_STATE" << endl;
	else if (state == ZOO_CONNECTED_STATE)
		cout << "state:" << "ZOO_CONNECTED_STATE" << endl;
	else
		cout << "unknown state:" << state << endl;
	
	if (event == ZOO_CREATED_EVENT)
		cout << "event:" << "ZOO_CREATED_EVENT" << endl;
	else if (event == ZOO_DELETED_EVENT)
		cout << "event:" << "ZOO_DELETED_EVENT" << endl;
	else if (event == ZOO_CHANGED_EVENT)
		cout << "event:" << "ZOO_CHANGED_EVENT" << endl;
	else if (event == ZOO_CHILD_EVENT)	
		cout << "event:" << "ZOO_CHILD_EVENT" << endl;
	else if (event == ZOO_SESSION_EVENT)		
		cout << "event:" << "ZOO_SESSION_EVENT" << endl;
	else if (event == ZOO_NOTWATCHING_EVENT)		
		cout << "event:" << "ZOO_NOTWATCHING_EVENT" << endl;
	else
		cout << "unknown event:" << event << endl;
}

void wgetWatcher(zhandle_t *zh , int event, int state, const char *path, void* watcherCtx) {
	char* data = (char*) watcherCtx;
	struct Stat stat;
	int len = 512;
	if (event == ZOO_CHANGED_EVENT){
		zoo_wget(zh, path, wgetWatcher, data, data, &len, &stat);
	}
	else if(event == ZOO_DELETED_EVENT){
		return;
	}
}

zhandle_t* zkTools::zkInit(char* hostInfo){
    const int RECV_TIMEOUT = 10000;
    zhandle_t *zh = zookeeper_init(hostInfo, watcher, RECV_TIMEOUT, 0, 0, 0);
    return zh;
}

bool zkTools::zkCreatePersistent(zhandle_t *zh, char* path, char* data){
	struct Stat stat;
	char newPath[512];
	int rc = 0;
	
	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		rc = zoo_create(zh, path, data, sizeof(data), &ZOO_OPEN_ACL_UNSAFE, 0, newPath, sizeof(newPath)-1);
		cerr << "[INFO]SUCESS CREATE PERSISTENT ZNODE" << endl;
		return 	true;
	}
	else{
		cerr << "[ERROR]EXIST ZNODE" << endl;
		return 	false;
	}

	cerr << "[ERROR]FAIL CREATE PERSISTENT ZNODE" << endl;
	return false;
}

bool zkTools::zkExists(zhandle_t *zh,  char* path){
	int rc = 0;
	struct Stat stat;
	rc = zoo_exists(zh, path, 0, &stat);
}

bool zkTools::zkCreateEphemeral(zhandle_t *zh, char* path, char* data){
	struct Stat stat;
	char newPath[512];
	int rc = 0;
	
	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		rc = zoo_create(zh, path, data, sizeof(data), &ZOO_READ_ACL_UNSAFE, ZOO_EPHEMERAL, newPath, sizeof(newPath));
		cerr << "[INFO]SUCESS CREATE EHEMERAL ZNODE" << endl;
		return 	true;
	}
	else{
		cerr << "[ERROR]EXIST ZNODE" << endl;
		return 	false;
	}

	cerr << "[ERROR]FAIL CREATE EHEMERAL ZNODE" << endl;
	return false;
}

bool zkTools::zkDelete(zhandle_t *zh, char* path){
	int rc = 0;
	struct Stat stat;
	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		cerr << "[ERROR]DO NOT EXIST ZNODE" << endl;
		return 	false;
	}
	else{
		zoo_delete(zh, path, 0);
		cerr << "[INFO]SUCESS DELETE " << path << " ZNODE"<< endl;
		return 	true;
	}

 	cerr << "[ERROR]FAIL TO DELETE ZNODE" << endl;
	return false;
}

bool zkTools::zkClose(zhandle_t *zh){
	zookeeper_close(zh);
	cerr << "[INFO]SUCESS TO CLOSE ZK" << endl;
	return true;
}

bool zkTools::zkWget(zhandle_t *zh, char* path, char* buffer){
	struct Stat stat;
	int rc = 0;
	int len = 512;

	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		cerr << "[ERROR]DO NOT EXIST ZNODE" << endl;
		return false;
	}
	else{
		cerr << "[INFO]SUCESS TO GET DATA" << endl;
		zoo_wget(zh, path, wgetWatcher, buffer, buffer, &len, &stat);
		return true;
	}

	cerr << "[ERROR]FAIL TO GET DATA" << endl;
	return false;
}

bool zkTools::zkGet(zhandle_t *zh, char* path, char* buffer){
	struct Stat stat;
	int rc = 0;
	int len = 512;

	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		cerr << "[ERROR]DO NOT EXIST ZNODE" << endl;
		return false;
	}
	else{
		cerr << "[INFO]SUCESS TO GET DATA" << endl;
		zoo_get(zh, path, 0, buffer, &len, &stat);
		return true;
	}

	cerr << "[ERROR]FAIL TO GET DATA" << endl;
	return false;
}

bool zkTools::zkSet(zhandle_t *zh, char* path, char* data){
	struct Stat stat;
	int rc = 0;
	int len = 512;
	
	rc = zoo_exists(zh, path, 0, &stat);
	if(rc == ZNONODE){
		cerr << "[ERROR]DO NOT EXIST ZNODE" << endl;
		return false;
	}
	else{
		cerr << "[INFO]SUCESS TO SET DATA" << endl;
		zoo_set(zh, path, data, len, 0);
		return true;
	}

	cerr << "[ERROR]FAIL TO SET DATA" << endl;
	return false;
}