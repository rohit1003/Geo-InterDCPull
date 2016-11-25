#include "../headers.h"

mutex mtx; 
unordered_map<int,int> store;

class _queue{
	public:
	queue<pair<int,int>> q;
	mutex m;
	string readQueue();
	void writeQueue(string key,string value);
};

_queue repl_1;
_queue repl_2;
_queue cross_lead;

string _queue::readQueue(){

	string result="";

	/* Lock Queue Here */
	m.lock();
	while(!q.empty()){
		pair<int,int> p=q.front();
		result = result + to_string(p.first) + "#" + to_string(p.second) + "$";
		q.pop();
	}
	m.unlock();
	/* Unlock Queue Here */
	
	return result;
}
void _queue::writeQueue(string key,string value){
	m.lock();
	
		if(!key.empty() && !value.empty()){
			cout<<"\nInteger value is:"<<key<<".."<<value<<__LINE__<<"\n";
			if(valid_for_stoi(key) && valid_for_stoi(value)){
			
				int k=stoi(key);
				int v=stoi(value);
				q.push(make_pair(k,v));
			}
		}
	m.unlock();
}
void update_store(string result){

		/* Read Changes from Leader */
		if(result.empty() || result[0]=='#')
			return;

		mtx.lock();
			std::vector<string> pair = splitString(result,"#");
			
			if(pair.size()!=0 && !pair[0].empty() && !pair[1].empty()) {
				cout<<"\nInteger value is:"<<pair[0]<<".."<<pair[1]<<__LINE__<<"\n";
				if(valid_for_stoi(pair[0]) && valid_for_stoi(pair[1])){
					store[stoi(pair[0])] = stoi(pair[1]);
					cout<<"\nLeader node Received Changes"<<store[stoi(pair[0])];
				}
			}
		mtx.unlock();
		/* Unlock Store */
}
string Leader_read(string key,string in);
string Leader_write(string key,string value,string in);

void lead_to_lead_push_recv_update_store(vector<string> v){
					
						/* Result will have one extra $ (or not dollar sign) at the end ???*/

						for(int i=0;i<v.size();i++){

							std::vector<string> p=splitString(v[i],"#");
										
							/*cross_lead.writeQueue(p[0],p[1]);*/

							mtx.lock();
							
							if(p.size()!=0 && !p[0].empty() && !p[1].empty()){
								cout<<"\nInteger value is:"<<p[0]<<".."<<p[1]<<__LINE__<<"\n";
								if(valid_for_stoi(p[0]) && valid_for_stoi(p[1])){
									store[stoi(p[0])] = stoi(p[1]);
								}
							}
							mtx.unlock();

							repl_1.writeQueue(p[0],p[1]);
							repl_2.writeQueue(p[0],p[1]);	

							cout<<"\nLead:lead_to_lead_push_recv_update_store: Received Writes";
						}

						return;
						/* Read Writes from Different DC-Leader */
}


class ClientSocket{
			public:
			int sockfd;
            /* connector’s address information */
			struct sockaddr_in their_addr;
			struct hostent *he;
			int numbytes,addr_len;
		    char buffer[10000];

		    ClientSocket(int Port,string ip){
				if ((he = gethostbyname(ip.c_str())) == NULL) {
					cout<<"Client-gethostbyname() error"<<endl;;
					exit(1);
				}
				else {	
						cout<<"Client-gethostname() is OK"<<endl;
				}
				if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
					cout<<"Client-socket() error lol!"<<endl;
					exit(1);
				}
				else {
					cout<<"Client-socket() sockfd is OK..."<<endl;			/* host byte order */
				}

				their_addr.sin_family = AF_INET;							/* short, network byte order */
				addr_len = sizeof(struct sockaddr);
				cout<<"Using port: "<<Port<<"endl";
				their_addr.sin_port = htons(Port);
				their_addr.sin_addr = *((struct in_addr *)he->h_addr);       /* zero the rest of the struct */
				memset(&(their_addr.sin_zero), '\0', 8);

			}
			string send_request(string str){

				strcpy(buffer, str.c_str());
				printf("\nsending --> %s",buffer);
				if((numbytes = sendto(sockfd, buffer, strlen(buffer), 0, 
				 	(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
					cout<<"Client-sendto() error ! in Leader at DC"<<endl;
					exit(1);
				}
				else {
					numbytes=recvfrom(sockfd, buffer, strlen(buffer), 0, 
					(struct sockaddr *)&their_addr, (socklen_t*)&addr_len);

					string result="";	    		
		    		buffer[numbytes] = '\0';
					result=result+string(buffer);

					cout<<"\nResponse is..."<<result<<endl;
//				cout<<"sent "<<numbytes<<" bytes to "<<inet_ntoa(their_addr.sin_addr)<<endl;
					return result;				
				}

				return NULL;
			}
			~ClientSocket(){
		
			/* Closing UDP socket */

				if (close(sockfd) != 0)
					cout<<"Client-sockfd closing is failed!"<<endl;
				else
					cout<<"Client-sockfd successfully closed!"<<endl;
			}


};
class ServerSocket_PC{
	public:
	int sockfd;
	struct sockaddr_in my_addr;
	/* connector’s address information */
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[10000];
	
	ServerSocket_PC(int port_no){
		numbytes=0;
		my_addr.sin_family = AF_INET;			/* short, network byte order */
		my_addr.sin_port = htons(port_no);			/* automatically fill with my IP */
		my_addr.sin_addr.s_addr = INADDR_ANY;	/* zero the rest of the struct */
		memset(&(my_addr.sin_zero), '\0', 8);

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			cout<<"Server-socket() sockfd error lol!"<<endl;
			exit(1);
		} else {
			cout<<"Server-socket() in PC sockfd is OK.."<<endl;
		}
		
		if(::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) <0)
		{
			cout<<"Server-bind() error !"<<endl;
			exit(1);
		} else {
			cout<<"Server-bind() in PC is OK.."<<endl;
		}
		addr_len = sizeof(struct sockaddr);
	}
	void periodic_check(){
	    cout<<"\nLeader Node: Periodic Check's reply is active...\n";
	    while(1){

		while((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1, 0, 
				(struct sockaddr *)&their_addr, (socklen_t*)&addr_len))!=-1) {
			    string result="";	    		
	    		buf[numbytes] = '\0';
				result=result+string(buf);
				
				vector<string> v=splitString(result,"#");
				cout<<"\n Receiving Request...";
				
				string response="";
				if(v[1]=="CHG"){
								
					if(v[0]=="1"){		
							
							cout<<"\nLeader: Repl-1..entertained";
							response = repl_1.readQueue();	

					} else if(v[0]=="2"){
							
							cout<<"\nLeader: Repl-2..entertained";
							response = repl_2.readQueue();
					}
				} else {

					/* Drop the Request */
					cout<<"\n Invalid Request Popped up ! Alert !";
				}

				/* Unlock Store */
				strcpy(buf, response.c_str());
				numbytes = sendto(sockfd,buf, strlen(buf), 0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
       				if (numbytes  < 0) cout<<"\nFailed to send";
			} 
	}

	}
	~ServerSocket_PC(){

		if(close(sockfd) != 0)
			cout<<"Server-sockfd closing failed!"<<endl;
		else
			cout<<"Server-sockfd in PC successfully closed!"<<endl;
	}
	
};
class ServerSocket_RDWR{
	public:
	int sockfd;
	struct sockaddr_in my_addr;
	/* connector’s address information */
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[10000];
	
	ServerSocket_RDWR(int port_no){
		numbytes=0;
		my_addr.sin_family = AF_INET;			/* Short, network byte order 		*/
		my_addr.sin_port = htons(port_no);		/* Automatically fill with my IP 	*/
		my_addr.sin_addr.s_addr = INADDR_ANY;	/* Zero the rest of the struct 		*/
		memset(&(my_addr.sin_zero), '\0', 8);

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			cout<<"Server-socket() sockfd error lol!"<<endl;
			exit(1);
		} else {
			cout<<"Server-socket() in Read/Write sockfd is OK.."<<endl;
		
		}
		
		if(::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) <0)
		{
			cout<<"Server-bind() error !"<<endl;
			exit(1);
		} else {
			cout<<"Server-bind() is OK.."<<endl;
		}
		addr_len = sizeof(struct sockaddr);
	}
	void L1_to_replicas(){
	    cout<<"\nLeader  receiving from L1\n";
	    while(1){

		while((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1, 0, 
				(struct sockaddr *)&their_addr, (socklen_t*)&addr_len))!=-1) {
			    string result="";	    		
	    		buf[numbytes] = '\0';
				result=result+string(buf);
				
				vector<string> v=splitString(result,"#");
				cout<<"\n Receiving Request...";
				/* Read/Write from/to this node*/
                
				/* Send to one of the replica also to achieve Quorum */
				string response = "";
				/* Actually send the request to Repl Node also */
				if(v[0]=="RD"){

					response = Leader_read(v[1],result);		
				
				} else if (v[0]=="WR"){

					response = Leader_write(v[1],v[2],result);				
					repl_1.writeQueue(v[1],v[2]);
					repl_2.writeQueue(v[1],v[2]);

					/* For variation 7, Send to Leader of secondary DC */
					cross_lead.writeQueue(v[1],v[2]);

				} else {

					/* Drop the Request */
				}


				strcpy(buf, response.c_str());
				numbytes = sendto(sockfd,buf, strlen(buf), 0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
       				if (numbytes  < 0) cout<<"\nFailed to send";
			} 
	}

	}
	~ServerSocket_RDWR(){

		if(close(sockfd) != 0)
			cout<<"Server-sockfd closing failed!"<<endl;
		else
			cout<<"Server-sockfd in Read/Write successfully closed!"<<endl;
	}
	
};
class ServerSocket_LeadRecv{
	public:
	int sockfd;
	struct sockaddr_in my_addr;
	/* connector’s address information */
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[10000];
	
	ServerSocket_LeadRecv(int port_no){
		numbytes=0;
		my_addr.sin_family = AF_INET;			/* Short, network byte order 		*/
		my_addr.sin_port = htons(port_no);		/* Automatically fill with my IP 	*/
		my_addr.sin_addr.s_addr = INADDR_ANY;	/* Zero the rest of the struct 		*/
		memset(&(my_addr.sin_zero), '\0', 8);

		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			cout<<"Server-socket() sockfd error lol!"<<endl;
			exit(1);
		} else {
			cout<<"Server-socket() in Read/Write sockfd is OK.."<<endl;
		
		}
		
		if(::bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) <0)
		{
			cout<<"Server-bind() error !"<<endl;
			exit(1);
		} else {
			cout<<"Server-bind() is OK.."<<endl;
		}
		addr_len = sizeof(struct sockaddr);
	}
	void lead_to_lead_recv(){
	    cout<<"\nLeader  receiving from L1\n";
	    while(1){

		while((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1, 0, 
				(struct sockaddr *)&their_addr, (socklen_t*)&addr_len))!=-1) {

			    string result="";	    		
	    		buf[numbytes] = '\0';
				result=result+string(buf);
				
				vector<string> v=splitString(result,"$");
				cout<<"\n Receiving Request...";

				/* Read/Write from/to this node*/
				/* Send to one of the replica also to achieve Quorum */
				
				string response = "Written successfully";
				
				if(!result.empty() && result[0]!='$' && v.size() !=0){

					/* Actually send the request to Repl Node also */
					lead_to_lead_push_recv_update_store(v);

				} else {
					/* Drop the Request */
				}

				strcpy(buf, response.c_str());
				numbytes = sendto(sockfd,buf, strlen(buf), 0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
       				if (numbytes  < 0) cout<<"\nFailed to send";
			} 
	}

	}
	~ServerSocket_LeadRecv(){

		if(close(sockfd) != 0)
			cout<<"Server-sockfd closing failed!"<<endl;
		else
			cout<<"Server-sockfd in Read/Write successfully closed!"<<endl;
	}
	
};

string Leader_read(string key,string in){	
	string result="";
	int port = (rand()%2==0)?6001:6002;
	ClientSocket c(port,"localhost");
	result = c.send_request(in);	

	/* Lock store 	*/
	mtx.lock();
	if(!key.empty()){							
		if(valid_for_stoi(key)){
		cout<<"\nLeader: Local The Read value is:"<<store[stoi(key)];
		}
	}
	mtx.unlock();
	/* Unlock store */

	return result;
}

string Leader_write(string key,string value,string in)
{
	string result="";
	int port = (rand()%2==0)?6001:6002;
	ClientSocket c(port,"localhost");
	result = c.send_request(in);

	/* Lock store 	*/
	mtx.lock();
	if(!key.empty() && !value.empty()){
		if(valid_for_stoi(key) && valid_for_stoi(value)){
			store[stoi(key)] = stoi(value);
			cout<<"\nLeader: Local The Write value is:"<<store[stoi(key)];
		}
	}
	mtx.unlock();
	/* Unlock store */

	return result;

}

void read_write_cmds_to_node(int port){
	ServerSocket_RDWR s(port);
	s.L1_to_replicas();
}

void periodic_check_from_node(int port){
	sleep(1);
	ServerSocket_PC s(port);
	s.periodic_check();
	
}
void lead_to_lead_push(int port, string ip){
	
	string result="";
	while(1){

		/* This functon Sleep in microseconds */
		usleep(1000);
		ClientSocket c(port,ip);

		/* Read Local Queue */
		string str = cross_lead.readQueue();
		
		/*  Send Changes to Leader in Secondary DataCenter */
		result = c.send_request(str);
	}

}
void lead_to_lead_push_recv(int port){

	usleep(1000);
	ServerSocket_LeadRecv s(port);
	s.lead_to_lead_recv();

}

int main(){
	cout<<"\n Leader Node : 1 started ..";
	
	string str_port_repl = config_read("DC2_PORT_LEAD_REPL");
	string str_port_L1	 = config_read("DC2_PORT_LEAD_L1");

  	string str_IP_cross_lead	= config_read("DC1_LEAD_IP");
  	string str_port_cross_lead	= config_read("DC1_PORT_LEAD_LEAD");
  	string str_port_cross_lead_recv =config_read("DC2_PORT_LEAD_LEAD");

    if(str_port_repl.empty()|| str_port_L1.empty()) {
    	cout<<"Config Read Failed";
    }

    int _port_repl 		 = stoi(str_port_repl);    
	int _port_L1   		 = stoi(str_port_L1);
	int _port_cross_lead = stoi(str_port_cross_lead);
	int _port_cross_lead_recv = stoi(str_port_cross_lead_recv);

	thread th_2 (read_write_cmds_to_node,  _port_L1);
	thread th_1 (periodic_check_from_node, _port_repl);
	thread th_3 (lead_to_lead_push, _port_cross_lead, str_IP_cross_lead);
	thread th_4 (lead_to_lead_push_recv, _port_cross_lead_recv);
	
	th_1.join();
	th_2.join();
	th_3.join();
	th_4.join();

	return 0;
}
