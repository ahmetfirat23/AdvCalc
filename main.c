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

struct token{
    token_type token_type;
    char token_val[256];
    struct token * next;
    struct token * prev;
};

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
struct token func_and_var_parser(char ** exp){
    struct token token;
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
struct token int_parser(char** exp){
    struct token token;
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
struct token sign_parser(char **exp){
    struct token token;
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

struct token eol_parser(){
    struct token token;
    token.token_type = EOL;
    strcpy(token.token_val,"<EOL>");
    return token;
}
/*
 * Returns token of type variable
 * */
struct token lhs_parser(char* exp){

}

/*
 * Return 0 on success
 * Return -1 on error
 * */
int lexer(char *p, int length, struct token** head, struct token** tail, struct token** p_equal){ // Initialize p_equal to NULL
    int idx = 0;
    struct token * prev_token;
    for(int i = 0; i<length; i++){
        struct token * token = malloc(sizeof(struct token));
        if(*p=='\n'){
            (*token) = eol_parser();
            (*tail) = token;
            if((*head)==NULL){
                (*head) = token;
                (*head)->next = NULL;
                (*head)->prev = NULL;
            }
            else{
                prev_token->next = (*tail);
                (*tail)->prev = prev_token;
            }
            break;
        }
        else if(isspace(*p)){
            p += 1;
            continue;
        }
        else if(isalpha(*p)){
            (*token) = func_and_var_parser(&p);
        }
        else if(isdigit(*p)){
            (*token) = int_parser(&p);
        }
        else if(is_sign(*p)){
            (*token) = sign_parser(&p);

            if(*(p-1)=='='){
                if((*p_equal)==NULL){
                    (*p_equal) = token;
                }
                else{
                    return -1;
                }
            }

            if((*token).token_type==COMMENT){
                (*token) = eol_parser();
                (*tail) = token;
                if((*head)==NULL){
                    (*head) = token;
                    (*head)->next = NULL;
                    (*tail)->prev = NULL;
                }
                else{
                    prev_token->next = (*tail);
                    (*tail)->prev = prev_token;
                }
                break;
            }
        }
        else{
            return -1;
        }
        if ((*head)==NULL){
            (*head) = token;
            (*head)->prev = NULL;
        }
        else{
            prev_token->next = token;
            (*token).prev = prev_token;
        }
        prev_token = token;
        idx++;
    }
    return 0;
}

int exp_syntax_checker(struct token* head){
    struct token* iter = head;
    int p_count = 0;
    int func_count = 0;
    while(iter->token_type!=EOL){
        if (p_count < 0 || func_count < 0){
            return -1;}
        token_type type = iter->token_type;
        token_type next_type = iter->next->token_type;
        if(iter->prev==NULL || iter->prev->token_type==EQUAL) {
            if (type==VAR || type==INT) {
                if(next_type==SUM || next_type==MULTI || next_type==MINUS || next_type==B_AND
                || next_type==B_OR || next_type==EOL){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==OPEN_P){
                p_count++;
                if(next_type==VAR || next_type==INT || next_type==OPEN_P|| next_type==LS
                || next_type==RS || next_type==LR || next_type==RR || next_type==NOT){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==B_XOR || type==LS || type==RS || type==LR || type==RR || type==NOT){
                if(type!=NOT){
                func_count++;
                }
                if(next_type==OPEN_P){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else{
                return -1;
            }
        }
        else{
            if (type==VAR || type==INT) {
                if(next_type==CLOSE_P || next_type==SUM || next_type==MULTI || next_type==MINUS
                || next_type==B_AND || next_type==B_OR  || next_type==COMMA || next_type==EOL){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==OPEN_P){
                p_count++;
                if(next_type==VAR || next_type==INT || next_type==OPEN_P|| next_type==LS
                || next_type==B_XOR || next_type==RS || next_type==LR || next_type==RR
                || next_type==NOT){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==CLOSE_P){
                p_count--;
                if(next_type==CLOSE_P || next_type==SUM || next_type==MULTI || next_type==MINUS
                   || next_type==B_AND || next_type==B_OR || next_type==COMMA || next_type==EOL){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==SUM || type==MULTI || type==MINUS || type==B_AND || type==B_OR
                    || type == COMMA){
                if(type==COMMA){
                    func_count--;
                }
                if(next_type==VAR || next_type==INT || next_type==OPEN_P || next_type==B_XOR
                || next_type==LS || next_type==RS || next_type==LR || next_type==RR
                || next_type==NOT){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else if(type==B_XOR || type==LS || type==RS || type==LR || type==RR || type==NOT){
                if(type!=NOT){
                    func_count++;
                }
                if(next_type==OPEN_P){
                    iter = iter->next;
                    continue;
                }
                else{
                    return -1;
                }
            }
            else{
                return -1;
            }
        }

    }
    if (p_count!=0 || func_count!=0){
        return -1;
    }
    return 0;
}

int assign_syntax_checker(struct token* head, struct token* p_equal){
    if(head->token_type!=VAR){
        return -1;
    }
    if(head->next != p_equal){
        return -1;
    }
    if(p_equal->next->token_type==EOL){
        return -1;
    }
    else{
        return exp_syntax_checker(p_equal->next);
    }
}

int remove_commas(struct token* head){
    int idx = 0;
    struct token * iter = head;
    struct token * stack[256];
    while(iter->token_type!=EOL){
        token_type type = iter->token_type;
        if(type==B_XOR || type==LS || type==RS || type==LR || type==RR){
            stack[idx] = iter;
            idx++;
        }
        else if(type==COMMA){
            struct token * func = stack[--idx];
            struct token * tmp = func->prev;
            func->prev->next = func->next;
            func->next->prev = tmp;
            iter->token_type = func->token_type;
            strcpy(iter->token_val, func->token_val);
        }
        iter = iter->next;
    }
    if(idx!=0){
        return -1;
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
        struct token *head = NULL;
        struct token * tail = NULL;
        struct token *p_equal = NULL;
        if(lexer(p, strlen(p), &head, &tail,&p_equal) != -1){
            struct token* iter = head;
            int a = 0;
            while(iter->token_type!=EOL){
                printf("%d:     ",a);
                printf("%s\n",iter->token_val);
                a++;
                iter = iter->next;
            }
        }
        else{
            printf("Error!\n");
            printf("%s ",">");
            continue;
        }
        if(p_equal!=NULL){
            if(assign_syntax_checker(head, p_equal)!=0){
                printf("Error!\n");
                printf("%s ",">");
                continue;
            }
        }
        else{
            if(exp_syntax_checker(head)!=0){
                printf("Error!\n");
                printf("%s ",">");
                continue;
            }
        }
        if(remove_commas(head)==0){
            struct token* iter = head;
            int a = 0;
            while(iter->token_type!=EOL){
                printf("%d:     ",a);
                printf("%s\n",iter->token_val);
                a++;
                iter = iter->next;
            }
        }
        else{
            printf("Error!");
            printf("%s ",">");
            continue;
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
