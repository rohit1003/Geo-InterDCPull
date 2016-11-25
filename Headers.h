#include <arpa/inet.h>
#include <algorithm>
#include <climits>
#include <errno.h>
#include <fstream>
#include <iostream>

#include <list>
#include <math.h>
#include <mutex>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <string>
#include <set>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <sys/time.h>
#include <thread>
#include <time.h> 

#include <queue>
#include <unistd.h>
#include <unordered_map>
#include <utility>   /* pair */
#include <vector>

#define MAXBUFLEN 10000
using namespace std;
vector<string> splitString(string input, string delimiter)
  {
     vector<string> output;
     char *pch;
  
     char *str = strdup(input.c_str());
  
     pch = strtok (str, delimiter.c_str());
  
     while (pch != NULL)
     {
        output.push_back(pch);
  
        pch = strtok (NULL,  delimiter.c_str());
     }
  
     free(str);
  
     return output;
 }
string config_read(string config_var){

    string result="";
    ifstream file("../config.txt");
    string line;

    while (file && getline(file, line)){
        std::vector<string> v=splitString(line,"=");
        if(v.size()>0 && v[0]==config_var){
            return v[1];
        }
    }
    return result;
}
int valid_for_stoi(string str){
    for(int i=0;i<str.length();i++){
        if(str[i]-'0' >= 0 && str[i]-'0'<=9 )
                cout<<str[i]-'0';
        else
            return 0;
        }
    return 1;
}