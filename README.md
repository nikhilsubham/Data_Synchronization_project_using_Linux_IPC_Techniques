# Data Synchronization Project using Inter_Process_Communication Techniques

As part of this project data is synchronized among multiple processes runing on the same machine.
This project uses Three Inter Process Communication Techniques
1) Unix Domain Socket
2) Shared Memory
3) signals

The details regarding the project is given below.

1) Servere process is created which is called as Routing Table Manager process.This process is 
in charge of layer 3 Routing Table and sends notifications to all the connected clients of any
change in the Routing Table.

2) Routing Table Manager process maintains Routing Table. So, admin is in charge of this table.
for this project linked list data structure is used to maintain the Routing Table.

3) Admin is provided with menu through which it can add, update, delete the record in Routing Table.
Once it makes changes in Table, entire snapshot of table is sent to all the connected clients using 
Unix Domain Socket which all update their respective internal data structure once data is recieved.

4) All above steps Inter process communication tecnique of Unix Domain Socket is used.

5) In 2nd phase of the project Routing Table Manager process also maintains ARP table and it synchronizes
it to all subscribed(connected) clients using shared memory as an IPC technique.

6) Admin maintains Table which contains Mac Addresses.The process creates a shared memory region
where this Mac table entries are used as a key in shared memory where IP addresses are stored.

7) when Admin make changes to the Mac adddress table the correspoinding IP addresess in shared memory
is also updated. This Mac address table is synchronzed to all the connected clients So, all can acess
the shared memory region where IP address are availalable.

8) At any point in time if any new client request comes it is given the full snapshot of the currnt data 
stored in both the tables.

9) In 3rd phase of project signals are used as means of IPC. Whenever new client connects to the server
process it provides the process ID of its own to the server which maintains it.

10) One more option of flushing both the table is provided to the Admin in server process. When admin 
flush this both table signals are provided to all connected clients So, all connected clients will flush
their internal data structure. 





