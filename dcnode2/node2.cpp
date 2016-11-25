#include "../headers.h"
mutex mtx;
unordered_map<int,int> store;
class ClientSocket{
			public:
			int sockfd;
            /* connector’s address information */
			struct sockaddr_in their_addr;
			struct hostent *he;
			int numbytes,addr_len;
		    char buffer[10000];


		    ClientSocket(int Port,string IP){
		    	string ip = IP;
				if ((he = gethostbyname(ip.c_str())) == NULL) {
					cout<<"Client-gethostbyname() error"<<endl;
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
					cout<<"Client-sendto() error ! in Node at DC"<<endl;
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
class ServerSocket{
	public:
	int sockfd;
	struct sockaddr_in my_addr;
	/* connector’s address information */
	struct sockaddr_in their_addr;
	int addr_len, numbytes;
	char buf[10000];
	
	ServerSocket(int port_no){
		numbytes=0;
		my_addr.sin_family = AF_INET;			/* short, network byte order */
		my_addr.sin_port = htons(port_no);			/* automatically fill with my IP */
		my_addr.sin_addr.s_addr = INADDR_ANY;	/* zero the rest of the struct */
		memset(&(my_addr.sin_zero), '\0', 8);


		if((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
			cout<<"Server-socket() sockfd error lol!"<<endl;
			exit(1);
		} else {
			cout<<"Server-socket() sockfd is OK.."<<endl;
		
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
	void rec_request(){
	    cout<<"\nNode: order chcck 2";
		while(1){	

	    while((numbytes=recvfrom(sockfd, buf, MAXBUFLEN-1, 0, 
				(struct sockaddr *)&their_addr, (socklen_t*)&addr_len))!=-1) {
			    string result="";	    		
	    		buf[numbytes] = '\0';
				result=result+string(buf);
				
				vector<string> v=splitString(result,"#");
				cout<<"\n Receiving Request...";
				
				string response="";
				
				/* Lock Store */					
				mtx.lock();

				if(v[0]=="RD"){
			
					cout<<"\nNode :The Read value is:"<<store[stoi(v[1])];
					response = response + to_string(store[stoi(v[1])]);			

				} else if (v[0]=="WR"){

					store[stoi(v[1])] = stoi(v[2]);
					cout<<"\nL1:The Written value is:"<<store[stoi(v[1])];
					response = response + "Written" + v[1] + "successfully";
				} else {
					/* Drop the Request */
					cout<<"\n Invalid Request Popped up ! Alert !";
				}

				/* Unlock Store */
				mtx.unlock();

				strcpy(buf, response.c_str());
				numbytes = sendto(sockfd,buf, strlen(buf), 0,(struct sockaddr *)&their_addr,sizeof(struct sockaddr));
       				if (numbytes  < 0) cout<<"\nFailed to send";
			}
		}


	}
	~ServerSocket(){


		if(close(sockfd) != 0)
			cout<<"Server-sockfd closing failed!"<<endl;
		else
			cout<<"Server-sockfd successfully closed!"<<endl;
	}
	
};

void update_store(string result){


		if(result.empty() || result[0]=='$')
			return;
		/* Result will have one extra $ (dollar sign) at the end */
		vector<string> v=splitString(result,"$");


		/* Lock Store */
		mtx.lock();
		for(int i=0;i<v.size()-1;i++){
			std::vector<string> pair=splitString(v[i],"#");
			store[stoi(pair[0])] = stoi(pair[1]);
			cout<<"\nReplica node Received Changes"<<store[stoi(pair[0])];
		}
		mtx.unlock();
		/* Unlock Store */


		/* Read Changes from Leader */


}
void periodic_pull(int Lead_N1_Port,string Lead_IP){

	ClientSocket c(Lead_N1_Port,Lead_IP);
	while(1){
		string msg="";
		msg = msg + "2#CHG#"; 	/* 1 is to indicate Replica Node 1,so that leader read its Queue 1 */

		string response = "";
		response = c.send_request(msg);
		/* Read Changes on port */
	
		update_store(response);

		/* Sleep thread for two seconds ,periodic pull will happen again after 2 seconds*/	
		sleep(2);
	}
}
void leader_cmds(int Port){
	/* Either Read or Write request */ 
	ServerSocket s(Port);
	s.rec_request();
}

int main(){
	
	cout<<"\nDC2: Replica Node : 2 started ..";
	int Lead_Repl_Port = stoi(config_read("DC2_PORT_LEAD_REPL")); /* 6100 */
	string Lead_IP = config_read("DC2_LEAD_IP");

	int Repl_Port = stoi(config_read("DC2_PORT_REPL2_LEAD"));  /* 5001 */
	//int self_port = 5001; 
	
	thread th_1(periodic_pull,Lead_Repl_Port,Lead_IP);
	thread th_2(leader_cmds,Repl_Port);


	th_1.join();
    th_2.join();
	
	return 0;
}
