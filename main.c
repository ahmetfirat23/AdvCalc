#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


typedef enum{
    VAR,
    INT,
    OPEN_P,
    CLOSE_P,
    SUM,
    MULTI,
    MINUS,
    EQUAL,
    B_AND,
    B_OR,
    B_XOR,
    LS,
    RS,
    LR,
    RR,
    NOT,
    COMMA,
}token_type;

typedef struct{
    token_type token_type;
    char token_val[256];
}token;

char* KEYWORDS[] = {"xor","ls","rs","lr","rr","not"};

/*
 * These functions will receive an iterator pointer when their corresponding type first encountered.
 * They will read from pointer until an unrelevant char was encountered and then they will return the token or throw error.
 * Unrelevance should be defined for each type since it differs which type can follow which type.
 * Pointer's current position must be updated in these functions since it will be used in the loop.
 * */

/*
 * Returns token of type function or variable
 * */
token func_and_var_parser(char * exp){

}
/*
 * Returns token of type integer
 * */
token int_parser(char* exp){

}
/*
 * Returns token of type operator, parenthesis, comma or equal sign
 * */
token sign_parser(char* exp){

}
/*
 * Returns token of type variable
 * */
token lhs_parser(char* exp){

}

/*
 * In the first loop of line:
 * Each position will be read one by one.
 * Tokens will be extracted and appended to another array.
 * If a token cannot be parsed or an unknown token exist error will be thrown.
 * Encounters of equal sign will be counted. First encounter pointer will be saved.
 * If zero, whole line will be taken as expression.
 * If one, LHS will be taken as assignment. From start to saved pointer will be given to parser.
 * If more, an error will be thrown.
 *
 * Then expression will be evaluated.
 * Expression's correctness will be checked.
 *
 * */

int main() {
    char** vars = calloc(128, sizeof (char*));
    int last_var_idx = 0;
    char line[256+1] = "";
    printf(">");
    token tokens[256];
    int last_token_idx = 0;
    while(fgets(line,sizeof(line),stdin)) {
        printf(line);
        if(line == NULL){
            break;
        }
        char tmp[256 + 1],*p;
        strcpy(tmp, line);
        char *is_assignment = strchr(tmp, '=');
        if (is_assignment != NULL) {
            if (is_assignment[1] == '=' || strcmp(is_assignment, tmp) == 0) {
                printf("Error1");
                return -1;
            }
            strcpy(tmp, line);
            char *assignment = strtok_r(tmp, "=", &p);
            if (assignment == NULL) {
                printf("Error2");
                return -1;
            }
            char *expression = strtok_r(NULL, "=", &p);
            short exp_found = 0;
            char found_assignment[256] = "";
            for (int i = 0; i < strlen(assignment); i++) {
                char target = assignment[i];
                if (target == ' ') {
                    if (exp_found == 0) {
                        continue;
                    } else if (exp_found == 1) {
                        exp_found = 2;
                    }
                } else if (isalpha(target) != 0) {
                    if (exp_found == 0) {
                        exp_found = 1;
                        char *assign = &assignment[i];
                        strcat(found_assignment, assign);
                    } else if (exp_found == 1) {
                        char *assign = &assignment[i];
                        strcat(found_assignment, assign);
                    } else if (exp_found == 2) {
                        printf("Error3!\n");
                        return -1;
                    }
                } else {
                    printf("Error4!");
                    return -1;
                }
            }
            if (exp_found == 0) {
                printf("Error5!");
                return -1;
            }

            assignment = NULL;

            token t1;
            t1.token_type = EQUAL;
            strcpy(t1.token_val, found_assignment);
            tokens[last_token_idx] = t1;
            last_token_idx++;

            vars[last_var_idx] = found_assignment;
            printf("%s\n", vars[last_var_idx]);
            last_var_idx++;
            printf("%s\n", expression);
        }
        //char line[256+1] = "";
        printf(">");
    }
    return 0;
}
