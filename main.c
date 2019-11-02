#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define BSIZE  128  /* buffer size for saving Identifiers */

int isIdentifier(char *);

int addIdentifier(char*);

int addString(char*);
// we save all the strings (even duplicates)

int isKeyword(char *);

bool isDelimiter(char);

bool isOperator(char *);

//test if a char can be operator or delimiter (according to the next char it can be operator)
bool canBeOperatorOrDelimiter(char);

//adding char to end of string
char *addCharToString(char *, char);

//make substring of a string base on right and left indexes
char *subString(const char *str, int left, int right);

//deleting all the spaces in a string
char *deleteSpace(char *);

// lexing the .b file
void lexer(char *);

/*
Delimiters:
( ) ' " : ; \ , { }
*/
char delimiters[] = {'(', ')', '"', '\'', ';', ':', ',', '\\', '{', '}', '[', ']'};

/*
Keywords:
array boolean char else false for function if
integer print return string true void while
 main
*/
//id of any keyword is index+1
char *keywords[] = {"array", "boolean", "char", "else", "false", "for", "function", "if", "integer", "print", "return",
                    "string", "true", "void", "while"};

/*
Operators:
[ ] f( )            array subscript, function call
++  --              postfix increment, decrement
-  !                unary negation, logical not
ˆ                   exponentiation
* / %               multiplication, division, modulus
+ -                 addition, subtraction
< <= > >= == !=     comparison
&&   ||             logical and, logical or
=                   assignment
*/
char *operators[] = {"++", "--", "-", "!", "^", "*", "/", "%", "+", "-", "<", "<=", ">", "[]", ">=", "==", "!=", "&&",
                     "||", "="};

char operatorStarter[] = {'+', '-', '!', '^', '*', '/', '%', '<', '>', '=', '&', '|', ',', '[','/'};

//identifiers
char* identifiers[BSIZE];
int numId=0;
//keep track of number of saved identifiers

char* strings[BSIZE];
int numS=0;
//keep track of number of saved strings

int main() {
    //location of .b file
    char* path="/home/yasfatft/CLionProjects/B-minor-compiler/example.b";

    // read all the .b file and copy it into  string
    FILE *f = fopen(path, "r");
    char c = fgetc(f);
    char *str = "";
    while (c != EOF) {
        str = addCharToString(str, c);
        c = fgetc(f);
    }
    lexer(str);
}

int isIdentifier(char* str){
    int i;
    for (i = 0; i < numId; i++) {
        if (strcmp(identifiers[i], str) == 0) {
            return i;
        }
    }
    return -1;

}

int addIdentifier(char * str){
    identifiers[numId]=str;
    numId++;
    return numId-1;
}

int addString(char* str){
    strings[numS]=str;
    numS++;
    return numS-1;
}

//keywords IDs are bigger than 0
int isKeyword(char *str) {

    int i;
    for (i = 0; i < sizeof(keywords) / sizeof(char *); i++) {
        if (strcmp(keywords[i], str) == 0) {
            return i+1;
        }
    }
    return 0;
}

bool isDelimiter(char c) {
    int i;
    for (i = 0; i < sizeof(delimiters) / sizeof(char); i++) {
        if (delimiters[i] == c) {
            return true;
        }
    }
    return false;
}

bool isOperator(char *str) {

    int i;
    for (i = 0; i < sizeof(operators) / sizeof(char *); i++) {
        if (strcmp(operators[i], str) == 0) {
            return true;
        }
    }
    return false;
}

bool canBeOperatorOrDelimiter(char str) {
    if (isDelimiter(str)) {
        return true;
    }
    int i;
    for (i = 0; i < sizeof(operatorStarter) / sizeof(char); i++) {
        if (str == operatorStarter[i]) {
            return true;
        }
    }
    return false;
}

char *addCharToString(char *str, char c) {
    size_t len = strlen(str);
    char *str2 = (malloc(len + 1 + 1)); /* one for extra char, one for trailing zero */
    strcpy(str2, str);
    str2[len] = c;
    str2[len + 1] = '\0';
    return str2;
}

char *subString(const char *str, int left, int right) {
    //make substring of size right-left+2 (one extra for \0)
    int i;
    char *subStr = (char *) malloc(
            sizeof(char) * (right - left + 2));

    for (i = left; i <= right; i++)
        subStr[i - left] = str[i];
    subStr[right - left + 1] = '\0';
    return (subStr);
}

char *deleteSpace(char *substr) {
    int i;
    int k = -1;
    for (i = 0; i <= strlen(substr); i++) {
        if (substr[i] != ' ') {
            k++;
        }
        substr[k] = substr[i];
    }
    return substr;
}

void lexer(char *str) {
    int left = 0, right = 1;//two cursor named left and right . string contains chars from left to right-1
    int lineNumber = 1;
    int len = strlen(str);

    while (right <= len) {
        //ignore spaces and enters
        while (str[left] == ' ' || str[left] == '\n') {
            left++;
        }
        if (right <= left) {
            right = left + 1;
            continue;
        }

        if (isdigit(str[left])) {
            printf("%d Digit: %c\n", lineNumber, str[left]);
            lineNumber++;
            left++;
            right = left + 1;
            continue;
        }

        if (canBeOperatorOrDelimiter(str[left])) {
            // check if the line is comment
            if (!strcmp(addCharToString(addCharToString("", str[left]), str[left + 1]), "//")) {
                printf("%d Operator: %s\n", lineNumber, addCharToString(addCharToString("", str[left]), str[left + 1]));
                while (str[left] != '\n') {
                    left++;
                }

                left++;
                right = left + 1;
                continue;
            }else if (isDelimiter(str[left])) {
                //ignore strings value
                int start;
                int end;
                // start and end saved index of the point string begins and finished
                if (str[left] == '"') {
                    left++;
                    start=left;
                    while (str[left] != '"') {
                        left++;
                    }
                    end=left-1;
                    printf("%d Delimiter: %c\n", lineNumber, str[left]);
                    printf("String %d %s\n",addString(subString(str,start,end)),subString(str,start,end));
                }
                if (strcmp(addCharToString("",str[left]) , "'")==0) {
                    left++;
                    start=left;
                    while (strcmp(addCharToString("",str[left]) , "'")!=0) {
                        left++;
                    }
                    end=left-1;
                    printf("%d Delimiter: %c\n", lineNumber, str[left]);
                    printf("String %d %s\n",addString(subString(str,start,end)),subString(str,start,end));
                }
                printf("%d Delimiter: %c\n", lineNumber, str[left]);
                lineNumber++;
                left++;
                right = left + 1;
                continue;
                //first we have to check operators with two chars and then operators with one char
            } else if (isOperator(addCharToString(addCharToString("", str[left]), str[left + 1]))) {
                printf("%d Operator: %s\n", lineNumber, addCharToString(addCharToString("", str[left]), str[left + 1]));
                lineNumber++;
                left = left + 2;
                right = left + 1;
                continue;
            } else if (isOperator(addCharToString("", str[left]))) {
                printf("%d Operator: %c\n", lineNumber, str[left]);
                lineNumber++;
                left++;
                right = left + 1;
                continue;
            }
            //if the right char is safe add it to the substring (safe means not delimiter or operator (keywords and identifiers are safe))
        } else if (!canBeOperatorOrDelimiter(str[right])) {
            right++;
        }

        char *substr = deleteSpace(subString(str, left, right - 1));
        if (strlen(substr) == 0) {
            continue;
        }
        //check that after a keyword must not be a to z or A to Z
        if (isKeyword(substr)!=0 &&
            (str[right] < 'A' || (str[right] > 'Z' && str[right] < 'a') || str[right] > 'z')) {
            printf("%d Keyword: %d %s\n", lineNumber, isKeyword(substr),substr);
            left = right;
            right++;
            lineNumber++;
            //if substring does not pass any of the filters(ifs) it can be identifier
        } else if ((str[right] < 'A' || (str[right] > 'Z' && str[right] < 'a') || str[right] > 'z')) {
            int id;
            if(isIdentifier(substr)!=-1){
                id=isIdentifier(substr);
            } else{
                id=addIdentifier(substr);
            }
            printf("%d Identifier: %d %s\n", lineNumber, id,substr);
            lineNumber++;
            left = right;
            right++;
        }
    }

}