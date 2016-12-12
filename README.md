# GeoReplication
CS523 - Project


There are 8 individual processes to be spawned on VMs
Consult network topology and update the IPs/Port Numbers as per the network set-up


Since all the processes are coded in C++
Use following command to compile and run:


g++ -std=c++11 process_name.cpp -o outputfile -pthread
./output


Before running above two commands ,remove all log files from previous run using 
rm log*


Processes to be deployed are:
############################
Client Side


WorkLoadGen.cpp
Layer1.cpp


############################
/* Within a DC, there are leader node and two replica nodes*/
DataCenter 1


locked_Lead.cpp
node1.cpp
node2.cpp


############################
/* Within a DC, there are leader node and two replica nodes*/
DataCenter 2


locked_Lead.cpp
node1.cpp
node2.cpp


============================
Internal architecture of different data centers is absolutely same.
The configurations which are essentially, IPs and Port Numbers which changes
on different DCs and nodes.


============================
To check for number of false negatives in the run
compile and run TestScript.cpp file inside TestCases/ directory


Please note: This process should only be invoked when workload generator has finished
its run.
============================


Order of Deployment


DC(i) processes > Layer1 > WorkLoad Generator
Unless preceding processes are fully up and running, do not spawn further processes 
in the order. It may not give correct results depending on the system delay to invoke processes maintaning dependencies.


Within DC, order can be any among Leader and the two replica nodes.Only when both Data
Centers are totally up, invoke Layer1(or ClientFacing App) and Workloadgen.
