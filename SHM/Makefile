TARGET: server1 client1
serialize.o:De_serialization/serialize.c
	gcc  -c -I De_serialization De_serialization/serialize.c -o De_serialization/serialize.o
sll.o:sll/sll1.c
	gcc  -c -I sll sll/sll1.c -o sll/sll.o
shm_demo.o:shm_demo.c
	gcc -g -c shm_demo.c -o shm_demo.o
client1.o:client1.c
	gcc -c -g client1.c -o client1.o
server1.o:server1.c
	gcc -c -g server1.c -o server1.o 
client1:client1.o De_serialization/serialize.o sll/sll.o shm_demo.o
	gcc -g -I De_serialization -I sll  De_serialization/serialize.o sll/sll.o client1.o shm_demo.o -o client1 -lrt
server1:server1.o De_serialization/serialize.o sll/sll.o shm_demo.o
	gcc -g -I De_serialization -I sll De_serialization/serialize.o sll/sll.o server1.o shm_demo.o -o server1 -lrt
clean:
	rm client1
	rm server1
	rm *.o
	rm sll/*.o
	rm De_serialization/*.o
