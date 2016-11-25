#include "../headers.h"

mutex mtx; 
unordered_map<int,int> store;

queue<pair<int,int>> rep_1;
queue<pair<int,int>> rep_2;

string readQueue(queue<pair<int,int>> &q){
	string result="";
	/* Lock Queue Here */
	while(!q.empty()){
		pair<int,int> p=q.front();
		result = result + to_string(p.first) + "#" + to_string(p.second) + "$";
		q.pop();
	}
	/* Unlock Queue Here */
	return result;
}
void update_store(string result){

		/* Read Changes from Leader */
		if(result == "" || result[0]=='#')
			return;

		mtx.lock();
			std::vector<string> pair = splitString(result,"#");
			store[stoi(pair[0])] = stoi(pair[1]);
			cout<<"\nLeader node Received Changes"<<store[stoi(pair[0])];
		mtx.unlock();
		/* Unlock Store */
}
string Leader_read(string key,string in);
string Leader_write(string key,string value,string in);



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
			cout<<"Server-bind() is OK.."<<endl;
		}
		addr_len = sizeof(struct sockaddr);
	}
	void periodic_check(){
	    cout<<"\nLeader Node: Periodic Check's reply is active...";
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
							cout<<"\nLeader: Node :1..entertained";
							
							response = readQueue(rep_1);
							response = response + to_string(store[stoi(v[1])]);			
							
					} else if(v[0]=="2"){
							cout<<"\nLeader: Node :1..entertained";
							
							response = readQueue(rep_2);
							response = response + to_string(store[stoi(v[1])]);			
							
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
	    cout<<"\nLeader Node: order check 2";
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

string Leader_read(string key,string in){	
	string result="";
	int port = (rand()%2==0)?5001:5002;
	ClientSocket c(port,"localhost");
	result = c.send_request(in);	

	/* Lock store 	*/
	mtx.lock();
	cout<<"\nLeader: Local The Read value is:"<<store[stoi(key)];
	mtx.unlock();
	/* Unlock store */

	return result;
}

string Leader_write(string key,string value,string in)
{
	string result="";
	int port = (rand()%2==0)?5001:5002;
	ClientSocket c(port,"localhost");
	result = c.send_request(in);

	/* Lock store 	*/
	mtx.lock();
	store[stoi(key)] = stoi(value);
	cout<<"\nLeader: Local The Write value is:"<<store[stoi(key)];
	mtx.unlock();
	/* Unlock store */

	return result;

}

void read_write_cmds_to_node(int port){
	ServerSocket_RDWR s(port);
	s.L1_to_replicas();
}

void periodic_check_from_node(int port){

	ServerSocket_PC s(port);
	s.periodic_check();
	
}

int main(){
	cout<<"\n Leader Node : 1 started ..";
	
	string str_port_repl = config_read("DC1_PORT_LEAD_REPL");
	string str_port_L1	 = config_read("DC1_PORT_LEAD_L1");
  
    if(str_port_repl.empty()|| str_port_L1.empty()) {
    	cout<<"Config Read Failed";
    }

    int _port_repl = stoi(str_port_repl);    
	int _port_L1   = stoi(str_port_L1);

	thread th_1 (periodic_check_from_node, _port_repl);
	thread th_2 (read_write_cmds_to_node,  _port_L1);
	
	th_1.join();
	th_2.join();

	return 0;
}
