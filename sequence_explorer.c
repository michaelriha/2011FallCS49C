/*
 * Michael Riha
 * CS 49C
 * Section 1
 * Assignment #8
 *
 * This is a "sequence explorer" program. It calculates how many iterations it
 * will take for an input number to reach 1 given the equations:
 * 
 * if n is even: N(n,b,m) = n/b
 * if n is odd : N(n,b,m) = n(b^m+1)+b^m-(n%b^m)
 *
 * It should work for both positive and negative numbers (it ignores 0).
 * My implementation only required a one dimensional array, probably due to
 * the way the recursive getSteps() function works, but I could not think
 * of a way to implement it iteratively.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define b         2     // default: b = 2, m = 1 | loops on positives for b = 2, m = 2
#define m         1
#define SIZE      80    // Size of input
#define BLOCKSIZE 4096  // Blocksize can be anything within reaeson
#define elif else if

struct infinite_array {
    int data[BLOCKSIZE];
    struct infinite_array *next;
};
typedef struct infinite_array ia;

/* Code more or less copied from Oualline */
static ia *ia_locate(ia *array, int index, int *current_index)
{
    ia *current    = array;
    *current_index = index;

    while(*current_index >= BLOCKSIZE) {
        if (current->next == NULL) {
            current->next = malloc(sizeof(ia));
            if (current->next == NULL) {
                fprintf(stderr, "Error: Out of memory. Press enter to exit\n");
                getchar();
                exit(EXIT_FAILURE);
            }
            memset(current->next, '\0', sizeof(ia));
        }
        current = current->next;
        *current_index -= BLOCKSIZE;
    }
    return current;
  }

/* Sets the value of the given index of the given array to data */
void ia_store(ia *array, int index, int data)
{
    ia *current;
    int cur_idx;
    current = ia_locate(array, index, &cur_idx);
    current->data[cur_idx] = data;
}

/* Gets the data at the given index in the given array */
int ia_get(ia *array, int index)
{
    int cur_idx;
    ia *current = ia_locate(array, index, &cur_idx);
    return (current->data[cur_idx]);
}
/* End provided code */

/* Calculates the next step of n */
int getNextN(int n)
{
    if (n == 1) return 1;
    int N = 0;
    if (n % b == 0)
        N = n / b;
    else
    {
        int p = pow(b, m);
        N = (p + 1) * n + p - (n % p);
    }
    return N;
}

/* Same as getNextN() but for negative numbers */
int getNextNeg(int n)
{
    if (n == -1) return -1;
    int N = 0;
    if (n % b == 0)
        N = n / b;
    else
    {
        int p = pow(b, m);
        N = (p + 1) * n + p + (n % p); // + (n%p) moves closer to -1 as intended
    }
    return N;
}

/* Recursively calculate number of steps for given n and stores any needed values into array*/
int getSteps(ia *array, int n)
{
    if (n == 1) return 0;
    
    int steps = ia_get(array, n);

    if   (steps < 0) return -555555; // make final value negative since cannot 'break' recursion
    elif (steps > 0) return steps;
    elif (steps == 0)
    { // Set the array index to -1 to tag it in casae of a loop
        ia_store(array, n, -1);

        // Get the amount of steps recursively
        steps = 1 + getSteps(array, getNextN(n));

        // Store the value and return the amount of steps it took
        ia_store(array, n, steps);
        return steps;
    }
}

/* Same as getSteps() but for  negative numbers*/
int getStepsNeg(ia *array, int neg)
{
    if (neg == -1) return 0;

    int steps = ia_get(array, -neg);
    
    if   (steps  < 0) return -555555;
    elif (steps  > 0) return steps;
    elif (steps == 0)
    {
        ia_store(array, -neg, -1);
        steps = getStepsNeg(array, getNextNeg(neg));
        ++steps;
        ia_store(array, -neg, steps);
        return steps;
    }
}

/* Removes trailing \n and converts str to lowercase */
void trimAndLower(char str[]) {
    unsigned int len = strlen(str);
    for (int i = 0; i < len - 1; i++) if (str[i] < 91 && str[i] > 57) str[i] += 32;
    if (str[--len] == '\n') str[len] = '\0';
}

/* Parses a character array and returns decimal representation */
int parseString(char s[], int len) {
    unsigned int place = 1; // holds decimal place factor
    unsigned int dec   = 0; // return value

    for (int i = len-2; i >= (s[0] == '-') ? 1 : 0; --i)  {
        dec += (s[i] - 48) * place;
        place *= 10;
    }   return (s[0] == '-') ? -dec : dec;
}

int main()
{
    int n;
    int steps;
    int largestn  = 0;
    int smallestn = 0;
    char isAlpha  = 0;
    char input[SIZE];

    // Initialize the infinite arrays for positive and negative numbers
    ia *pos = malloc(sizeof(ia));
    pos->next = NULL;
    memset(pos, '\0', sizeof(ia));
    ia *neg = malloc(sizeof(ia));
    neg->next = NULL;
    memset(neg, '\0', sizeof(ia));

    printf("Welcome to the sequence explorer program!\nb=%d, m=%d, blocksize=%d\n", b, m, BLOCKSIZE);
    while(1)
    {
        printf("Enter a command: ");
        fgets(input, SIZE, stdin);
        isAlpha = 0;

        for (int i = (input[0] == '-') ? 1 : 0; i < strlen(input)-1; ++i)
            if (input[i] < '0' || input[i] > '9') { 
                isAlpha = 1;
                break;
            }

        if (isAlpha) {
            trimAndLower(input);
            if   (strcmp(input, "help") == 0)
                printf("<n>    Calculates how many steps for n to reach 1\nhelp   Prints this help documentation\nquit   Quits the program");
            elif (strcmp(input, "quit") == 0)
                break;
            else
                printf("Command not understood.");
            printf("\n");
        }
        else {
            n = parseString(input, strlen(input));
            if (n >= 0) {
                for (int i = largestn+1; i <= n; ++i) {                    
                    steps = getSteps(pos, i);
                    if (steps >= 0)
                        printf("%d terminates at 1 after %d steps.\n", i, steps);
                    else
                        printf("%d creates a loop.\n", i);
                }
                if (n > largestn)  largestn = n;
            }
            else {
                for (int i = smallestn-1; i >= n; --i) {
                    steps = getStepsNeg(neg, i);
                    if (steps >= 0)
                        printf("%d terminates at -1 after %d steps.\n", i, steps);
                    else
                        printf("%d creates a loop.\n", i);
                }
                if (n < smallestn) smallestn = n;
            }
        }
    }
    printf("Have a nice day!\n");
    exit(EXIT_SUCCESS);
}
