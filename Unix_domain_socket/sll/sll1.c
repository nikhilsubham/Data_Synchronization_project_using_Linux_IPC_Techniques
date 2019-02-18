#include "sll1.h"
#include <memory.h>
#include <stdlib.h>
#include<stdio.h>

/* Public Function Implementation to create and return new empty singly linked list*/
dll_t * get_new_dll(){

    dll_t *dll = calloc(1, sizeof(dll_t));
    dll->head = NULL;
    return dll;
}


/* Given a reference (pointer to pointer) to the head of a list inserts a new node on the front of the list. */
void At_front(struct Node** head_ref, void*app_data) 
{ 
	
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
        new_node->data = app_data; 
        new_node->next = (*head_ref); 
        (*head_ref) = new_node; 
} 


/* Given a node prev_node, insert a new node after the given prev_node */
void insertAfter(struct Node* prev_node, void* new_data) 
{ 
	
	if (prev_node == NULL) 
	{ 
	  printf("the given previous node cannot be NULL");	 
	  return; 
	} 
		
	struct Node* new_node =(struct Node*) malloc(sizeof(struct Node)); 
        new_node->data = new_data; 
        new_node->next = prev_node->next; 
        prev_node->next = new_node; 
} 


/* Given a reference (pointer to pointer) to the head of a list and a data, appends a new node at the end */
void At_end(struct Node** head_ref, void* new_data) 
{
	
	struct Node* new_node = (struct Node*) malloc(sizeof(struct Node)); 
        struct Node *last = *head_ref; 

	new_node->data = new_data; 
        new_node->next = NULL; 

	if (*head_ref == NULL) 
	{ 
	   *head_ref = new_node; 
	   return; 
	} 
	
	while (last->next != NULL) 
		last = last->next; 

	last->next = new_node; 
	return;	 
}

void deleteNode(struct Node **head_ref, struct Node *del)
{ 
    struct Node* temp = (*head_ref); 
    struct Node* prev = NULL;

    if(*head_ref == NULL || del == NULL) 
        return; 

    if(temp == del) 
     {
        *head_ref = del->next;
        free(del); 
        return; 
     } 
    
    while(temp != del && temp != NULL)
    {
        prev = temp;
        temp= temp ->next;
    }

    if(temp == NULL)
       return;

    prev->next = temp ->next;
    free(temp); 
       
  } 

 






