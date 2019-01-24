# Inter_process_communication

Currently inter process communication is carried out using technique called unix domain socket.
As part of this project data is synchronized among multiple processes runing.

In this routing table manager process (RTM) is created. It is in charge of L3 routing table.

The aim here is that the state of routing table needs to be synchronized across all the connected clients at any point in time.
