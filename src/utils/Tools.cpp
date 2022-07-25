#include "../../include/utils/Tools.h"
vector<string> Tools::split_simple(string& input, char delimiter) {
	vector<string> answer;
	stringstream ss(input);
    string temp;
    
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }

    return answer;
}