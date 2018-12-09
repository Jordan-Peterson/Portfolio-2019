
#include "client.h"

client::client(shared_ptr<tcpUserSocket> sock):clientSocket(sock),nickname("Anonymous"),password("@"),level("user"),banned(false){};


void client::addPerms(string s){
    string::iterator sIter = s.begin();
    for(;sIter != s.end();advance(sIter,1)){
        if(mode.find_first_of(*sIter) == string::npos){
            mode += *sIter;
        }
    }

}

void client::removePerms(string s){
    string::iterator sIter = s.begin();
    for(;sIter != s.end();advance(sIter,1)){
        if(mode.find_first_of(*sIter) != string::npos){
            mode.erase(sIter);
        }
    }
}

bool client::checkMode(char m){
    for(int k = 0; k < mode.length();k++){
        if(m == mode[k]){
            return true;
        }
    }
    return false;
}

void client::removeSilenced(string name){
    vector<string>::iterator it = silenceList.begin();
    for(;it != silenceList.end();it++){
        if(*it == name){
            silenceList.erase(it);
        }
    }
}

bool client::checkSilenceList(string name){
    vector<string>::iterator it = silenceList.begin();
    for(; it != silenceList.end();it++){
        if(*it == name){
            return true;
        }
    }
    return false;
}