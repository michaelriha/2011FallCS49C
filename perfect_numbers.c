/*
 * Michael Riha
 * CS 49C
 * Section 1
 * Assignment #9
 *
 * This program finds groups of numbers whose divisor sums meet certain criteria.
 * For example, it finds amicable pairs and perfect numbers in addition to their cousins
 * "congenial triplets" and "harmonious quadruplets"
 *
 * I didn't find any triplets or quadruplets in my testing but I did find the
 * quintuplet: 12496, 14288, 15472, 14536, 14264
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define SIZE      80    // Size of input
#define BLOCKSIZE 4096  // Size of infinite array blocks
#define elif else if

struct infinite_array {
    int data[BLOCKSIZE];
    struct infinite_array *next;
};
typedef struct infinite_array ia;

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

/* Calculates the proper divisors of n and returns the sum. Expected values [2,2^32-1] */
unsigned int sumDivisors(unsigned int n) {
    if (n == 2) return 1;
    unsigned int sum = 0;
    for (unsigned int i = 1; i <= n >> 1; ++i) if (n % i == 0) sum += i;
    return sum; //returns 1 if prime
}

/* iteratively check the properties of n. Returns 1 if perfect, the second number of the pair
 if amicable, -1 if prime, a number between 1 and 200 if it reaches a perfect instead of a prime,
 and a negative number less than -1 if it loops in a triplet, quadruplet, etc. */
int checkProperties(ia *array, int n)
{
    int current = ia_get(array, n);

    ia_store(array, n, -1);

    int sum = sumDivisors(n);

    if   (sum == 1) current = -1;  // prime
    elif (sum == n) current = 1;   // perfect
    else
    {
        int timetoloop = 1;
        int sum2 = sumDivisors(sum);
        if (sum2 == n)
            current = sum;
        else
        {
            while (sum2 != n )
            {
                if (++timetoloop > 9)
                    return -1;
                sum  = sum2;
                sum2 = sumDivisors(sum2);
                if (sum2 == sum) // terminates at a perfect number, return # of iterations
                    return timetoloop;
                elif(sum == 1)
                    return -1; // terminates at 1
            }
            current = -timetoloop; // triplet, quadruplet, etc
        }
    }
    ia_store(array, n, current);
    return current;
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
    char isAlpha  = 0;
    char input[SIZE];

    ia *array = malloc(sizeof(ia));
    array->next = NULL;
    memset(array, '\0', sizeof(ia));

    printf("Welcome to the sigma loop program!\n");
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
                printf("<n>    Prints the properties of numbers up to n\nhelp   Prints this help documentation\nquit   Quits the program");
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
                    steps = checkProperties(array, i);
                    if   (steps == 1)
                        printf("%d is a perfect number.\n", i);
                    elif (steps > 200 && i < steps)
                        printf("%d and %d form an amicable pair.\n", i, steps);
                    elif (steps > 0 && steps < 200)
                        printf("%d is perfect after %d iterations.\n", i, steps-1);
                    elif (steps == -2)
                        printf("%d is part of a congenial triple.\n", i);
                    elif (steps == -3)
                        printf("%d is part of a harmonious quadruplet.\n", i);
                    elif (steps == -4)
                        printf("%d is part of a quintuplet.\n", i);
                }
                if (n > largestn) largestn = n;
            }
        }
    }
    printf("Have a nice day!\n");
    exit(EXIT_SUCCESS);
}
