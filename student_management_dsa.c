#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_UNDO  50
#define MAX_QUEUE 5

/* Student structure */
struct Student
{
    int rollNo;
    char name[50];
    float marks;
};

/* Binary Search Tree Node (main record storage) */
struct TreeNode
{
    struct Student data;
    struct TreeNode *left;
    struct TreeNode *right;
};

/* Root of the BST */
struct TreeNode *root = NULL;

int studentCount = 0;

/* Stack (array based) for undoing deletions */
struct Student undoStack[MAX_UNDO];
int top = -1;

/* Circular Queue (array based) for admission waiting list */
struct Student admissionQueue[MAX_QUEUE];
int front = 0, rear = -1, queueCount = 0;


/* -------------------------------------------------
   HELPER FUNCTIONS
   ------------------------------------------------- */

/* Read student details from the user (roll number is passed in) */
void readStudentDetails(struct Student *s)
{
    printf("Enter Name: ");
    scanf(" %49[^\n]", s->name);

    printf("Enter Marks: ");
    scanf("%f", &s->marks);
}


/* -------------------------------------------------
   BST FUNCTIONS
   ------------------------------------------------- */

/* Create a new tree node */
struct TreeNode *createNode(struct Student s)
{
    struct TreeNode *newNode;

    newNode = (struct TreeNode *)malloc(sizeof(struct TreeNode));

    if (newNode == NULL)
    {
        printf("\nMemory allocation failed!\n");
        return NULL;
    }

    newNode->data = s;
    newNode->left = NULL;
    newNode->right = NULL;

    return newNode;
}


/* Search student by roll number (BST search) */
struct TreeNode *searchBST(struct TreeNode *node, int roll)
{
    if (node == NULL || node->data.rollNo == roll)
    {
        return node;
    }

    if (roll < node->data.rollNo)
    {
        return searchBST(node->left, roll);
    }

    return searchBST(node->right, roll);
}


/* Insert student into BST. Returns 1 on success, 0 on failure */
int insertBST(struct Student s)
{
    struct TreeNode *newNode, *current, *parent = NULL;

    if (searchBST(root, s.rollNo) != NULL)
    {
        printf("\nRoll number %d already exists!\n", s.rollNo);
        return 0;
    }

    newNode = createNode(s);

    if (newNode == NULL)
    {
        return 0;
    }

    if (root == NULL)
    {
        root = newNode;
        studentCount++;
        return 1;
    }

    current = root;

    while (current != NULL)
    {
        parent = current;

        if (s.rollNo < current->data.rollNo)
        {
            current = current->left;
        }
        else
        {
            current = current->right;
        }
    }

    if (s.rollNo < parent->data.rollNo)
    {
        parent->left = newNode;
    }
    else
    {
        parent->right = newNode;
    }

    studentCount++;
    return 1;
}


/* Find the node with the smallest roll number in a subtree */
struct TreeNode *findMin(struct TreeNode *node)
{
    while (node->left != NULL)
    {
        node = node->left;
    }

    return node;
}


/* Delete student from BST. Copies the deleted record into *deleted */
struct TreeNode *deleteBST(struct TreeNode *node, int roll, struct Student *deleted)
{
    struct TreeNode *temp;

    if (node == NULL)
    {
        return NULL;
    }

    if (roll < node->data.rollNo)
    {
        node->left = deleteBST(node->left, roll, deleted);
    }
    else if (roll > node->data.rollNo)
    {
        node->right = deleteBST(node->right, roll, deleted);
    }
    else
    {
        *deleted = node->data;

        /* Case 1 & 2: zero or one child */
        if (node->left == NULL)
        {
            temp = node->right;
            free(node);
            return temp;
        }
        else if (node->right == NULL)
        {
            temp = node->left;
            free(node);
            return temp;
        }

        /* Case 3: two children - replace with inorder successor */
        temp = findMin(node->right);
        node->data = temp->data;

        {
            struct Student dummy;
            node->right = deleteBST(node->right, temp->data.rollNo, &dummy);
        }
    }

    return node;
}


/* Inorder traversal (prints students sorted by roll number) */
void inorderDisplay(struct TreeNode *node)
{
    if (node == NULL)
    {
        return;
    }

    inorderDisplay(node->left);

    printf("%d\t\t%s\t\t%.2f\n",
           node->data.rollNo,
           node->data.name,
           node->data.marks);

    inorderDisplay(node->right);
}


/* Copy BST records into an array using inorder traversal */
void fillArray(struct TreeNode *node, struct Student arr[], int *index)
{
    if (node == NULL)
    {
        return;
    }

    fillArray(node->left, arr, index);

    arr[*index] = node->data;
    (*index)++;

    fillArray(node->right, arr, index);
}


/* Free the entire tree */
void freeTree(struct TreeNode *node)
{
    if (node == NULL)
    {
        return;
    }

    freeTree(node->left);
    freeTree(node->right);
    free(node);
}


/* -------------------------------------------------
   STUDENT OPERATIONS (USING BST)
   ------------------------------------------------- */

/* Add student */
void addStudent()
{
    struct Student s;

    printf("\nEnter Roll Number: ");
    scanf("%d", &s.rollNo);

    if (searchBST(root, s.rollNo) != NULL)
    {
        printf("\nRoll number %d already exists!\n", s.rollNo);
        return;
    }

    readStudentDetails(&s);

    if (insertBST(s))
    {
        printf("\nStudent added successfully!\n");
    }
}


/* Display students sorted by roll number */
void displayStudents()
{
    if (root == NULL)
    {
        printf("\nNo students available!\n");
        return;
    }

    printf("\n---------------------------------------------\n");
    printf("Roll No\t\tName\t\tMarks\n");
    printf("---------------------------------------------\n");

    inorderDisplay(root);

    printf("\nTotal students: %d\n", studentCount);
}


/* Search student using BST search */
void searchStudent()
{
    int roll;
    struct TreeNode *result;

    printf("\nEnter Roll Number to search: ");
    scanf("%d", &roll);

    result = searchBST(root, roll);

    if (result != NULL)
    {
        printf("\nStudent Found!\n");

        printf("Roll Number: %d\n", result->data.rollNo);
        printf("Name: %s\n", result->data.name);
        printf("Marks: %.2f\n", result->data.marks);
    }
    else
    {
        printf("\nStudent not found!\n");
    }
}


/* Update marks of a student */
void updateMarks()
{
    int roll;
    struct TreeNode *result;

    printf("\nEnter Roll Number to update: ");
    scanf("%d", &roll);

    result = searchBST(root, roll);

    if (result == NULL)
    {
        printf("\nStudent not found!\n");
        return;
    }

    printf("Current Marks: %.2f\n", result->data.marks);
    printf("Enter New Marks: ");
    scanf("%f", &result->data.marks);

    printf("\nMarks updated successfully!\n");
}


/* -------------------------------------------------
   STACK FUNCTIONS (UNDO DELETE)
   ------------------------------------------------- */

/* Push deleted student onto stack */
void push(struct Student s)
{
    if (top == MAX_UNDO - 1)
    {
        printf("\nUndo stack overflow! Oldest undo history is full.\n");
        return;
    }

    undoStack[++top] = s;
}


/* Delete student and save the record for undo */
void deleteStudent()
{
    int roll;
    struct Student deleted;

    printf("\nEnter Roll Number to delete: ");
    scanf("%d", &roll);

    if (searchBST(root, roll) == NULL)
    {
        printf("\nStudent not found!\n");
        return;
    }

    root = deleteBST(root, roll, &deleted);
    studentCount--;

    push(deleted);

    printf("\nStudent deleted successfully! (Use Undo to restore)\n");
}


/* Pop last deleted student and restore into BST */
void undoDelete()
{
    struct Student s;

    if (top == -1)
    {
        printf("\nNothing to undo (stack underflow)!\n");
        return;
    }

    s = undoStack[top--];

    if (insertBST(s))
    {
        printf("\nRestored student: %d - %s\n", s.rollNo, s.name);
    }
}


/* -------------------------------------------------
   QUEUE FUNCTIONS (ADMISSION WAITING LIST)
   ------------------------------------------------- */

/* Enqueue a new applicant */
void enqueueAdmission()
{
    struct Student s;

    if (queueCount == MAX_QUEUE)
    {
        printf("\nAdmission queue is full!\n");
        return;
    }

    printf("\nEnter Roll Number: ");
    scanf("%d", &s.rollNo);

    readStudentDetails(&s);

    rear = (rear + 1) % MAX_QUEUE;
    admissionQueue[rear] = s;
    queueCount++;

    printf("\nApplicant added to admission queue!\n");
}


/* Dequeue the next applicant and admit into BST */
void processAdmission()
{
    struct Student s;

    if (queueCount == 0)
    {
        printf("\nAdmission queue is empty!\n");
        return;
    }

    s = admissionQueue[front];
    front = (front + 1) % MAX_QUEUE;
    queueCount--;

    if (insertBST(s))
    {
        printf("\nAdmitted: %d - %s\n", s.rollNo, s.name);
    }
    else
    {
        printf("\nAdmission rejected for %s.\n", s.name);
    }
}


/* Display admission queue */
void displayQueue()
{
    int i, idx;

    if (queueCount == 0)
    {
        printf("\nAdmission queue is empty!\n");
        return;
    }

    printf("\n---------------------------------------------\n");
    printf("Roll No\t\tName\t\tMarks\n");
    printf("---------------------------------------------\n");

    idx = front;

    for (i = 0; i < queueCount; i++)
    {
        printf("%d\t\t%s\t\t%.2f\n",
               admissionQueue[idx].rollNo,
               admissionQueue[idx].name,
               admissionQueue[idx].marks);

        idx = (idx + 1) % MAX_QUEUE;
    }
}


/* -------------------------------------------------
   SORTING (MERGE SORT) - RANK LIST
   ------------------------------------------------- */

/* Merge two sorted halves (descending by marks) */
void merge(struct Student arr[], int left, int mid, int right)
{
    int i, j, k;
    int n1 = mid - left + 1;
    int n2 = right - mid;

    struct Student *L = (struct Student *)malloc(n1 * sizeof(struct Student));
    struct Student *R = (struct Student *)malloc(n2 * sizeof(struct Student));

    for (i = 0; i < n1; i++)
    {
        L[i] = arr[left + i];
    }

    for (j = 0; j < n2; j++)
    {
        R[j] = arr[mid + 1 + j];
    }

    i = 0;
    j = 0;
    k = left;

    while (i < n1 && j < n2)
    {
        if (L[i].marks >= R[j].marks)
        {
            arr[k++] = L[i++];
        }
        else
        {
            arr[k++] = R[j++];
        }
    }

    while (i < n1)
    {
        arr[k++] = L[i++];
    }

    while (j < n2)
    {
        arr[k++] = R[j++];
    }

    free(L);
    free(R);
}


void mergeSort(struct Student arr[], int left, int right)
{
    int mid;

    if (left < right)
    {
        mid = left + (right - left) / 2;

        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);

        merge(arr, left, mid, right);
    }
}


/* Display rank list (highest marks first) */
void displayRankList()
{
    int i, index = 0;
    struct Student *arr;

    if (root == NULL)
    {
        printf("\nNo students available for ranking!\n");
        return;
    }

    arr = (struct Student *)malloc(studentCount * sizeof(struct Student));

    if (arr == NULL)
    {
        printf("\nMemory allocation failed!\n");
        return;
    }

    fillArray(root, arr, &index);

    mergeSort(arr, 0, studentCount - 1);

    printf("\n---------------------------------------------\n");
    printf("Rank\tRoll No\t\tName\t\tMarks\n");
    printf("---------------------------------------------\n");

    for (i = 0; i < studentCount; i++)
    {
        printf("%d\t%d\t\t%s\t\t%.2f\n",
               i + 1,
               arr[i].rollNo,
               arr[i].name,
               arr[i].marks);
    }

    free(arr);
}


/* -------------------------------------------------
   MAIN FUNCTION
   ------------------------------------------------- */

int main()
{
    int choice;

    do
    {
        printf("\n\n========================================");
        printf("\n       STUDENT MANAGEMENT SYSTEM");
        printf("\n========================================");

        printf("\n\n--- BST OPERATIONS ---");

        printf("\n1. Add Student");
        printf("\n2. Display Students (Sorted by Roll No)");
        printf("\n3. Search Student");
        printf("\n4. Update Marks");
        printf("\n5. Delete Student");
        printf("\n6. Display Rank List (Merge Sort)");

        printf("\n\n--- STACK OPERATIONS ---");

        printf("\n7. Undo Last Delete");

        printf("\n\n--- QUEUE OPERATIONS ---");

        printf("\n8. Add Applicant to Admission Queue");
        printf("\n9. Process Next Admission");
        printf("\n10. Display Admission Queue");

        printf("\n\n11. Exit");

        printf("\n\nEnter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            /* Invalid (non-numeric) input: clear buffer */
            while (getchar() != '\n');
            choice = 0;
        }

        switch (choice)
        {
            case 1:
                addStudent();
                break;

            case 2:
                displayStudents();
                break;

            case 3:
                searchStudent();
                break;

            case 4:
                updateMarks();
                break;

            case 5:
                deleteStudent();
                break;

            case 6:
                displayRankList();
                break;

            case 7:
                undoDelete();
                break;

            case 8:
                enqueueAdmission();
                break;

            case 9:
                processAdmission();
                break;

            case 10:
                displayQueue();
                break;

            case 11:
                printf("\nThank you for using Student Management System!\n");
                break;

            default:
                printf("\nInvalid choice! Please try again.\n");
        }

    } while (choice != 11);

    freeTree(root);

    return 0;
}
