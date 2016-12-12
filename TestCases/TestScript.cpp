#include "../Headers.h"
using namespace std;

int populateHash(unordered_map<int,int> &m,string filename){

    ifstream file(filename);
    string line;

    while (file && getline(file, line)){
        std::vector<string> v=splitString(line,"#");
        if(v.size()>0){
        	m[stoi(v[0])] = stoi(v[1]);
        }
    }
    return m.size();
}

int falseNegativesCount(unordered_map<int,int> &actual_data, unordered_map<int,int> &read_data){
	
	int falseNegatives = 0;
	for(auto it=read_data.begin(); it!=read_data.end(); it++){
		
		if(actual_data.find((*it).first)!=actual_data.end()){
			if(actual_data[(*it).first] == (*it).second){
				/* Correct Read */
			} else {
				falseNegatives++;
			}
		} else {
			falseNegatives++;
		}

	}
	return falseNegatives;
}
int main(){
	
	int number_of_requests = stoi(config_read("NumberOfRequests"));

	unordered_map<int,int> actual_data;
	unordered_map<int,int> read_data;

	if(!populateHash(actual_data,"actual_data.txt")){
		cout<<"\nNo Data Found";
		exit(0);
	}

	if(!populateHash(read_data,"read_data.txt")){
		cout<<"\nNo Data Read";
		exit(0);
	}

	int falseNegatives = falseNegativesCount(actual_data,read_data);
	cout<<"\nThe number of False Negatives per "<<number_of_requests<<" : "<<falseNegatives;

	return 0;
}
