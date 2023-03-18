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
    COMMENT,
    EOL,
}token_type;

typedef struct{
    token_type token_type;
    char token_val[256];
}token;

char* KEYWORDS[] = {"xor","ls","rs","lr","rr","not"};
char SIGNS[] ={'=','+','-','*','&','|','(',')',',','%'};

int is_sign(char chr){
    for(int i = 0; i< 10;i++){ //There are 10 signs if there is more or less update the number here
        if(chr == SIGNS[i]){
            return 1;
        }
    }
    return 0;
}

int is_keyword(char* word){
    for(int i = 0; i <6; i++){ //There are 6 keywords if there is more or less update the number here
        if(strcmp(word, KEYWORDS[i])==0){
            return 1;
        }
    }
    return 0;
}

/*
 * These functions will receive an iterator pointer when their corresponding type first encountered.
 * They will read from pointer until an unrelevant char was encountered and then they will return the token or throw error.
 * Unrelevance should be defined for each type since it differs which type can follow which type.
 * Pointer's current position must be updated in these functions since it will be used in the loop.
 * */

/*
 * Returns token of type function or variable
 * */
token func_and_var_parser(char ** exp){
    token token;
    char str[256+1] = "";
    int idx = 0;
    while(isalpha(**exp)){
        str[idx] = **exp;
        idx++;
        (*exp)++;
    }
    str[idx] = '\0';
    strcpy(token.token_val,str);
    if(is_keyword(str)==1){
        if(strcmp(str, "xor")==0){
            token.token_type = B_XOR;
        }
        else if(strcmp(str, "ls")==0){
            token.token_type = LS;
        }
        else if(strcmp(str, "rs")==0){
            token.token_type = RS;
        }
        else if(strcmp(str, "lr")==0){
            token.token_type = LR;
        }
        else if(strcmp(str, "rr")==0){
            token.token_type = RR;
        }
        else if(strcmp(str, "not")==0){
            token.token_type = NOT;
        }
    }
    else{
        token.token_type = VAR;
    }
    return token;
}
/*
 * Returns token of type integer
 * */
token int_parser(char** exp){
    token token;
    char num[256+1] = "";
    int idx = 0;
    while(isdigit(**exp)){
        num[idx] = **exp;
        idx++;
        (*exp)++;
    }
    num[idx] = '\0';
    strcpy(token.token_val,num);
    token.token_type = INT;
    return token;
}
/*
 * Returns token of type operator, parenthesis, comma or equal sign
 * */
token sign_parser(char **exp){
    token token;
    if(**exp=='='){
        token.token_type = EQUAL;
    }
    else if(**exp=='+'){
        token.token_type = SUM;
    }
    else if(**exp=='-'){
        token.token_type = MINUS;
    }
    else if(**exp=='*'){
        token.token_type = MULTI;
    }
    else if(**exp=='&'){
        token.token_type = B_AND;
    }
    else if(**exp=='|'){
        token.token_type = B_OR;
    }
    else if(**exp=='('){
        token.token_type = OPEN_P;
    }
    else if(**exp==')'){
        token.token_type = CLOSE_P;
    }
    else if(**exp==','){
        token.token_type = COMMA;
    }
    else if(**exp=='%'){
        token.token_type = COMMENT;
    }
    strcpy(token.token_val, (char[2]){**exp,'\0'});
    (*exp)++;
    return token;
}

token eol_parser(){
    token token;
    token.token_type = EOL;
    strcpy(token.token_val,"<EOL>");
    return token;
}
/*
 * Returns token of type variable
 * */
token lhs_parser(char* exp){

}

/*
 * Return 0 on success
 * Return -1 on error
 * */
int lexer(char *p, int length, token* tokens, char* p_equal){ // Initialize p_equal to NULL
    int idx = 0;
    p_equal = NULL; //for safety
    token token;
    for(int i = 0; i<length; i++){
        if(*p=='\n'){
            token = eol_parser();
            tokens[idx] = token;
            break;
        }
        else if(isspace(*p)){
            p += 1;
            continue;
        }
        else if(isalpha(*p)){
            token = func_and_var_parser(&p);
        }
        else if(isdigit(*p)){
            token = int_parser(&p);
        }
        else if(is_sign(*p)){
            if(*p=='='){
                if(p_equal==NULL){
                    p_equal = p;
                }
                else{
                    return -1;
                }
            }

            token = sign_parser(&p);

            if(token.token_type==COMMENT){
                token = eol_parser();
                tokens[idx] = token;
                break;
            }
        }
        else{
            return -1;
        }
        tokens[idx] = token;
        idx++;
    }
    return 0;
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
    char** vars = calloc(128 ,sizeof(char*));
    int last_var_idx = 0;
    char line[256+1] = "";
    printf("%s ",">");
    while(fgets(line, sizeof(line), stdin)){
        //printf("%s\n",line);
        if(line == NULL){
            break;
        }
        char *p = line;
        token tokens[256];
        char *equal_p = NULL;
        if(lexer(p, strlen(p),tokens,equal_p)!=-1){
            token* token = tokens;
            int a = 0;
            while((*token).token_type!=EOL){
                printf("%d:     ",a);
                printf("%s\n",(*token).token_val);
                a++;
                token += 1;
            }
        }
        else{
            printf("Error!\n");
        }
        printf("%s ",">");
    }
}
//int main() {
//    char** vars = calloc(128, sizeof (char*));
//    int last_var_idx = 0;
//    char line[256+1] = "";
//    printf(">");
//    token tokens[256];
//    int last_token_idx = 0;
//    while(fgets(line,sizeof(line),stdin)) {
//        printf(line);
//        if(line == NULL){
//            break;
//        }
//        char tmp[256 + 1],*p;
//        strcpy(tmp, line);
//        char *is_assignment = strchr(tmp, '=');
//        if (is_assignment != NULL) {
//            if (is_assignment[1] == '=' || strcmp(is_assignment, tmp) == 0) {
//                printf("Error1");
//                return -1;
//            }
//            strcpy(tmp, line);
//            char *assignment = strtok_r(tmp, "=", &p);
//            if (assignment == NULL) {
//                printf("Error2");
//                return -1;
//            }
//            char *expression = strtok_r(NULL, "=", &p);
//            short exp_found = 0;
//            char found_assignment[256] = "";
//            for (int i = 0; i < strlen(assignment); i++) {
//                char target = assignment[i];
//                if (target == ' ') {
//                    if (exp_found == 0) {
//                        continue;
//                    } else if (exp_found == 1) {
//                        exp_found = 2;
//                    }
//                } else if (isalpha(target) != 0) {
//                    if (exp_found == 0) {
//                        exp_found = 1;
//                        char *assign = &assignment[i];
//                        strcat(found_assignment, assign);
//                    } else if (exp_found == 1) {
//                        char *assign = &assignment[i];
//                        strcat(found_assignment, assign);
//                    } else if (exp_found == 2) {
//                        printf("Error3!\n");
//                        return -1;
//                    }
//                } else {
//                    printf("Error4!");
//                    return -1;
//                }
//            }
//            if (exp_found == 0) {
//                printf("Error5!");
//                return -1;
//            }
//
//            assignment = NULL;
//
//            token t1;
//            t1.token_type = EQUAL;
//            strcpy(t1.token_val, found_assignment);
//            tokens[last_token_idx] = t1;
//            last_token_idx++;
//
//            vars[last_var_idx] = found_assignment;
//            printf("%s\n", vars[last_var_idx]);
//            last_var_idx++;
//            printf("%s\n", expression);
//        }
//        //char line[256+1] = "";
//        printf(">");
//    }
//    return 0;
//}
