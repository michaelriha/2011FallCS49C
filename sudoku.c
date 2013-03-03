/*
 * Michael Riha
 * CS 49C
 * Section 1
 * Assignment #10
 *
 * This program solves sudoku puzzles. Its output is split by nonet for extra credit.
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#define SIZE 80 // size of input
#define HEIGHT 1 // dictates the size of the small boxes
#define WIDTH 3
#define MAXIMUM 36

uint32_t GRIDSIZE = 9;
char marked[WIDTH+1];   // marked box column-row (part of row equal to one column)
char unmarked[WIDTH+1]; // unmarked box column-row
char major[WIDTH+1];      // major/bottom/sep column-row
char data[MAXIMUM][MAXIMUM];      // keeps track of marked or unmarked boxes

void help() {
    printf("\nThis program is an interactive sudoku solver\n"
            "Commands:\n"
            "rows <n>        Sets rows and columns to n\n"
            "columns <n>     Sets rows and columns to n\n"
            "<n1>,<n2>,<n3>  Marks the box at row n1, column n2 with value n3\n"
            "reset           Resets the grid to a 9x9 with no marked boxes\n"
            "solve           Solves the current puzzle\n"
            "unsolve         Returns the grid to its state before solving\n"
            "help            What you're viewing now\n"
            "quit            Exits the program\n\n");
}

/* Removes trailing \n and converts str to lowercase */
void trimAndLower(char str[]) {
    unsigned int len = strlen(str);
    for (int i = 0; i < len - 1; i++) if (str[i] < 91 && str[i] > 57) str[i] += 32;
    if (str[--len] == '\n') str[len] = '\0';
}

/* Parses a character array and returns decimal representation */
unsigned int parseString(char s[], int len) {
    unsigned int place = 1; // holds decimal place factor
    unsigned int dec   = 0; // return value

    for (int i = len-2; i >= 0; --i)  {
        dec += (s[i] - 48) * place;
        place *= 10;
    }   return dec;
}

/* Prints the grid with size rows and columns */
void printGrid(unsigned int size) {
    char maj[size*WIDTH];    // separator row for top/bottom/between major rows
    char min[size*WIDTH];    // holds minor rows
    uint32_t boxsize = sqrt(size); // size of inner grids

    for (int i = 0; i < size*WIDTH; ++i) {
        maj[i] = 0;
        min[i] = 0;
    }

    printf("\t");
    for (int i = 0; i < size; ++i) {
        // Print column numbers
        for (int j = 2; j < WIDTH; ++j)
            printf(" ");
        printf("%2d ", i+1);

        // Build major and minor separators
        strcat(maj, "#===");
        strcat(min, (i % boxsize == 0) ? "#---" : "+---");
    }
    strcat(maj, "#\n\0");
    strcat(min, "#\n\0");

    // Print the top
    printf("\n\t%s", maj);

    for (int i = 0; i < size; ++i) {
        printf("%6d", i+1);
        for (int k = 0; k < HEIGHT; ++k) {
            printf("\t");
            for (int j = 0; j < size; ++j)
            {
                printf((j % boxsize == 0) ? "# " : "| ");
                if (data[i][j])
                    printf("%d ", data[i][j]);
                else
                    printf("  ");
            }
            printf("#\n");
        }
        printf("\t%s", (i % boxsize == ((boxsize == 3) ? 2 : 1)) ? maj : min);
    }
}

/* Attempts to mark a box, returns 0 if invalid entry */
char tryProcessNumbers(char input[]) {
    uint32_t len = strlen(input); // more efficient
    uint32_t commaindex[2]; // stores comma indices
    char commas = 0; // how many commas in input

    // check that there are exacatly 2 commas, store their indices
    for (int i = 0; i < len; ++i) {
        if (input[i] == '.' || input[i] > 57) return 0;
        if (commas > 2)                       return 0;
        if (input[i] == ',')                { commaindex[commas] = i; ++commas; }
    } if (commas != 2) return 0;

    // Get the input values
    uint32_t n1 = parseString(input, commaindex[0]+1);
    uint32_t n2 = parseString(&input[commaindex[0]+1], commaindex[1] - commaindex[0]);
    uint32_t n3 = parseString(&input[commaindex[1]+1], len - commaindex[1] - 1);

    if (n3 > GRIDSIZE) { printf("n3 value must be between 0 and %d\n", GRIDSIZE); return 0; }

    // Mark the grid at n1,n2 with valaue n3
    if (!(n1 > MAXIMUM || n2 > MAXIMUM)) {// segfault on e.g. 3,52342 without this check
        data[--n1][--n2] = n3;
        if (n1 > GRIDSIZE || n2 > GRIDSIZE) printf("Box is off the board\n");
    }
    else { printf("You're way off the board. Command not registered.\n"); return 0; }

    return 1;
}

/* Validates the current grid */
static int checkGrid(int row, int col, int val)
{
    uint32_t nonetsize = sqrt(GRIDSIZE); // size of the nonet, e.g. 2 for 4x4, 3 for 9x9
    uint32_t nonetrow = (row / nonetsize) * nonetsize; // index of nonet row
    uint32_t nonetcol = (col / nonetsize) * nonetsize; // index of nonet col
    uint32_t i, j;

    // Check if number is already in col
    for (i = 0; i < GRIDSIZE; ++i) {
        if (i != row) // skip current spot
            if (data[i][col] == val)
                return 0;
    }
    
    // Check if number is already in row
    for (j = 0; j < GRIDSIZE; ++j) {
        if (j != col) // skip current spot
            if (data[row][j] == val)
                return 0;
    }

    // Check if number is in the nonet
    for (i = nonetrow; i < nonetrow+nonetsize; ++i)
        for (j = nonetcol; j < nonetcol+nonetsize; ++j) {
            if ((i != row) || (j != col))
                if (data[i][j]==val)
                    return 0;
    }
    return 1;
}

static int solutions = 0;

/* Recursively solve a sudoku grid */
static int solve(int row, int col)
{
    int value;

    if (col == GRIDSIZE) {  // reached end of row
        col = 0;
        row++;
    }

    if (row == GRIDSIZE) { // row number off grid, so solution has been found
        ++solutions;
        printf(" %d solution(s) found.\n", solutions);
        return 1;
    }

    if (data[row][col]) { // there's already something at this position, check the next one
        if (solve(row, col+1))
            return 1;
    }
    else { // try all possible values at current position
        for (value = 1; value < GRIDSIZE+1; ++value)
            if (checkGrid(row, col, value)) { // check if value works
                data[row][col]=value;
                if (solve(row, col+1)) // if it works, check the next square
                    return 1;
                }
        data[row][col] = 0; // Reset square
    }
    return 0;
}

void reset() {
    GRIDSIZE = 9;
    for (int i = 0; i <= MAXIMUM; ++i)
        for (int j = 0; j <= MAXIMUM; ++j)
            data[i][j] = 0;
    printf("Board has been reset\n");
}

int main() {
    char input[SIZE];
    char lastGrid[MAXIMUM][MAXIMUM];
    char* num; // pointer to the number for the columns / rows  command
    int len;   // length of input

    // Initialize grids
    for (int i = 0; i < MAXIMUM + 1; ++i)
        for (int j = 0; j < MAXIMUM + 1; ++j) {
            data[i][j] = 0;
            lastGrid[i][j] = 0;
        }

    printf("Welcome to the sudoku solver program!\n");
    while(1) {
        printGrid(GRIDSIZE);
        printf("\nEnter a command: ");
        fgets(input, SIZE, stdin);

        // check if the user entered a number  n1,n2,n3 or other input
        if (!tryProcessNumbers(input)) {
            len = strlen(input);  // store the input length if user entered columns or rows
            trimAndLower(input);  // lowercase the input and trim \n for strcmp()
            int i;                // need for length argument of parseString()

            // iterate through input string and check for #. num -> first numeric value
            // null terminate the input before the numeric value to check if the user typed rows / columns
            for (i = 0; i < len; ++i) {
                if (input[i] < 58 && input[i] > 47) {
                    input[i-1] = '\0';
                    num = &input[i];
                    break;
                }
            }
            if (strcmp(input, "solve")== 0) {
                solutions = 0;
                for (int r = 0; r < MAXIMUM; ++r)
                    memcpy(lastGrid[r], data[r], MAXIMUM); // store last grid
                solve(0,0);
            }
            else if (strcmp(input, "unsolve")==0)
                for (int r = 0; r < MAXIMUM; ++r)
                    memcpy(data[r], lastGrid[r], MAXIMUM); // restore last grid
            else if (strcmp(input, "columns") ==0 || strcmp(input, "rows") == 0) {
                int temp = parseString(num, len-i);
                if (sqrt(temp)*sqrt(temp) == temp)
                    GRIDSIZE = temp;
                else
                    printf("Row/column number must be the square of an integer.\n");
            }
            else if (strcmp(input, "reset")== 0) reset();
            else if (strcmp(input, "help") == 0) help();
            else if (strcmp(input, "quit") == 0) break;
            else printf("Command not understood.\n");
        }
    }
    printf("Come back when you can stay longer!\n");
    return 0;
}
