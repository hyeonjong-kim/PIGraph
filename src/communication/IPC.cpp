#include "IPC.h"

int IPC::createShm(key_t key){
    int shmId = shmget(key, 1024, 0666|IPC_CREAT);
    return shmId;
}

bool IPC::setData(int shmId, string data){
    char *shm = (char*) shmat(shmId,(void*)0,0);
    if(shm == (char*) -1){
        perror("shmat fild");
        return false;
    }

    sprintf(shm, "%s", (char*) data.c_str());
    this->detachShm(shm);
    return true;
}

string IPC::getData(int shmId){
    char *shm = (char*) shmat(shmId,(void*)0,0);
    if(shm == (char*) -1){
        perror("shmat fild");
    }

    string msg = string(shm);
    this->detachShm(shm);
    return msg;
}

bool IPC::deleteShm(int shmId){
    if(shmctl(shmId, IPC_RMID, NULL) == -1){
        return false;
    }
    
    return true;
}

bool IPC::detachShm(const void *shmaddr){
    if(shmdt(shmaddr) == -1){
        return false;
    }
    
    return true;
}

char* IPC::getShm(int shmId){
    return (char*) shmat(shmId,(void*)0,0);
}

int IPC::initSem(key_t semKey) {
    union semun semunarg;
    int status = 0, semid;

    semid = semget(semKey, 1, IPC_CREAT | IPC_EXCL | 0666);
    if(semid == -1) {
        if (errno == EEXIST)
            semid = semget(semKey, 1, 0);
    } else {
        semunarg.val = 1;
        status = semctl(semid, 0, SETVAL, semunarg);
    }

    if(semid == -1 || status == -1) {
        perror("initsem");
        return (-1);
    }

    return semid;
}

int IPC::semLock(int semId){
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = -1;
    buf.sem_flg = SEM_UNDO;

    if(semop(semId, &buf, 1) == -1) {
        perror("semlock failed");
        exit(1);
    }
    return 0;
}

int IPC::semUnlock(int semId) {
    struct sembuf buf;

    buf.sem_num = 0;
    buf.sem_op = 1;
    buf.sem_flg = SEM_UNDO;

    if(semop(semId, &buf, 1) == -1) {
        perror("semunlock failed");
        exit(1);
    }
    return 0;
}