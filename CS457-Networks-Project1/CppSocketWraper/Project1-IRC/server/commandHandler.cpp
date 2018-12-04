#include "commandHandler.h"

commandHandler::commandHandler(tcpServerSocket s):serverSocket(s){};

void commandHandler::handleCommand(vector<string> command,shared_ptr<client> usr){
    if(usr->getFirst()){
        if(commandMap.find(command[0]) == commandMap.end()){
            string s = "[Server]: NOPE: you have to log-in/register to chat. type /PASS, /NICK.";
            thread thread(&tcpUserSocket::sendString,usr->getSock(),s,true);
            thread.join();
        }
        else{
            switch(commandMap.find(command[0])->second){
                
                case NICK:nickCommand(usr,command);usr->setNickSet(true);break;
                case PASS:passCommand(usr,command);usr->setPassSet(true);break;
                case QUIT:quitCommand(usr,command);break;
                default:string s = "[Server]: NOPE: you have to log-in/register to chat. type /PASS, /NICK.";
                        thread thread(&tcpUserSocket::sendString,usr->getSock(),s,true);
                        thread.join();
            }
        }

        if(usr->getNickSet() && usr->getPassSet()){

            vector<string> clientInfo = checkUser(usr);
            if(clientInfo.size() != 0){
                if(clientInfo.at(0) == ""){

                }
                else{
                    usr->setFirst(false);
                    cout << "found user" << endl;
                    getClient(usr->getNick())->setSock(usr->getSock());
                    
                    thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Welcome back " + usr->getNick() +", enjoy your stay! ",true);
                    t1.join();
                }
            }
            else{
                usr->setFirst(false);
                usr->setLevel("user");
                string line = usr->getNick() + " " + usr->getPass() + " " + usr->getLevel();
                if(usr->getBanned()){
                    line += " true"; 
                }
                else{
                    line += " false";
                }
                addClient(usr);
                writeToFile("users.txt","",line);

                thread t2(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Welcome " + usr->getNick() +", you have registered a new account!",true);
                t2.join();
            }
        }

    }

    else if(commandMap.find(command[0]) == commandMap.end()){
        if(command[0][0] == '#' || command[0][0] == '&'){
            if(command.size() > 1){
                channel channel = *getChannel(command[0]);
                command.erase(command.begin());
                string message;
                for (auto const& s : command) { message += s + " "; };
                vector<shared_ptr<client>> clients = channel.getClients();
                vector<shared_ptr<client>>::iterator clIter;
                for(clIter = clients.begin();clIter != clients.end();advance(clIter,1)){
                    string s = "[" + channel.getChannelName() + "] " + usr->getNick() + ": " + message;
                    thread t1(&tcpUserSocket::sendString,clIter->get()->getSock(),s,true);
                    t1.join();
                }
            }
        }
        else{
            thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid command: If you wish to talk in a channel, preface the sentance with the channel name starting with # or &",true);
            t1.join();
        }
    }
    else{
        //lock this
        switch(commandMap.find(command[0])->second){
            case LIST:listCommand(usr,command);break;
            case AWAY:awayCommand(usr, command);break;
            case CONNECT:;
            case DIE:;
            case HELP:helpCommand(usr, command);break;
            case INFO:infoCommand(usr,command);break;
            case INVITE:inviteCommand(usr, command);break;
            case ISON:isonCommand(usr, command);break;
            case JOIN:joinCommand(usr, command);break;
            case KICK:;
            case MODE:modeCommand(usr, command);break;
            case NICK:nickCommand(usr,command);break;
            case NOTICE:noticeCommand(usr, command);break;
            case PART:partCommand(usr,command);break;
            case OPER:;
            case PASS:passCommand(usr,command);break;
            case PING:pingCommand(usr, command);break;
            case PONG:pongCommand(usr, command);break;
            case PRIVMSG:privmsgCommand(usr,command);break;
            case QUIT:quitCommand(usr,command);break;
            case RESTART:;
            case RULES:rulesCommand(usr, command);break;
            case SETNAME:setnameCommand(usr, command);break;
            case SILENCE:;
            case TIME:timeCommand(usr, command);break;
            case TOPIC:topicCommand(usr, command);break;
            case USER:;
            case USERHOST:;
            case USERIP:userIpCommand(usr, command);break;
            case USERS:;
            case VERSION:versionCommand(usr, command);break;
            case WALLOPS:;
            case WHO:whoCommand(usr, command);break;
            case WHOIS:;
            default:;
        }
    }
}

vector<string> commandHandler::splitMsg(string msg){
    vector<string> ret;
    stringstream stream(msg);
    string word;

    while(getline(stream,word,' ')){
        ret.push_back(word);
    }

    return ret;
}

string commandHandler::convertMsgtoString(vector<string> msg){
    string message = "";
    for(int i = 0;i < msg.size();i++){
            if(i == msg.size()-1){
                 message += msg[i];
            }
            else{
                message += msg[i] + " ";
            }
        }   
    return message;
}

shared_ptr<client> commandHandler::getClient(string name){
    vector<shared_ptr<client>>::iterator clIter;
    for(clIter = clients.begin();clIter != clients.end();advance(clIter,1)){
        if(clIter->get()->getNick() == name){
            return *clIter;
        }
    }
    return NULL;
}

vector<shared_ptr<client>> commandHandler::getAllClients(){
    return clients;
}

void commandHandler::removeChannel(string channelName){
    
    channelList.erase(getChannel(channelName));
    writeToFile("channels.txt",channelName,"");
        
}

bool commandHandler::checkChannel(string channelName){
    if(channelList.size() == 0){
        return false;
    }
    //Check if the name provided is an existing channel
    vector<channel>::iterator chIter;
    
    for(chIter = channelList.begin();chIter != channelList.end();advance(chIter,1)){
        if(chIter->getChannelName() == channelName){
            return true;
        }
    }
    return false;
}

vector<channel>::iterator commandHandler::getChannel(string channelName){
        vector<channel>::iterator chIter;
        for(chIter = channelList.begin();chIter != channelList.end();advance(chIter,1)){
            if(chIter->getChannelName() == channelName){
                return chIter;
            }
        }
    return chIter;
}

bool commandHandler::channelHasClient(string channelName,shared_ptr<client> cl){
    channel ch = *getChannel(channelName);
    vector<shared_ptr<client>> clients = ch.getClients();
    vector<shared_ptr<client>>::iterator clIter;

    for(clIter = clients.begin();clIter != clients.end();advance(clIter,1)){
        if(clIter->get()->getNick() == cl->getNick()){
            return true;
        }
    }
    return false;
}

vector<string> commandHandler::checkUser(shared_ptr<client> usr){
    vector<vector<string>>::iterator usersIter;
    for(usersIter = clientList.begin();usersIter != clientList.end();advance(usersIter,1)){
        if(usersIter->at(0) == usr->getNick()){
            if(usersIter->at(1) == usr->getPass()){
                return *usersIter;
            }
            else{
                string response = "[Server]: User nickname taken! Please try another";
                thread t1(&tcpUserSocket::sendString,usr->getSock(),response,true);
                t1.join();
                vector<string> ret = vector<string>();
                ret.push_back("");
                return ret;   
            }
        }
    }
    return vector<string>();
}

bool commandHandler::writeToFile(string file, string check, string newline){

        string name = dbPath + file + ".new";
        string infile = dbPath + file;
        ifstream openFile(infile);
        ofstream newFile(name);
        string temp;
        getline(openFile,temp);
            while (!openFile.eof()) {
                istringstream buf(temp);
                istream_iterator<string> beg(buf), end;
                vector<string> line(beg,end);
                if(line[0] == check && check != ""){
                    newFile << newline << endl;
                }
                else{
                    newFile << temp << endl;
                }
                getline(openFile,temp);
            }

        if(check == ""){
            newFile << newline << endl;
        }
        openFile.close();
        newFile.close();
        rename(name.c_str(),infile.c_str());

    return true;

}

bool commandHandler::joinCommand(shared_ptr<client> usr, vector<string> msg){

    if(msg.size() >= 2){
        string channelName = msg[1];
        if(channelName[0] == '#' || channelName[0] == '&'){

            if(!checkChannel(channelName)){
                channel ch = channel(channelName);
                channelList.push_back(ch);
                writeToFile("channels.txt","",channelName + " null, @");
            }
            if(!channelHasClient(channelName,usr)){
                (*getChannel(channelName)).addClient(usr);
                cout << "User " << usr->getNick() << " added to " << channelName << endl;
                thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: You have been added to channel: " + channelName,true);
                t1.join();
                return true;
            }
            else{
                thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: You are already in channel: " + channelName,true);
                t1.join();
                return false;
            }
        }
        else{
            thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Could not find/create channel, channel names begin with # or &",true);
            t1.join();
            return false;
        }
    }
    thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /JOIN",true);
    t1.join();
    return false;
}


bool commandHandler::partCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() >= 2){
        string channelName = msg[1];
        if(checkChannel(channelName) && channelHasClient(channelName,usr)){
            (*getChannel(channelName)).removeClient(usr);
            cout << "user " << usr->getNick() << " removed from " << channelName << endl;
            thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: You have been removed from channel: " + channelName,true);
            t1.join();
        }
        if((*getChannel(channelName)).getClients().size() == 0){
            removeChannel(channelName);
            cout << "channel " + channelName + " has been deleted" <<endl;
        }
        return true;
    }
    return false;
}

bool commandHandler::infoCommand(shared_ptr<client> usr, vector<string> msg){
    thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Server Info: IP= " + serverSocket.getIP() + " Port= " + to_string(serverSocket.getPort()),true);
    t1.join();
    return true;
}

bool commandHandler::listCommand(shared_ptr<client> usr, vector<string> msg){
    string channels = "";
    vector<channel>::iterator chIter;
    for(chIter = channelList.begin();chIter != channelList.end();advance(chIter,1)){
        if(chIter == channelList.begin()){
            channels += chIter->getChannelName();
        }
        else{
            channels += ", " + chIter->getChannelName();
        }
    }
    thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: List of all channels: " + channels,true);
    t1.join();
    return true;
}

bool commandHandler::nickCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() >= 2){
        string oldName = usr->getNick();
        msg.erase(msg.begin());
        usr->setNick(convertMsgtoString(msg));
        
        string response = "[Server]: User nickname changed from: " + oldName + " to: " + usr->getNick();
        thread t1(&tcpUserSocket::sendString,usr->getSock(),response,true);
        t1.join();
        return true;
    }
    else{
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Nickname not changed: possible invalid arguments to /NICK",true);
        t1.join();
        return false;
    }

}

bool commandHandler::topicCommand(shared_ptr<client> usr, vector<string> msg){
    string channelName = msg[1];
    //List the topic of the channel
    if(msg.size() == 2){
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Channel " + channelName + " topic is: " + (*getChannel(channelName)).getTopic(),true);
        t1.join();
        return true;
    }
    //Change the topic of the channel
    else if(msg.size() >= 3){
        msg.erase(msg.begin());
        msg.erase(msg.begin());
        string newTopic = convertMsgtoString(msg);
        
        (*getChannel(channelName)).setTopic(newTopic);
        cout << "channel: " + msg[1] + " topic has been changed to: " + newTopic << endl;
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Channel: " + channelName + " topic has been changed to: " + newTopic,true);
        t1.join();
        return true;

    }
    //Do nothing
    else{
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /TOPIC",true);
        t1.join();
        return false;
    }
}

bool commandHandler::whoCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() < 2){
        vector<shared_ptr<client>> allClients = getAllClients();
        vector<shared_ptr<client>>::iterator clIter;
        for(clIter = allClients.begin();clIter != allClients.end();advance(clIter,1)){
            thread t1(&tcpUserSocket::sendString,usr->getSock(),clIter->get()->getNick(),true);
            t1.join();
        }
        return true;
    }
    else{
        vector<shared_ptr<client>> allClients = getAllClients();
        vector<shared_ptr<client>>::iterator clIter;
        for(clIter = allClients.begin();clIter != allClients.end();advance(clIter,1)){
            if(clIter->get()->getNick().find(msg[1]) != string::npos){
                thread t1(&tcpUserSocket::sendString,usr->getSock(),clIter->get()->getNick(),true);
                t1.join();
            }
        }
        return true;
    }
    return false;
}

bool commandHandler::privmsgCommand(shared_ptr<client> usr, vector<string> msg){
    
    string toClient = msg[1];
    msg.erase(msg.begin());
    msg.erase(msg.begin());
    string message = convertMsgtoString(msg);
    thread t1(&tcpUserSocket::sendString,getClient(toClient)->getSock(),"{" + usr->getNick() + "}: " + message,true);
    t1.join();
    thread t2(&tcpUserSocket::sendString,usr->getSock(),"{" + usr->getNick() + "}: " + message,true);
    t2.join();
    return true;
    
}

bool commandHandler::passCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() > 1){
        if(msg[1].find_first_of('@') != string::npos){
            thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: password not set, no '@' symbols allowed!",true);
            t.join();
            return false;
        }
        usr->setPass(msg[1]);
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: password set",true);
        t1.join();
        return true;
    }
    else{
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid parameters: /PASS takes a no-space password",true);
        t1.join();
        return false;
    }
}

bool commandHandler::quitCommand(shared_ptr<client> usr, vector<string> msg){
    vector<channel>::iterator chIter = channelList.begin();
    for(;chIter != channelList.end();advance(chIter,1)){
        if(checkChannel(usr->getNick())){
            chIter->removeClient(usr);
        }
    }
    
    thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: You have left JPIRC, See you soon!",true);
    t.join();
    usr->getSock()->closeSocket();
    return true;
}

bool commandHandler::timeCommand(shared_ptr<client> usr, vector<string> msg){
    time_t tim = time(0);
    thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server Time]: " + string(ctime(&tim)),true);
    t.join();
    return true;
}

bool commandHandler::awayCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() > 1){
        msg.erase(msg.begin());
        string message = convertMsgtoString(msg);
        usr->setAwayReply(message);
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: you have set your away message to: " + message,true);
        t.join();
        return true;
    }
    else{
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: your away message is: " + usr->getAwayReply(),true);
        t.join();
        return false;
    }
}

bool commandHandler::pingCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() == 2){
        if(msg[1] == serverSocket.getIP()){
            thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: PONG!",true);
            t1.join();
            return true;
        }
        else{
            string toClient = msg[1];
            thread t2(&tcpUserSocket::sendString,getClient(toClient)->getSock(),"[PING] from: " + usr->getNick(),true);
            t2.join();
            return true;
        }
    }
    else{
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /PING. follow with the serve or a user.",true);
        t.join();
        return false;
    }
}


bool commandHandler::pongCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() == 2){
        string toClient = msg[1];
        thread t(&tcpUserSocket::sendString,getClient(toClient)->getSock(),"[PONG] from: " + usr->getNick(),true);
        t.join();
        return true;
    }
    return false;
}

bool commandHandler::isonCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() >= 2){
        msg.erase(msg.begin());
        vector<string> names;
        for(string c: msg){
            if(getClient(c) != NULL){
                names.push_back(c);
            }
        }
        thread t1(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Those online = " + convertMsgtoString(names),true);
        t1.join();
        return true;
    }
    else{
        thread t3(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /ISON, provide at least one nickname.",true);
        t3.join();
        return false;
    }
}


bool commandHandler::modeCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() == 3){
        if(getChannel(msg[1]) != channelList.end()){
            string channelName = msg[1];
            string perms = msg[2];
            if(perms[0] == '+'){
                getChannel(channelName)->addPerms(perms);
                thread t2(&tcpUserSocket::sendString,usr->getSock(),"[Server]: added " + perms + " modes to channel " + channelName,true);
                t2.join();
                return true;
            }
            else if(perms[0] == '-'){
                getChannel(channelName)->removePerms(perms);
                thread t2(&tcpUserSocket::sendString,usr->getSock(),"[Server]: removed " + perms + " modes from channel " + channelName,true);
                t2.join();
                return true;
            }
            else{
                thread t4(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid argument for /MODE, + or - to add or remove modes preceeded by channel.",true);
                t4.join();
                return false;
            }
        }
        else if(getClient(msg[1]) != NULL){
            string clientName = msg[1];
            string perms = msg[2];
            if(perms[0] == '+'){
                getClient(clientName)->addPerms(perms);
                thread t2(&tcpUserSocket::sendString,usr->getSock(),"[Server]: added " + perms + " modes to " + clientName,true);
                t2.join();
                return true;
            }
            else if(perms[0] == '-'){
                getChannel(clientName)->removePerms(perms);
                thread t2(&tcpUserSocket::sendString,usr->getSock(),"[Server]: removed " + perms + " modes from " + clientName,true);
                t2.join();
                return true;
            }
            else{
                thread t4(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid argument for /MODE, + or - to add or remove modes preceeded by channel or nickname.",true);
                t4.join();
                return false;
            }
        
        }
        return false;

    }
    else{
            thread t3(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid argument for /MODE, + or - to add or remove modes preceeded by channel.",true);
            t3.join();
            return false;
        }
}

bool commandHandler::inviteCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() >2){
        string channelName = msg[1];
        vector<string>::iterator clIter = msg.begin();
        advance(clIter,1);
        for(;clIter != msg.end();advance(clIter,1)){
            if(getClient(*clIter) != NULL){
                thread t(&tcpUserSocket::sendString,getClient(*clIter)->getSock(),"{" + usr->getNick() + "}: Hey! you should join channel: " + channelName,true);
                t.join();
            }
        }
        return true;
    }
    thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /INVITE, state channel and 1 or more nicknames.",true);
    t.join();
    return false;
}

bool commandHandler::noticeCommand(shared_ptr<client> usr, vector<string> msg){
    string toClient = msg[1];
    string message = convertMsgtoString(msg);
    thread t1(&tcpUserSocket::sendString,usr->getSock(),"{" + usr->getNick() + "}" + message,true);
    t1.join();
    return true;
}

bool commandHandler::versionCommand(shared_ptr<client> usr, vector<string> msg){
    thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Server version is: 1.0",true);
    t.join();
    return true;
}

bool commandHandler::setnameCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() > 1){
        string s;
        msg.erase(msg.begin());
        s = convertMsgtoString(msg);
        usr->setFullName(s);
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: full name has been set to: " + s,true);
        t.join();
        return true;
    }
    else{
            thread t3(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid argument for /SETNAME",true);
            t3.join();
            return false;
        }   
}

bool commandHandler::userIpCommand(shared_ptr<client> usr, vector<string> msg){
    if(msg.size() == 2){
        string ip = getClient(msg[1])->getSock()->getIP();
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: user: " + msg[1] + " has ip address: " + ip,true);
        t.join();
        return true;
    }
    else{
        thread t(&tcpUserSocket::sendString,usr->getSock(),"[Server]: Invalid arguments for /USERIP. provide 1 nickname",true);
        t.join();
        return false;
    }
}


bool commandHandler::rulesCommand(shared_ptr<client> usr, vector<string> msg){
    string rules = "These are the rules for JP-IRC:\n 1. No Advertisements allowed\n 2. Be on your best behavior\n 3. The admins are always watching\n 4. Have fun!"; 
    thread t(&tcpUserSocket::sendString, usr->getSock(),rules,true);
    t.join();
    return true;
}

bool commandHandler::helpCommand(shared_ptr<client> usr, vector<string> msg){
    string helpMessage;
    ifstream helpIn("README");
        string helpTemp;
        getline(helpIn,helpTemp);
        while (!helpIn.eof()) {
            helpMessage += helpTemp + "\n";
            getline(helpIn,helpTemp);
        }

    thread t(&tcpUserSocket::sendString,usr->getSock(),helpMessage,true);
    t.join();
    return true;
}