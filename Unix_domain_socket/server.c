#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "sll/sll1.h"
#include "De_serialization/sentinel.h"
#include "De_serialization/serialize.h"
#include <memory.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 128

#define MAX_CLIENT_SUPPORTED    32

/*application specific structure*/
typedef struct person_{

    char name[32];
    int age;
    int weight;
} person_t;


void serialize_struct_Node(struct Node*obj, ser_buff_t *b);
void serialize_dll_t(dll_t * obj, ser_buff_t *b);

struct Node* de_serialize_struct_Node(ser_buff_t *b);
dll_t* de_serialize_dll_t(ser_buff_t *b);

void serialize_void(void* obj, ser_buff_t *b);
person_t* de_serialize_void(ser_buff_t *b);


static void
print_person_details(person_t *person){

    printf("Name = %s\t",   person->name);
    printf("Age = %d\t",    person->age);
    printf("weight = %d\t", person->weight);
    printf("\n");
}

static void print_person_db(dll_t* person_db) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        node = node->next; 
    } 
}


struct Node* find_person_db(dll_t* person_db, char*name) 
{
    if(!person_db || !person_db->head) return;
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
   
     while (node != NULL) { 
        data = node->data;
        if(!strncmp(data->name, name,sizeof(name)))
            return node;
        node = node->next; 
    } 
    return NULL;
}




void serialize_dll_t(dll_t * obj, ser_buff_t *b)
{
  SENTINEL_INSERTION_CODE(obj,b);
  serialize_struct_Node(obj->head, b);
}

void serialize_struct_Node(struct Node*obj, ser_buff_t *b)
{
   SENTINEL_INSERTION_CODE(obj,b);
   serialize_void(obj->data,b);
   serialize_struct_Node(obj->next,b);
}


dll_t* de_serialize_dll_t(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  dll_t* obj = calloc(1, sizeof(dll_t));
  obj->head = de_serialize_struct_Node(b);
  return obj;
}


struct Node* de_serialize_struct_Node(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  struct Node*obj = calloc(1,sizeof(struct Node));
  obj->data = de_serialize_void(b);
  obj->next = de_serialize_struct_Node(b);
  return obj;
}


void serialize_void(void* obj1, ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    person_t *obj = (person_t*)obj1;
    
    SENTINEL_INSERTION_CODE(obj,b);
    serialize_data(b, (char *)obj->name, 32);
    serialize_data(b, (char *)&obj->age, sizeof(int));
    serialize_data(b, (char *)&obj->weight, sizeof(int));
}


person_t* de_serialize_void(ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    SENTINEL_DETECTION_CODE(b);

    person_t *obj =  calloc(1, sizeof(person_t)); 
    de_serialize_data((char *)obj->name, b, 32);
    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&obj->weight, b, sizeof(int));
    return obj;
}


int monitored_fd_set[MAX_CLIENT_SUPPORTED];

int client_result[MAX_CLIENT_SUPPORTED] = {0};

static void intitiaze_monitor_fd_set(){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++)
        monitored_fd_set[i] = -1;
}


static void add_to_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != -1)
            continue;
        monitored_fd_set[i] = skt_fd;
        break;
    }
}

/*Remove the FD from monitored_fd_set array*/
static void
remove_from_monitored_fd_set(int skt_fd){

    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){

        if(monitored_fd_set[i] != skt_fd)
            continue;

        monitored_fd_set[i] = -1;
        break;
    }
}

/* Clone all the FDs in monitored_fd_set array into fd_set Data structure*/
static void
refresh_fd_set(fd_set *fd_set_ptr){

    FD_ZERO(fd_set_ptr);
    int i = 0;
    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] != -1){
            FD_SET(monitored_fd_set[i], fd_set_ptr);
        }
    }
}



static int
get_max_fd(){

    int i = 0;
    int max = -1;

    for(; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitored_fd_set[i] > max)
            max = monitored_fd_set[i];
    }

    return max;
}



int
main(int argc, char *argv[])
{
    struct sockaddr_un name;
    int number= 0;
    char name1[32] = "\0";
    int age = 0;
    int weight =0;

    person_t *person1 = calloc(1, sizeof(person_t));
    strncpy(person1->name, "Abhishek", strlen("Abhishek"));
    person1->age = 31;
    person1->weight = 75;

    person_t *person2 = calloc(1, sizeof(person_t));
    strncpy(person2->name, "Joseph", strlen("Joseph"));
    person2->age = 41;
    person2->weight = 70;

    
    dll_t *person_db = get_new_dll();
    At_front(&person_db->head, person1);
    At_front(&person_db->head, person2);
   

    int ret;
    int connection_socket;
    int data_socket;
    int result;
    int data;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int comm_socket_fd, i;
    intitiaze_monitor_fd_set();
    add_to_monitored_fd_set(0);

    
    unlink(SOCKET_NAME);

    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (connection_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    printf("Master socket created\n");

    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    strncpy(name.sun_path, SOCKET_NAME, sizeof(name.sun_path) - 1);

    
    ret = bind(connection_socket, (const struct sockaddr *) &name,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    printf("bind() call succeed\n");
    

    ret = listen(connection_socket, 20);
    if (ret == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    add_to_monitored_fd_set(connection_socket);
    
    ser_buff_t *Doubly_buffer =NULL;
    init_serialized_buffer(&Doubly_buffer);
    serialize_dll_t(person_db, Doubly_buffer);

    for (;;) {

        refresh_fd_set(&readfds); 
        
        printf("Waiting on select() sys call\n");
        printf("printing the object to be serialized on sending machine before serialiation\n\n");
        print_person_db(person_db);

        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(connection_socket, &readfds)){

            /*Data arrives on Master socket only when new client connects with the server (that is, 'connect' call is invoked on client side)*/
            printf("New connection recieved recvd, accept the connection\n");

            data_socket = accept(connection_socket, NULL, NULL);

            if (data_socket == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            printf("Connection accepted from client\n");
            ret = send(data_socket, Doubly_buffer->b, get_serialize_buffer_data_size(Doubly_buffer),0);
            printf("%s() : %d bytes sent\n", __FUNCTION__, ret);

            add_to_monitored_fd_set(data_socket);
        }
         else if(FD_ISSET(0, &readfds)){
            ser_buff_t *Console_buffer =NULL;
            init_serialized_buffer(&Console_buffer);
          
            memset(buffer, 0, 200);
            ret = read(0, buffer, 200);
          
            printf("Enter your choice\n");
            scanf("%d", &number);
            printf("%d\n", number);

            if(number ==1)
            {
            	printf("Enter your name\n");
            	scanf("%s", name1);
            	printf("%s\n", name1);
   
            	printf("Enter your age\n");
            	scanf("%d", &age);
            	printf("%d\n", age);

            	printf("Enter your weight\n");
            	scanf("%d", &weight);
            	printf("%d\n", weight);

               person_t *person3 = calloc(1, sizeof(person_t));
               //person3->name = name1;
               strcpy(person3->name, name1);
               person3->age = age;
               person3->weight = weight;
               At_front(&person_db->head, person3);
               print_person_db(person_db);
            }
            else if(number ==2)
		{
                   printf("Enter your name\n");
            	   scanf("%s", name1);
            	   printf("%s\n", name1);
                   struct Node* new_node=NULL;
                   new_node= find_person_db(person_db, name1); 
                   deleteNode(&person_db->head, new_node);
                   print_person_db(person_db);
		}
   
            

            init_serialized_buffer(&Doubly_buffer);
            serialize_dll_t(person_db, Doubly_buffer);

            i = 0;
            for(; i < MAX_CLIENT_SUPPORTED; i++){
                if(monitored_fd_set[i] != -1){
                   data_socket = monitored_fd_set[i];
                   ret = send(data_socket, Doubly_buffer->b, get_serialize_buffer_data_size(Doubly_buffer),0);
               }
            }
            
            
            // printf("Input read from console : %s\n", (char*)Console_buffer->b);
        }
        else /* Data srrives on some other client FD*/
        {
            /*Find the client which has send us the data request*/
            i = 0, comm_socket_fd = -1;
            for(; i < MAX_CLIENT_SUPPORTED; i++){

                if(FD_ISSET(monitored_fd_set[i], &readfds)){
                    comm_socket_fd = monitored_fd_set[i];

                    /*Prepare the buffer to recv the data*/
                    memset(buffer, 0, BUFFER_SIZE);

                   /* Wait for next data packet. */
                   /*Server is blocked here. Waiting for the data to arrive from client'read' is a blocking system call*/
                    printf("Waiting for data from the client\n");
                    ret = read(comm_socket_fd, buffer, BUFFER_SIZE);

                    if (ret == -1) {
                        perror("read");
                        exit(EXIT_FAILURE);
                    }

                    /* Add received summand. */
                    memcpy(&data, buffer, sizeof(int));
                    if(data == 0) {
                        /* Send result. */
                        memset(buffer, 0, BUFFER_SIZE);
                        sprintf(buffer, "Result = %d", client_result[i]);

                        printf("sending final result back to client\n");
                        ret = write(comm_socket_fd, buffer, BUFFER_SIZE);
                        if (ret == -1) {
                            perror("write");
                            exit(EXIT_FAILURE);
                        }

                        /* Close socket. */
                        close(comm_socket_fd);
                        client_result[i] = 0; 
                        remove_from_monitored_fd_set(comm_socket_fd);
                        continue; /*go to select() and block*/
                    }
                    client_result[i] += data;
                }
            }
        }
    } /*go to select() and block*/

    
    close(connection_socket);
    remove_from_monitored_fd_set(connection_socket);
    printf("connection closed..\n");

    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
