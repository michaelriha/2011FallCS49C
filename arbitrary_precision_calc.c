/*
 * Michael Riha
 * CS 49C
 * Section 1
 * Assignment #7
 *
 * This is a four function calculator utilizing arbitrary-precision decimals
 * stored in a linked list. Everything works as described, but the arithmetic
 * functions have memory leaks that hinder divide() for large difference in values a and b
 *
 * Multiplication is suprisingly fast, but divide is extremely slow as I'm not
 * sure how to implement the 'long division' algorithm, and this assignment already
 * took me long enough to complete.
 *
 * I also implemented the first extra credit to find when commas are enterred wrong.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 512     // Max input size can be anything within reaeson
#define elif else if // Python got this one right

struct node {
    char val;
    struct node *next;
    struct node *prev;
};
typedef struct node digit;

struct dllist {
    digit *head;
    digit *tail;
};
typedef struct dllist list;

list *add(digit *a, digit *b);
list *subtract(digit *a, digit *b);
list *multiply(digit *a, digit *b);
list *divide(list *a, list *b);
char compare(digit *a, digit *b);
char compareAbsolute(digit *a, digit *b);
void deleteList(digit *d);

char sep  = 0;    // Separators on if sep = 1, otherwise off

/* Removes trailing \n and converts str to lowercase */
void trimAndLower(char str[]) {
    unsigned int len = strlen(str);
    for (int i = 0; i < len - 1; i++) if (str[i] < 91 && str[i] > 57) str[i] += 32;
    if (str[--len] == '\n') str[len] = '\0';
}

void help() {
    printf("This program takes 2 inputs and performs arithmetic operations on them."
            "After an operation is performed, the result is stored as the first number.\n"
            "Commands:\n"
            "<n>         A number to store. If only one is stored the second is assumed to be 0\n"
            "+           Adds the last two numbers\n"
            "-           Subtracts the last number from the first\n"
            "* or x      Multiplies the last two numbers\n"
            "/           Divides the first number by the second\n"
            "separators  Turns comma separation in the output on or off\n"
            "clear       Clears both numbers\n"
            "quit        Quits the program\n"
            "help        Prints this help documentation\n");
}

/* Takes a string of numbers as an argument and returns a list of digits */
list * strtollist(char* s)
{
    digit *curr, *head, *tail;

    // Create the head of the list
    head = (digit*) malloc(sizeof(digit));
    tail = head;

    if   (s[0] == '-')            head->val = '-';
    elif (s[0] == '\n')           head->val = 0;
    elif (s[0] <='9'&& s[0]>='0') head->val = s[0]-'0';
    else                          return NULL;

    uint32_t length = strlen(s);
    uint32_t digits = 1;
    for (int i = 1; i < length; ++i)
    {
        if   (s[i] >= '0' && s[i] <= '9')  ++digits;
        elif (s[i] == ',' || s[i] == '\n') continue;
        else                               return NULL;
    }
    char found_comma = 0;
    char bad_spacing = 0;
    char offset      = (2 + digits) % 3; // digits till first comma needed
    for (int i = 1; i < length; ++i)
    {
        if (s[i] >= '0' && s[i] <= '9')
        {   // Create new node
            curr = (digit*) malloc(sizeof(digit));
            curr->val  = s[i]-'0';
            curr->prev = tail;
            curr->next = NULL;

            // Add it to the list
            tail->next = curr;
            tail       = curr;
            if (found_comma) ++offset;
        }
        elif (s[i] == ',')
        {
            if (found_comma && offset != 3)
                bad_spacing = 1;
            else
                found_comma = 1;
            offset = 0;
        }
        elif (s[i] == '\n') continue;
        else return NULL; // Bad input
    }

    if (found_comma && (bad_spacing || offset != 3))
        printf("Incorrect comma spacing detected. Number stored successfully.\n");
    
    list *r = (list*) malloc(sizeof(list));
    r->head = head;
    r->head->prev = NULL;
    r->tail = tail;
    r->tail->next = NULL;
    return r;
}

/* Compare digit a to digit b. Returns: 1 if a > b, 0 if a == b, -1 if a < b*/
char compare(digit *a, digit *b)
{
    if (!a->next && !b->next && a->val == 0 && b->val == 0)
        return 0;
    if (a->val == '-' && b->val != '-')
        return -1;
    if (a->val != '-' && b->val == '-')
        return 1;

    uint32_t lena = 0;
    uint32_t lenb = 0;
    a             = (a->val != '-') ? a : a->next;
    b             = (b->val != '-') ? b : b->next;
    digit * ahead = a;
    digit * bhead = b;

    if (a->val == '-' && b->val == '-')
    {  // if both numbers negative, flip them
        digit *swap;
        swap  = bhead;
        bhead = ahead;
        ahead = swap;
        a     = ahead;
        b     = bhead;
        swap  = NULL;
    }
    while(a)
    {
        if (lena || a->val != 0) ++lena;
        a = a->next;
    }
    while(b)
    {
        if (lenb || b->val != 0) ++lenb;
        b = b->next;
    }
    if   (lena > lenb) return  1;
    elif (lena < lenb) return -1;
    else
    {
        while(ahead && bhead)
        {
            if   (ahead->val > bhead->val) return  1;
            elif (ahead->val < bhead->val) return -1;
            bhead = bhead->next;
            ahead = ahead->next;
        }
    }
    deleteList(ahead);
    deleteList(bhead);
    ahead = NULL;
    bhead = NULL;
    a     = NULL;
    b     = NULL;
    return 0;
}

/* Compare |a| to |b|. Returns: 1 if a > b, 0 if a == b, -1 if a < b*/
char compareAbsolute(digit *a, digit *b)
{
    if (!a->next && !b->next && a->val == 0 && b->val == 0)
        return 0;
    uint32_t lena = 0;
    uint32_t lenb = 0;
    a             = (a->val != '-') ? a : a->next;
    b             = (b->val != '-') ? b : b->next;
    digit * ahead = a;
    digit * bhead = b;

    while(a)
    {
        if (lena || a->val != 0) ++lena;
        a = a->next;
    }
    while(b)
    {
        if (lenb || b->val != 0) ++lenb;
        b = b->next;
    }

    if   (lena > lenb) return  1;
    elif (lena < lenb) return -1;
    else
    {
        while (ahead->val == 0) ahead = ahead->next;
        while (bhead->val == 0) bhead = bhead->next;        
        while(ahead && bhead)
        {
            if   (ahead->val > bhead->val) return  1;
            elif (ahead->val < bhead->val) return -1;
            bhead = bhead->next;
            ahead = ahead->next;
        }
    }
    deleteList(ahead);
    deleteList(bhead);
    ahead = NULL;
    bhead = NULL;
    a     = NULL;
    b     = NULL;
    return 0;
}

/* Deletes the lists whose tails are pointed at by a and b */
void deleteList(digit *d)
{
    while(d->prev) {
        d = d->prev;
        free(d->next);
        d->next = NULL;
    } free(d); d = NULL;
}

/* Adds b to a and returns a list of the result, a>=b>=0 are tail pointers */
list * add(digit *a, digit *b)
{
    char carry   = 0;
    digit *head  = NULL;
    digit *tail  = NULL;
    digit *curr  = NULL;
    digit *atail = a;

    while (a || b)
    {   // create result node
        curr = (digit*) malloc(sizeof(digit));

        // Calculate
        curr->val = (a ? a->val : 0) + (b ? b->val : 0) + carry;
        if (curr->val > 9)
        {
            carry      = 1;
            curr->val %= 10;
        }
        else carry = 0;

        // prepend node to list
        curr->prev = NULL;
        curr->next = head;
        head       = curr;

        if (a) a = a->prev;
        else   a = NULL;
        if (b) b = b->prev;
        else   b = NULL;
    }
    if (carry) // carry final digit
    {
        curr             = (digit*) malloc(sizeof(digit));
        curr->val        = 1;
        curr->prev       = NULL;
        curr->next       = head;
        curr->next->prev = curr;
        head             = curr;
    }
    list *result = (list*) malloc(sizeof(list));
    result->head = head;
    tail         = head;
    while(tail->next)
    { // add prev pointers and get a pointer to the tail
        tail->next->prev = tail;
        tail = tail->next;
    }
    result->tail = tail;
    deleteList(atail);
    atail = NULL;
    head = NULL;
    tail = NULL;
    curr = NULL;
    return result;
}

/* Subtracts b from a and returns a list containing the result. Assumes a > b and a and b are tail pointers */
list * subtract(digit *a, digit *b)
{
    char borrow  = 0;
    digit *tail  = NULL;
    digit *head  = NULL;
    digit *curr  = NULL;
    digit *atail = a;

    while (a || b)
    {
        curr = (digit*) malloc(sizeof(digit));
        if (!b)
        { // if no second number, simply carry the values from a
            if   (a ? a->val == 0 : 0)
                curr->val = borrow ? 9 : 0;
            else
            {
                curr->val = a->val - borrow;
                borrow    = 0;
            }
        }
        else
        {
            curr->val = (a ? a->val : 0) - b->val - borrow;
            if (curr->val < 0)
            {
                borrow     = 1;
                curr->val += 10;
            }
            else
                borrow = 0;
            b = b->prev;
        }
        // insert new node
        curr->prev = NULL;
        curr->next = head;
        head       = curr;
        if (a)   a = a->prev;
    }
    list *result = (list*) malloc(sizeof(list));
    result->head = head;
    tail         = head;
    while(tail->next)
    { // add prev pointers and get a pointer to the tail
        tail->next->prev = tail;
        tail             = tail->next;
    }
    result->tail = tail;
    deleteList(atail);
    atail = NULL;
    tail  = NULL;
    head  = NULL;
    curr  = NULL;
    return result;
}

/* Multiplies b * a by summing intermediate results (e.g. 54*62 = 60*54 + 2*54)
   Returns a list containing the result. Assumes a>=b>=0 and a and b are tail pointers */
list * multiply(digit *a, digit *b)
{
    char carry   = 0;
    char place   = 0;
    digit *atail = a;
    digit *btail = b;

    list *result = (list*)  malloc(sizeof(list));
    digit *r     = (digit*) malloc(sizeof(digit));
    result->tail = r;

    digit *head, *curr;
    head = NULL;

    while (b)
    {
        curr       = (digit*) malloc(sizeof(digit));
        curr->val  = 0;
        curr->prev = NULL;
        curr->next = head;
        head       = curr;

        if (!a) // intermediate result finished calculating
        {
            curr->val = carry;
            carry     = 0;
            b         = b->prev;
            a         = atail;

            digit *cur = head;
            while (cur->next)
            { // traverse this result list and set  cur to the tail. Add prev pointers for add
                cur->next->prev = cur;
                cur = cur->next;
            }
            for (int p = place; p > 0; --p)
            { // add zeroes to this intermediate result
                digit *d  = (digit*) malloc(sizeof(digit));
                d->val    = 0;
                d->prev   = cur;
                cur->next = d;
                cur       = d;
            }
            ++place;

            result = add(result->tail, cur);
            head   = NULL;
        }
        else
        {
            curr->val = a->val * b->val + carry;
            if (curr->val > 9)
            {
                carry     = curr->val / 10;
                curr->val = curr->val % 10;
            }
            else carry = 0;
            a = a->prev;
        }
    }
    deleteList(atail);
    deleteList(btail);
    atail = NULL;
    btail = NULL;
    return result;
}

/* Converts an integer to a string */
char* itos(int i)
{
    char inlength = 0;
    int i2        = i;
    while (i2 > 0)
    {
        i2 /= 10;
        ++inlength;
    }
    static char ret[10];
    ret[inlength] = '\0';
    while (i > 0)
    {
        ret[--inlength] = i % 10 + 48;
        i               = i / 10;
    }
    return ret;
}

/* Divides a by b in the most inefficient manner possible (repeat subtraction). Assumes a>b>0 */
list * divide(list *a, list *b)
{
    list *result = (list*) malloc(sizeof(list));
    digit *r     = (digit*) malloc(sizeof(digit));
    r->val       = 0;
    result->head = r;
    result->tail = r;
    list *ilist  = NULL;
    uint32_t i   = 0;

    // while a >= b subtract b from a and increment result
    while (compareAbsolute(a->head, b->head) >= 0) 
    {
        ++i;
        a = subtract(a->tail, b->tail);
        if (i == UINT32_MAX)
        {
            i      = 0;
            ilist  = strtollist(itos(UINT32_MAX));
            result = add(result->tail, ilist->tail);
            deleteList(ilist->tail);
            ilist  = NULL;
        }
    }
    ilist = strtollist(itos(i));
    if (ilist)
    {
        result = add(result->tail, ilist->tail);
        deleteList(ilist->tail);
        ilist = NULL;
    }
    deleteList(b->tail);
    b = NULL;
    return result;
}

/* Prints the list starting with d */
void printList(digit *d)
{
    printf("Result is: ");
    if (d->val == '-') { printf("-"); d = d->next; }
    while (d->val == 0 && d->next) d = d->next; // point head to first nonzero digit
    if (sep)
    {
        digit *head     = d;
        uint32_t length = 0;
        char firstelem  = 1;

        digit *tail = head;
        while (tail)
        {
            tail = tail->next;
            ++length;
        }
        char offset = (length  % 3 == 0) ? -1 : 2 - length % 3; // how many digits till first comma needed
        
        for(digit *curr = head; curr != NULL; curr = curr->next)
        {
            if (++offset == 3 && !firstelem)
            {
                printf(",");
                offset = 0;
            }
            printf("%d", curr->val);
            firstelem = 0;
        }
    }
    else
        for(digit *curr = d; curr != NULL; curr = curr->next)
            printf("%d", curr->val);
    printf("\n");    
}

/* Adds negative sign to head */
digit * negate(digit *head)
{
    digit *neg_sign = (digit*) malloc(sizeof(digit));
    neg_sign->val   = '-';
    neg_sign->next  = head;
    head            = neg_sign;
}

int main()
{    
    char mode = 'A';  // A = collecting list a, B = collecting list b, C = command or replace a with b with input
    char abscmp;
    char input[SIZE];
    list *l = (list*) malloc(sizeof(list)); // Swap storage if user enters more than 2 numbers
    list *a = (list*) malloc(sizeof(list));
    list *b = (list*) malloc(sizeof(list));
    uint32_t len;

    digit *neg_sign = (digit*) malloc(sizeof(digit));
    neg_sign->val = '-';

    printf("Welcome to the 4-function calculator program!\n");
    while(1)        
    {   // Get A, B, or a command
        printf("Enter a number or command: "); // should not change to just number since it will take last 2 numbers as operands
        fgets(input, SIZE, stdin);
        len = strlen(input);
        if (len < 3 && (input[0] > '9' || input[0] < '0'))
        {
            if (mode == 'C'){
                abscmp = compareAbsolute(a->head, b->head);}
            if (mode == 'A')
                printf("A number was expected.\n");
            elif   (input[0] == '+')
            {
                if (mode == 'B')
                    printList(a->head);
                else
                {   // quickly figure out if a+b = 0
                    if (abscmp == 0 && ((a->head->val != '-' && b->head->val == '-') || (a->head->val == '-' && b->head->val != '-')))
                    {
                        digit *d = (digit*) malloc(sizeof(digit));
                        d->val   = 0;
                        a->head  = d;
                        a->tail  = d;
                    } // if both negative, subtract and negate
                    elif (a->head->val == '-' && b->head->val == '-')
                    {
                        a->head       = a->head->next;
                        b->head       = b->head->next;
                        free(b->head->prev);
                        free(a->head->prev);
/*
                        b->head->prev = NULL;
                        a->head->prev = NULL;
*/
                        a             = add(a->tail, b->tail);
                        a->head       = negate(a->head);
                    } // if first number negative, subtract b from a and negate
                    elif (a->head->val == '-' && b->head->val != '-')
                    {
                        a->head       = a->head->next;
                        a->head->prev = NULL;
                        if (abscmp == 1)
                        {
                            a       = subtract(a->tail, b->tail);
                            a->head = negate(a->head);
                        }
                        else
                            a = subtract(b->tail, a->tail);
                    } // if second number negative, subtract a from b and negate
                    elif (a->head->val != '-' && b->head->val == '-')
                    {
                        b->head       = b->head->next;
                        free(b->head->prev);
                        //b->head->prev = NULL;

                        if (abscmp == 1)
                            a = subtract(a->tail, b->tail);
                        else
                        {
                            a = subtract(b->tail, a->tail);
                            a->head = negate(a->head);
                        }
                    }
                    else a = add(a->tail, b->tail);
                    
                    printList(a->head);
                    mode = 'B';
                }
            }
            elif (input[0] == '-')
            {                
                if (mode == 'B')
                    printList(a->head);
                else
                {   // quickly determine if the result is going to be 0
                    if   (abscmp == 0 && ((a->head->val == '-' && b->head->val == '-') || (a->head->val != '-' && b->head->val != '-')))
                    {
                        digit *d = (digit*) malloc(sizeof(digit));
                        d->val   = 0;
                        a->head  = d;
                        a->tail  = d;
                    } // if both numbers negative subtract the smaller from the larger
                    elif (a->head->val == '-' && b->head->val == '-')
                    {
                        a->head         = a->head->next;
                        b->head         = b->head->next;
                        free(a->head->prev);
                        free(b->head->prev);
                        //b->head->prev   = NULL;
                        //a->head->prev   = NULL;
                        
                        if (abscmp == 1)
                        {
                            a       = subtract(a->tail, b->tail);
                            a->head = negate(a->head);
                        }
                        else
                            a = subtract(b->tail, a->tail);
                    } // if first number negative add it to the second and negate
                    elif (a->head->val == '-' && b->head->val != '-')
                    {
                        a->head         = a->head->next;
                        free(a->head->prev);
                        //a->head->prev   = NULL;
                        a               = add(a->tail, b->tail);
                        a->head         = negate(a->head);
                    } // if second number negative add it to the first
                    elif (a->head->val != '-' && b->head->val == '-')
                    {
                        b->head         = b->head->next;
                        free(b->head->prev);
                        //b->head->prev   = NULL;
                        a               = add(a->tail, b->tail);
                    } // if a > b subtract normaaly
                    elif (abscmp == 1)
                        a = subtract(a->tail, b->tail);
                    else // if b > a subtract a from b and negate
                    {
                        a       = subtract(b->tail, a->tail);
                        a->head = negate(a->head);
                    }
                    printList(a->head);
                    mode = 'B';
                }
            }
            elif (input[0] == '*' || input[0] == 'x')
            {
                if (mode == 'B')
                {
                    a->head->val  = 0;
                    a->head->next = NULL;
                    a->head->prev = NULL;
                    a->tail       = a->head;
                    printf("Result is: 0.\n");
                }
                else
                {   // figure out result's sign
                    char neg = 0;
                    if   (a->head->val == '-')
                    {
                        a->head       = a->head->next;
                        free(a->head->prev);
                        //a->head->prev = NULL;
                        neg           = !neg;
                    }
                    if (b->head->val == '-')
                    {
                        b->head       = b->head->next;
                        free(b->head->prev);
                        //b->head->prev = NULL;
                        neg           = !neg;
                    }
                    
                    if  (abscmp == 1 || abscmp == 0) // if a >= b multiply a*b
                        a = multiply(a->tail, b->tail);
                    else // if b > a, multiply b*a
                        a = multiply(b->tail, a->tail);
                    
                    if (neg) a->head = negate(a->head);
                    printList(a->head);
                    mode = 'B';
                }
            }
            elif (input[0] == '/')
            {
                if (mode == 'B' || (mode =='C' && b->head->val == 0 && b->head->next == NULL))
                    printf("Cannot divide by zero.\n");
                else
                {   // figure out the result's sign
                    char neg = 0;
                    if   (a->head->val == '-')
                    {
                        a->head = a->head->next;
                        free(a->head->prev);
                        //a->head->prev = NULL;
                        neg = !neg;
                    }
                    if (b->head->val == '-')
                    {
                        b->head = b->head->next;
                        free(b->head->prev);
                        //b->head->prev = NULL;
                        neg = !neg;
                    }

                    char cmp = compareAbsolute(a->head, b->head);
                    if (cmp == -1)
                    { // if a < b result = 0
                        a->head->val  = 0;
                        a->head->next = NULL;
                        a->head->prev = NULL;
                        a->tail       = a->head;
                    } // same number result = 1
                    elif (cmp == 0)
                    {                        
                        a->head->val  = 1;
                        a->head->next = NULL;
                        a->head->prev = NULL;
                        a->tail       = a->head;
                    }
                    else 
                        a = divide(a, b);

                    if(neg) a->head = negate(a->head);
                    printList(a->head);
                    mode = 'B';
                }
            }
            else printf("Command not understood.\n");
        }
        elif (input[0] != '-' && (input[0] > '9' || input[0] < '0'))
        {
            trimAndLower(input);
            if (strcmp(input,"separators") == 0) {sep = !sep; printf("Separators enabled.\n");}
            elif    (strcmp(input,"clear") == 0) {deleteList(a->tail);deleteList(b->tail);mode='A';printf("Entries cleared.\n");}
            elif    (strcmp(input, "help") == 0) help();
            elif    (strcmp(input, "quit") == 0) break;
            else                                 printf("Command not understood.\n");
        }
        elif   (mode == 'A')
        {
            a = strtollist(input);
            if (a) mode = 'B';
            else   printf("Incorrect number format.\n");
        }
        elif (mode == 'B')
        {
            b = strtollist(input);
            if (b) mode = 'C';
            else   printf("Incorrect number format.\n");
        }
        elif (mode == 'C')
        {
            deleteList(a->tail);
            a = b;
            l = strtollist(input);
            if(l) b = l;
            else  printf("Incorrect number format.\n");
        }
        else printf("A command was expected.\n");
    }
    printf("Have a nice day!\n");
    deleteList(l->tail);
    deleteList(a->tail);
    deleteList(b->tail);
    l = NULL;
    a = NULL;
    b = NULL;
    return EXIT_SUCCESS;
}
