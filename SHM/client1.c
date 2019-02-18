#include <errno.h>
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
#include <signal.h>

#define SOCKET_NAME "/tmp/DemoSocket"
#define BUFFER_SIZE 12

char read_buffer[128];
dll_t *person_db1 = NULL;
dll_t *Mac_db = NULL;
int data_socket;
char buffer[BUFFER_SIZE];


typedef struct person_{

    char name[32];
    int age;
    int weight;
} person_t;

typedef struct Mac_address{

   char address[60];
} M_addre;


void serialize_struct_Node(struct Node*obj, ser_buff_t *b);
void serialize_dll_t(dll_t * obj, ser_buff_t *b);

struct Node* de_serialize_struct_Node(ser_buff_t *b);
dll_t* de_serialize_dll_t(ser_buff_t *b);

void serialize_void(void* obj, ser_buff_t *b);
person_t* de_serialize_void(ser_buff_t *b);

void serialize_dll_t_mac(dll_t * obj, ser_buff_t *b);
void serialize_struct_Node_mac(struct Node*obj, ser_buff_t *b);

dll_t* de_serialize_dll_t_mac(ser_buff_t *b);
struct Node* de_serialize_struct_Node_mac(ser_buff_t *b);

void serialize_void_mac(void* obj1, ser_buff_t *b);
M_addre* de_serialize_void_mac(ser_buff_t *b);


static void
print_person_details(person_t *person){

    printf("Name = %s\t",   person->name);
    printf("Age = %d\t",    person->age);
    printf("weight = %d\t", person->weight);
    printf("\n");
}

static void print_person_db(dll_t* person_db) 
{
    if(!person_db || !person_db->head) 
     {
       printf("Entire structure is deleted\n");
       printf("*********************************************************\n");
       return;
     }
    
    struct Node *node = person_db->head;
    person_t *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        print_person_details(data);
        node = node->next; 
    } 
}

static void print_Mac_address(dll_t* Mac) 
{
    if(!Mac || !Mac->head) 
     {
       printf("Entire structure is deleted\n");
       //printf("*********************************************************\n");
       return;
     }
    
    struct Node *node = Mac->head;
    M_addre *data = NULL;
  
     while (node != NULL) { 
        data = node->data;
        printf("%s\t",data->address);
        memset(read_buffer, 0, 128);
        read_from_shared_memory(data->address, read_buffer, 128, 128);
        printf("%s\n", read_buffer);
        node = node->next; 
    } 
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
    printf("%s", obj->name);
    de_serialize_data((char *)&obj->age, b, sizeof(int));
    de_serialize_data((char *)&obj->weight, b, sizeof(int));
    return obj;
}




void serialize_dll_t_mac(dll_t * obj, ser_buff_t *b)
{
  SENTINEL_INSERTION_CODE(obj,b);
  serialize_struct_Node_mac(obj->head, b);
}

void serialize_struct_Node_mac(struct Node*obj, ser_buff_t *b)
{
   SENTINEL_INSERTION_CODE(obj,b);
   serialize_void_mac(obj->data,b);
   serialize_struct_Node_mac(obj->next,b);
}


dll_t* de_serialize_dll_t_mac(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  dll_t* obj = calloc(1, sizeof(dll_t));
  obj->head = de_serialize_struct_Node_mac(b);
  return obj;
}

struct Node* de_serialize_struct_Node_mac(ser_buff_t *b)
{
  SENTINEL_DETECTION_CODE(b);
  struct Node*obj = calloc(1,sizeof(struct Node));
  obj->data = de_serialize_void_mac(b);
  obj->next = de_serialize_struct_Node_mac(b);
  return obj;
}


void serialize_void_mac(void* obj1, ser_buff_t *b)
{
    int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    M_addre*obj = (M_addre*)obj1;
    
    SENTINEL_INSERTION_CODE(obj,b);
    serialize_data(b, (char *)obj->address, 60);
}


M_addre* de_serialize_void_mac(ser_buff_t *b)
{
   int loop_var = 0;
    unsigned int sentinel = 0xFFFFFFFF;
    SENTINEL_DETECTION_CODE(b);

    M_addre *obj =  calloc(1, sizeof(M_addre)); 
    de_serialize_data((char *)obj->address, b, 60);
    return obj;
}

static void
signal_handler(int sig){

    printf("Signal %d recieved from the server\n", sig);
    printf("Now both the internal data structures will get deleted\n");
    deleteList(&Mac_db->head);
    deleteList(&person_db1->head);
    printf("\n\nNew Routing table recieved from the server\n");
    print_person_db(person_db1);
    printf("\nARP table data\n"); 
    print_Mac_address(Mac_db);
} 

static void
ctrlC_signal_handler(int sig)
{
    printf("\nBye Bye");
    
    int pid =  getpid();
    memset(buffer, 0, 200);
    sprintf(buffer, "%d", pid);
    int ret = write(data_socket, buffer, 50);

    close(data_socket);
    exit(EXIT_SUCCESS);
}

int
main(int argc, char *argv[])
{
    struct sockaddr_un addr;
    int i;
    int ret;
    //int data_socket;
    //char buffer[BUFFER_SIZE];

    signal(SIGUSR1, signal_handler);
    signal(SIGINT, ctrlC_signal_handler);
    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (data_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_NAME, sizeof(addr.sun_path) - 1);

    ret = connect (data_socket, (const struct sockaddr *) &addr,
            sizeof(struct sockaddr_un));

    if (ret == -1) {
        fprintf(stderr, "The server is down.\n");
        exit(EXIT_FAILURE);
    }
    
    ser_buff_t *Doubly_coming_buffer= NULL;
    init_serialized_buffer(&Doubly_coming_buffer);
    reset_serialize_buffer(Doubly_coming_buffer);
    ret = recv(data_socket, Doubly_coming_buffer->b, get_serialize_buffer_length(Doubly_coming_buffer),0);

    
    /* Recieving machine has recieved the serialized buffer, Lets derialize it and reconstruct the object from it*/
        
     person_db1 = de_serialize_dll_t(Doubly_coming_buffer);
     printf("\nRouting table data recieved from server\n");
     print_person_db(person_db1);
     printf("\n");

     int pid =  getpid();
     memset(buffer, 0, 200);
     sprintf(buffer, "%d", pid);
     ret = write(data_socket, buffer, 50);

     ser_buff_t *Mac_coming_buffer= NULL;
     init_serialized_buffer(&Mac_coming_buffer);
     reset_serialize_buffer(Mac_coming_buffer);
     ret = read(data_socket, Mac_coming_buffer->b, get_serialize_buffer_length(Mac_coming_buffer));

     Mac_db = de_serialize_dll_t_mac(Mac_coming_buffer);
     printf("ARP table data recieved from server\n");
     print_Mac_address(Mac_db);
     printf("\n");
    
     printf("The process id is %d\n", pid);
     
     //int pid =  getpid();
     //printf("The process id is %d\n", pid);
     //memset(buffer, 0, 200);
     //sprintf(buffer, "%d", pid);
     //ret = write(data_socket, buffer, 50);
    
    do{
        ret = read(data_socket, buffer, 200);
        int n = atoi(buffer);
        if(n==12)
        {
        
       	  init_serialized_buffer(&Doubly_coming_buffer);
          reset_serialize_buffer(Doubly_coming_buffer);
          ret = recv(data_socket, Doubly_coming_buffer->b, get_serialize_buffer_length(Doubly_coming_buffer),0);

          person_db1 = de_serialize_dll_t(Doubly_coming_buffer);
          printf("\n\nNew Routing table recieved from the server\n");
          print_person_db(person_db1);
          printf("No of bytes sent = %d, data sent = %d\n", ret, i);
          printf("\nARP table data\n"); 
          print_Mac_address(Mac_db);
          printf("**********************************************\n");
       }

       if(n==34)
        {
           init_serialized_buffer(&Mac_coming_buffer);
           reset_serialize_buffer(Mac_coming_buffer);
           ret = read(data_socket, Mac_coming_buffer->b, get_serialize_buffer_length(Mac_coming_buffer));

           Mac_db = de_serialize_dll_t_mac(Mac_coming_buffer);
           printf("\n\nNew ARP table data recieved from server\n");
           print_Mac_address(Mac_db);
           printf("No of bytes sent = %d, data sent = %d\n", ret, i); 
           printf("\nRouting table data\n"); 
           print_person_db(person_db1);
           printf("**********************************************\n");
        }
    } while(1);

    /* Request result. */
    memset(buffer, 0, BUFFER_SIZE);
    strncpy (buffer, "RES", strlen("RES"));
    buffer[strlen(buffer)] = '\0';
    printf("buffer = %s\n", buffer);

    ret = write(data_socket, buffer, strlen(buffer));
    if (ret == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    
    /* Close socket. */
    close(data_socket);
    exit(EXIT_SUCCESS);
}
