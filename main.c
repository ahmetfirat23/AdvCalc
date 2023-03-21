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
/*
 * VAR_KEYS holds variable names for the lookup table.
 * VARS holds variable values, and they share indices with VAR_KEYS
 * */
char* VAR_KEYS[128];
int VARS[128];
int VAR_IDX = 0;

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

int reformat_token_list(struct token** head){
    int idx = 0;
    struct token * iter = *head;
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
            if (tmp!=NULL){
            func->prev->next = func->next;
            }
            else{
                *head = func->next;
            }
            func->next->prev = tmp;
            iter->token_type = func->token_type;
            strcpy(iter->token_val, func->token_val);
        }
        else if(type==VAR){
            int i = 0;
            iter->token_type = INT;
            while(i<VAR_IDX){
                if(strcmp(iter->token_val,VAR_KEYS[i])==0){
                    sprintf(iter->token_val, "%d", VARS[i]);
                    break;
                }
                i++;
            }
            if(i==VAR_IDX){
                sprintf(iter->token_val, "%d", 0);
            }
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


 void calculate_opr(struct token *head, token_type type) {
    struct token *left_side = head->prev;
    struct token *right_side = head->next;
    int left_value;
    sscanf(left_side->token_val, "%d", &left_value);
    int right_value;
    sscanf(right_side->token_val, "%d", &right_value);
    int opr_result = 0;
    switch(type) {
        case MULTI:
        {
         opr_result = left_value * right_value;
         break;
        }
        case SUM:
        {
         opr_result = left_value + right_value;
         break;
        }
        case MINUS:
        {
         opr_result = left_value - right_value;
         break;
        }
        case B_AND:
        {
         opr_result = left_value & right_value;
         break;
        }
        case B_OR:
        {
         opr_result = left_value | right_value;
         break;
        }
        case B_XOR:
        {
         opr_result = left_value ^ right_value;
         break;
        }
        case LS:
        {
         opr_result = left_value << right_value;
         break;
        }
        case RS:
        {
         opr_result = left_value >> right_value;
         break;
        }
        case LR:
        {
         opr_result = (left_value << right_value) | (left_value >> (32 - right_value));
         break;
        }
        case RR:
        {
         opr_result = (left_value >> right_value) | (left_value << (32 - right_value));
         break;
        }

        default:
        {
            printf("Error! something wrong with calculate_opr()");
        }
    }

    char string_result[16];

    sprintf(string_result,"%d", opr_result);
    strcpy(left_side->token_val, string_result);

    left_side->next = right_side->next;
    right_side->next->prev = left_side;

}

 int calculate (struct token *head) {
    if(head->token_type == NOT) {
        head = head->next;
    }
    struct token *temp_head = head;

//     Loop for parentheses

    while(temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == OPEN_P) {
            int parenthesis_result = calculate(temp_head->next);
            char string_result[16];
            sprintf(string_result,"%d", parenthesis_result);
            temp_head->token_type = INT;
            strcpy(temp_head->token_val, string_result);
        }
        temp_head = temp_head->next;
    }


//     Loop for multiplication


    temp_head = head;
    while (temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == MULTI) {
            calculate_opr(temp_head, MULTI);
        }
        temp_head = temp_head->next;
    }

    temp_head = head;

    while (temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == SUM) {

            calculate_opr(temp_head, SUM);
            //printf("%s\n", temp_head->prev->token_val);
        }
        if (temp_head->token_type == MINUS) {
            calculate_opr(temp_head, MINUS);
        }
        temp_head = temp_head->next;
    }


//     Loop for binary operations
    temp_head = head;

   // printf("%s\n", temp_head->next->token_val);
    while (temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == B_AND) {
            calculate_opr(temp_head, B_AND);
        }
        if (temp_head->token_type == B_OR) {
            calculate_opr(temp_head, B_OR);
        }
        temp_head = temp_head->next;
    }


    // Loop for binary functions
    temp_head = head;

    while (temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == B_XOR) {
            calculate_opr(temp_head, B_XOR);
        }
        if (temp_head->token_type == LS) {
            calculate_opr(temp_head, LS);
        }
        if (temp_head->token_type == RS) {
            calculate_opr(temp_head, RS);
        }
        if (temp_head->token_type == LR) {
            calculate_opr(temp_head, LR);
        }
        if (temp_head->token_type == RR) {
            calculate_opr(temp_head, RR);
        }
        temp_head = temp_head->next;
    }




//     Detect close paranthesis

    if (head->next->token_type == CLOSE_P) {
        if (head->prev->prev != NULL && head->prev->prev->token_type == NOT) {
            int val;

            sscanf(head->token_val, "%d", &val);
            char string_result[16];
            sprintf(string_result,"%d", ~val);
            strcpy(head->token_val, string_result);
            if (head->prev->prev->prev == NULL) {
                head->prev->prev = NULL;
            }
            else {
                head->prev->prev->prev->next = head->prev;
                head->prev->prev = head->prev->prev->prev;
            }
        }
        head->prev->next = head->next->next;
        head->next->next->prev = head->prev;
    }



    int result;

    //sscanf((tokens->token_type == INT)? tokens->token_val : "0", "%d", &result);
    sscanf(head->token_val, "%d", &result);
    return result;

 }


 void print_debug(struct token * head){
     struct token * iter = head;
     while(iter->token_type!=EOL){
         printf("%s ",iter->token_val);
         iter= iter->next;
     }
 }

int main() {
    int error_code = 0;
    char line[256+1] = "";
    printf("%s ",">");
    while(fgets(line, sizeof(line), stdin)){
        error_code = 0;
        if(line == NULL){ // Stop when CTRL+D
            break;
        }
        char *p = line;
        struct token *head = NULL;
        struct token * tail = NULL;
        struct token *p_equal = NULL;
        error_code = lexer(p, strlen(p), &head, &tail,&p_equal);
        if(error_code==0) {
            if (p_equal != NULL) {
                error_code = assign_syntax_checker(head, p_equal);
                if(error_code==0){
                error_code = reformat_token_list(&p_equal);}
            } else {
                error_code = exp_syntax_checker(head);
                if(error_code==0){
                error_code = reformat_token_list(&head);}
            }
            if (error_code == 0) {
                if (head->token_type != EOL) {

                    if (p_equal != NULL) {
                   int res = calculate(p_equal->next);

                   char *var_name = p_equal->prev->token_val;

                   int declared = 0;
                   for(int i = 0; i < VAR_IDX; i++) {
                    if (strcmp(VAR_KEYS[i], var_name) == 0) {
                        VARS[i] = res;
                        declared = 1;
                        break;
                        }
                   }
                   if (declared == 0) {
                        VARS[VAR_IDX] = res;
                        VAR_KEYS[VAR_IDX] = var_name;
                        VAR_IDX++;
                   }

                }
                else {
                    int res = calculate(head);
                    printf("%d\n", res);
                }
                }
            }
            else{
                printf("Error!\n");
            }
        }
        else{
            printf("Error!\n");
        }

        printf("%s ",">");
    }
}
