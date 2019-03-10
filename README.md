# Data Synchronization Project using Inter_Process_Communication Techniques

1) Servere process is created which is called as Routing Table Manager process.This process is 
in charge of layer 3 Routing Table. And sends notifications to all the connected clients of any
change in the Routing Table.

2) Routing Table Manager process maintains Routing Table. So, admin is in charge of this table.
for this project linked list data structure is used to maintain the Routing Table.

3) Admin is provided with menu through which it can add, update, delete the record in Routing Table.
once it makes change in Table, entire snapshot of table is sent to all the connected clients which all
update their respective internal data structure once data is recieved.

4) All above steps Inter process communication tecnique of Unix Domain Socket is used.

5)  

Currently inter process communication is carried out using technique called unix domain socket.
As part of this project data is synchronized among multiple processes runing.

In this routing table manager process (RTM) is created. It is in charge of L3 routing table.

The aim here is that the state of routing table needs to be synchronized across all the connected clients at any point in time.
