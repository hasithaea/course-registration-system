#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SEATS 5

typedef struct node {
    int id;
    char name[50];
    struct node *next;
} node;

typedef struct {
    node *head;
    int count;
} StudentList;

typedef struct {
    node *front;
    node *rear;
    int count;
} WaitingQueue;

typedef struct {
    node *top;
} DropStack;




node *createNode(int id, char *name)
{
    node *nw = (node *)malloc(sizeof(node));
    if (nw == NULL) return NULL;
    nw->id = id;
    strcpy(nw->name, name);
    nw->next = NULL;
    return nw;
}

void initList(StudentList *list)
{
    list->head  = NULL;
    list->count = 0;
}

void initQueue(WaitingQueue *queue)
{
    queue->front = NULL;
    queue->rear  = NULL;
    queue->count = 0;
}

void initStack(DropStack *stack)
{
    stack->top = NULL;
}


// waiting queue

void enqueue(WaitingQueue *queue, int id, char *name)
{
    node *nw = createNode(id, name);
    if (nw == NULL) return;

    if (queue->rear == NULL)
    {
        queue->front = nw;
        queue->rear  = nw;
    }
    else
    {
        queue->rear->next = nw;
        queue->rear       = nw;
    }
    queue->count++;
    printf("Student %s added to waiting queue\n", name);
}

node *dequeue(WaitingQueue *queue)
{
    if (queue->front == NULL) return NULL;

    node *temp   = queue->front;
    queue->front = queue->front->next;

    if (queue->front == NULL)
        queue->rear = NULL;

    queue->count--;
    temp->next = NULL;
    return temp;
}

void displayQueue(WaitingQueue *queue)
{
    if (queue->front == NULL)
    {
        printf("Waiting queue is empty.\n");
        return;
    }

    printf("\n--- Waiting Queue ---\n");
    node *current = queue->front;
    int pos = 1;
    while (current != NULL)
    {
        printf("%d. [%d] %s\n", pos++, current->id, current->name);
        current = current->next;
    }
}


// drop stack

void push(DropStack *stack, int id, char *name)
{
    node *nw = createNode(id, name);
    if (nw == NULL) return;

    nw->next   = stack->top;
    stack->top = nw;
}

node *pop(DropStack *stack)
{
    if (stack->top == NULL) return NULL;

    node *temp = stack->top;
    stack->top = stack->top->next;
    temp->next = NULL;
    return temp;
}

void displayStack(DropStack *stack)
{
    if (stack->top == NULL)
    {
        printf("Drop history is empty\n");
        return;
    }

    printf("\n--- Drop History ---\n");
    node *current = stack->top;
    int pos = 1;
    while (current != NULL)
    {
        printf("%d. [%d] %s\n", pos++, current->id, current->name);
        current = current->next;
    }
}


// student list

void displayStudents(StudentList *list)
{
    if (list->head == NULL)
    {
        printf("No registered students.\n");
        return;
    }

    printf("\n--- Registered Students (%d/%d) ---\n", list->count, MAX_SEATS);
    node *current = list->head;
    int pos = 1;
    while (current != NULL)
    {
        printf("%d. [%d] %s\n", pos++, current->id, current->name);
        current = current->next;
    }
}

void searchStudent(StudentList *list, int id)
{
    node *current = list->head;
    while (current != NULL)
    {
        if (current->id == id)
        {
            printf("Found: [%d] %s\n", current->id, current->name);
            return;
        }
        current = current->next;
    }
    printf("Student ID %d not found in registered list.\n", id);
}

void registerStudent(StudentList *list, WaitingQueue *queue, int id, char *name)
{
    // duplicate check
    node *current = list->head;
    while (current != NULL)
    {
        if (current->id == id)
        {
            printf("Student ID %d is already registered.\n", id);
            return;
        }
        current = current->next;
    }

    if (list->count < MAX_SEATS)
    {
        node *nw = createNode(id, name);
        if (nw == NULL) return;

        if (list->head == NULL)
        {
            list->head = nw;
        }
        else
        {
            node *tail = list->head;
            while (tail->next != NULL)
                tail = tail->next;
            tail->next = nw;
        }
        list->count++;
        printf("Student %s registered successfully \n", name);
    }
    else
    {
        printf("Course is full. Adding %s to waiting queue \n", name);
        enqueue(queue, id, name);
    }
}

void dropStudent(StudentList *list, WaitingQueue *queue, DropStack *stack, int id)
{
    node *current  = list->head;
    node *previous = NULL;

    while (current != NULL)
    {
        if (current->id == id)
        {
            // remove from list
            if (previous == NULL)
                list->head = current->next;
            else
                previous->next = current->next;

            list->count--;
            current->next = NULL;

            printf("Student [%d] %s dropped \n", current->id, current->name);

            // push to drop
            push(stack, current->id, current->name);
            free(current);

            // admit next from queue
            node *next = dequeue(queue);
            if (next != NULL)
            {
                printf("Admitting %s from waiting queue \n", next->name);
                registerStudent(list, queue, next->id, next->name);
                free(next);
            }
            return;
        }
        previous = current;
        current  = current->next;
    }
    printf("Student ID %d not found in registered list \n", id);
}

void undoLastDrop(StudentList *list, WaitingQueue *queue, DropStack *stack)
{
    node *student = pop(stack);
    if (student == NULL)
    {
        printf("No drop to undo.\n");
        return;
    }

    if (list->count < MAX_SEATS)
    {
        printf("Undo: adding [%d] %s back to registered list.\n", student->id, student->name);
        registerStudent(list, queue, student->id, student->name);
    }
    else
    {
        printf("Cannot undo: registered list is full. Student [%d] %s not re added\n",
               student->id, student->name);
    }
    free(student);
}




int main()
{
    StudentList list;
    WaitingQueue queue;
    DropStack stack;

    initList(&list);
    initQueue(&queue);
    initStack(&stack);

    int choice, id;
    char name[50];

    while (1)
    {
        printf("\n===== Main Menu =====\n");
        printf("1. Register Student\n");
        printf("2. Drop Student\n");
        printf("3. Undo Last Drop\n");
        printf("4. Search Student\n");
        printf("5. Display Registered Students\n");
        printf("6. Display Waiting Queue\n");
        printf("7. Display Drop History\n");
        printf("8. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
            case 1:
                printf("Enter Student ID: ");
                scanf("%d", &id);
                printf("Enter Student Name: ");
                scanf(" %[^\n]", name);
                registerStudent(&list, &queue, id, name);
                break;

            case 2:
                printf("Enter Student ID to drop: ");
                scanf("%d", &id);
                dropStudent(&list, &queue, &stack, id);
                break;

            case 3:
                undoLastDrop(&list, &queue, &stack);
                break;

            case 4:
                printf("Enter Student ID to search: ");
                scanf("%d", &id);
                searchStudent(&list, id);
                break;

            case 5:
                displayStudents(&list);
                break;

            case 6:
                displayQueue(&queue);
                break;

            case 7:
                displayStack(&stack);
                break;

            case 8:
                printf("Exiting.\n");
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
}
