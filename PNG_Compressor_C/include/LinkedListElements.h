#include <stddef.h> // required for NULL
#include <stdio.h>
#include <stdlib.h>
typedef struct list_node
{
   struct list_node *next; //8 byte because pointer
   
} list_node;


typedef struct string_item
{
    struct list_node node;  //8 byte
    const char *string;     //8 byte because pointer
}string_item;


//DOUBLE LINKED LIST ELEMENTS 
typedef struct list_double_node
{
    struct list_double_node *prev;
    struct list_double_node *next;
}list_double_node;

typedef struct string_item_double
{
    struct list_double_node node;  //8 byte
    const char *string;     //8 byte because pointer
}string_item_double;

void invert_LinkedList(list_node** head)
{
    if (!head)
        return;

    list_node* curr = *head;
    list_node* prev = NULL;
    list_node* next = NULL;

    
   
    while (curr)
    {
        
        //next node is current node's next 
        next = curr->next;

        // current node's next is now set to previous, first iteration becomes null
        curr->next = prev;

        //cycle forward: current becomes previous, next becomes current
        prev = curr;
        curr = next;
        

    }
    
    *head = prev;
    
    return;
}



void PrintList(string_item* my_linked_list)
{
    string_item* string = my_linked_list;
    while (string)
    {
        printf("%s\n", string->string);
        string = (struct string_item *)string->node.next;
    }
}