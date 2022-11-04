#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream>
#include <map>
#include <queue>

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>



using namespace std;
typedef uint32_t vid_t;

int main(){
    string path = "/home/hjkim/data/Amazon0601.txt";
    string path2 = "/home/hjkim/data/Amazon0601.txt_gemini";
    FILE* inf = fopen(path.c_str(), "r");
    FILE* ouf = fopen(path2.c_str(), "wb");
    
    while (!feof(inf))
    {
        int from;
        int to;
        float w = 1;
        fscanf(inf, "%d/t%d", &from, &to);

        size_t res1 = fread(&from, sizeof(vid_t), 1, inf);
        size_t res2 = fread(&to, sizeof(vid_t), 1, inf);
        if(from != to){
            fwrite(&from, sizeof(vid_t),1,ouf);
            fwrite(&to, sizeof(vid_t),1,ouf);
            fwrite(&w, sizeof(vid_t),1,ouf);
        }
    }
}
