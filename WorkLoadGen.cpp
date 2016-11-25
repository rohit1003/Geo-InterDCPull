#include "Headers.h"
string _local_config_read(string config_var){
    
    string result="";
    ifstream file("config.txt");
    string line;

    while (file && getline(file, line)){
        std::vector<string> v=splitString(line,"=");
        if(v.size()>0 && v[0]==config_var){
            return v[1];
        }

    }
    return result;
}

class ClientSocket{
			public:
			int sockfd;
            /* connector’s address information */
			struct sockaddr_in their_addr;
			struct hostent *he;
			int numbytes,addr_len;
		    char buffer[10000];

		    ClientSocket(int port,string ip){
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
				cout<<"Using port: 4001"<<endl;
				their_addr.sin_port = htons(port);
				their_addr.sin_addr = *((struct in_addr *)he->h_addr);       /* zero the rest of the struct */
				memset(&(their_addr.sin_zero), '\0', 8);

			}
			void send_request(string str){

				strcpy(buffer, str.c_str());
				printf("\nsending --> %s",buffer);
				if((numbytes = sendto(sockfd, buffer, strlen(buffer), 0, 
				 	(struct sockaddr *)&their_addr, sizeof(struct sockaddr))) == -1) {
					cout<<"Client-sendto() error !"<<endl;
					exit(1);
				}
				else {
					numbytes=recvfrom(sockfd, buffer, strlen(buffer), 0, 
					(struct sockaddr *)&their_addr, (socklen_t*)&addr_len);

					string result="";	    		
		    		buffer[numbytes] = '\0';
					result=result+string(buffer);

					cout<<"Response is..."<<result<<endl;
				cout<<"sent "<<numbytes<<" bytes to "<<inet_ntoa(their_addr.sin_addr)<<endl;
				
				}


			}
			~ClientSocket(){
		
			/* Closing UDP socket */

				if (close(sockfd) != 0)
					cout<<"Client-sockfd closing is failed!"<<endl;
				else
					cout<<"Client-sockfd successfully closed!"<<endl;
			}


};

class workload{
	int blob_id_count;
	int number_of_requests ; 
	int readWriteRatio;
	unordered_map<int,int> data;
	public:
	workload(){
		blob_id_count =1 ;
		number_of_requests = stoi(_local_config_read("NumberOfRequests"));
		readWriteRatio = stoi(_local_config_read("ReadWriteRatio"));
	}
	void generation(ClientSocket &c);
	unsigned long long writeload(int _blob_id_count,int _value,ClientSocket &c);
	unsigned long long readload(int _blob_id_count,ClientSocket &c);
};
void workload::generation(ClientSocket &c){
 
 	int i=1,temp=0;

    int num_reads=0,num_writes=0;
    int max_read=0,max_write=0;
    unsigned long long read_sum=0,write_sum=0;
 	
 	writeload(blob_id_count++,rand()%200,c);
 	writeload(blob_id_count++,rand()%200,c);
 	while(i<number_of_requests){

 		if( i % readWriteRatio == 0){
 			temp = writeload(blob_id_count++,rand()%200,c);
 			max_write = max(max_write,temp);
 			write_sum += temp;
 			num_writes++;
 		} else {
 			
 			temp = readload( (rand()%(blob_id_count-1)) + 1,c);
 			max_read = max(max_read,temp);
 			read_sum  += temp;
 			num_reads++;
 		}
 		i++;
 	}
 	cout<<"\n The average read time for : " << num_reads << " request " << 
 	(float)read_sum/num_reads<<"..Max Read is"<<max_read;
 	cout<<"\n The average write time for : " << num_writes << " request " << 
 	(float)write_sum/num_writes<<"..Max Write is"<<max_write;
}
unsigned long long time_gap(struct timeval tv){

	unsigned long long millisecondsSinceEpoch =
    (unsigned long long)(tv.tv_sec) * 1000 +
    (unsigned long long)(tv.tv_usec) / 1000;

    struct timeval ct;
    gettimeofday(&ct, NULL);



    unsigned long long diff = (unsigned long long)(ct.tv_sec) * 1000 +
    (unsigned long long)(ct.tv_usec) / 1000   - millisecondsSinceEpoch;

    return diff;
}
unsigned long long workload::writeload(int _blob_id_count,int _value,ClientSocket &c){
    data[_blob_id_count]=_value;
	
	struct timeval ct;
    gettimeofday(&ct, NULL);
    //sleep(1);
    string str="";
    str = "WR#" + to_string(_blob_id_count) +"#"+ to_string(_value);
    
    c.send_request(str);
	unsigned long long gap = time_gap(ct);
    cout<<"\n The write value of blob: "<<_blob_id_count<<" is : "<<_value<<" in "<<gap<<" ms";
    return gap;
}

unsigned long long workload::readload(int _blob_id_count,ClientSocket &c){
	
	struct timeval ct;
    gettimeofday(&ct, NULL);

    string str="";
    str = "RD#" + to_string(_blob_id_count);
	c.send_request(str);
	unsigned long long gap = time_gap(ct);
	cout<<"\nThe read value of blob: "<<_blob_id_count<<" is : "<<data[_blob_id_count]<<" in "<<gap<<" ms";
	return gap;
}

int main(){
	sleep(1);
	cout<<"WorkLoadGen started \n";
	/* Starting Client Socket to send read/write Requests*/
	
	string _L1_port = _local_config_read("LAYER1_PORT");
	string L1_ip    = _local_config_read("LAYER1_IP");


	if(L1_ip.empty()|| _L1_port.empty()) {
    	cout<<"Workload Gen config read Failed";
    }

    int L1_port=stoi(_L1_port);
	ClientSocket c(L1_port, L1_ip);

	workload w;
	w.generation(c);

    return 0;
}
