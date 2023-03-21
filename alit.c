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

int assign_syntax_checker(struct token* tokens, char * p_equal, int length){
    struct token *token = tokens;
}

int exp_syntax_checker(struct token* tokens, int length){

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


 /*

CALCULATE FUNCTION AND HELPERS

*/
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
        default:
        {
            printf("Error! something wrong with calculate_opr()");
        }
    }

    char string_result[16];

    sprintf(string_result,"%d", opr_result);
    //printf("%s\n", string_result);
    strcpy(left_side->token_val, string_result);

    left_side->next = right_side->next;
    right_side->next->prev = left_side;

}

 int calculate (struct token *head) {
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

//    token *open_p;
//    int j;
//    for(j = 0; tokens[j].token_type != CLOSE_P &&  tokens[j].token_type != EOL; j++) {
//        if (tokens[j].token_type == OPEN_P) {
//            open_p = tokens + j;
//            int parenthesis_result = calculate(tokens + j + 1);
//            char string_result[16];
//            sprintf(string_result,"%d", parenthesis_result);
//            open_p->token_type = INT;
//            strcpy(open_p->token_val, string_result);
//            (open_p + 1)->token_type = PRST;
//
//        }
//    }

//     Loop for multiplication


    temp_head = head;
    while (temp_head->token_type != CLOSE_P &&  temp_head->token_type != EOL) {
        if (temp_head->token_type == MULTI) {
            calculate_opr(temp_head, MULTI);
        }
        temp_head = temp_head->next;
    }
//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == MULTI) {
//            calculate_opr(tokens, MULTI, i);
//        }
//    }
//     Loop for addition and substraction
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


//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == SUM) {
//            calculate_opr(tokens, SUM, i);
//        }
//        if (tokens[i].token_type == MINUS) {
//            calculate_opr(tokens, MINUS, i);
//        }
//
//    }
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

//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == B_AND) {
//            calculate_opr(tokens, B_AND, i);
//        }
//        if (tokens[i].token_type == B_OR) {
//            calculate_opr(tokens, B_OR, i);
//
//        }
//    }
//     Detect close paranthesis

    if (head->next->token_type == CLOSE_P) {
        head->prev->next = head->next->next;
        head->next->next->prev = head->prev;
    }

//    token *close_p;
//    if (tokens[j].token_type == CLOSE_P) {
//        close_p = tokens + j;
//        close_p->token_type = PRST;
//    }

    int result;

    //sscanf((tokens->token_type == INT)? tokens->token_val : "0", "%d", &result);
    sscanf(head->token_val, "%d", &result);
    return result;

 }

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
        struct token *equal_p = NULL;
        if(lexer(p, strlen(p), &head, &tail,&equal_p) != -1){
            //struct token* iter = head;
            int res = calculate(head);
            printf("%d\n", res);
//            int a = 0;
//            while(iter->token_type!=EOL){
//                printf("%d:     ",a);
//                printf("%s\n",iter->token_val);
//                a++;
//                iter = iter->next;
//            }
//            iter = tail;
//            a = 0;
//            while(iter->token_type!=VAR){
//                printf("%d:     ",a);
//                printf("%s\n",iter->token_val);
//                a++;
//                iter = iter->prev;
//            }
        }
        else{
            printf("Error!\n");
        }
        printf("%s ",">");
    }
}




//
//
//
//
//
//
//
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <ctype.h>
//
//
//typedef enum{
//    VAR,
//    INT,
//    OPEN_P,
//    CLOSE_P,
//    SUM,
//    MULTI,
//    MINUS,
//    EQUAL,
//    B_AND,
//    B_OR,
//    B_XOR,
//    LS,
//    RS,
//    LR,
//    RR,
//    NOT,
//    COMMA,
//    COMMENT,
//    EOL,
//    PRST
//}token_type;
//
//typedef struct{
//    token_type token_type;
//    char token_val[256];
//
//}token;
//
//char* KEYWORDS[] = {"xor","ls","rs","lr","rr","not"};
//char SIGNS[] ={'=','+','-','*','&','|','(',')',',','%'};
//
//int is_sign(char chr){
//    for(int i = 0; i< 10;i++){ //There are 10 signs if there is more or less update the number here
//        if(chr == SIGNS[i]){
//            return 1;
//        }
//    }
//    return 0;
//}
//
//int is_keyword(char* word){
//    for(int i = 0; i <6; i++){ //There are 6 keywords if there is more or less update the number here
//        if(strcmp(word, KEYWORDS[i])==0){
//            return 1;
//        }
//    }
//    return 0;
//}
//
///*
// * These functions will receive an iterator pointer when their corresponding type first encountered.
// * They will read from pointer until an unrelevant char was encountered and then they will return the token or throw error.
// * Unrelevance should be defined for each type since it differs which type can follow which type.
// * Pointer's current position must be updated in these functions since it will be used in the loop.
// * */
//
///*
// * Returns token of type function or variable
// * */
//token func_and_var_parser(char ** exp){
//    token token;
//    char str[256+1] = "";
//    int idx = 0;
//    while(isalpha(**exp)){
//        str[idx] = **exp;
//        idx++;
//        (*exp)++;
//    }
//    str[idx] = '\0';
//    strcpy(token.token_val,str);
//    if(is_keyword(str)==1){
//        if(strcmp(str, "xor")==0){
//            token.token_type = B_XOR;
//        }
//        else if(strcmp(str, "ls")==0){
//            token.token_type = LS;
//        }
//        else if(strcmp(str, "rs")==0){
//            token.token_type = RS;
//        }
//        else if(strcmp(str, "lr")==0){
//            token.token_type = LR;
//        }
//        else if(strcmp(str, "rr")==0){
//            token.token_type = RR;
//        }
//        else if(strcmp(str, "not")==0){
//            token.token_type = NOT;
//        }
//    }
//    else{
//        token.token_type = VAR;
//    }
//    return token;
//}
///*
// * Returns token of type integer
// * */
//token int_parser(char** exp){
//    token token;
//    char num[256+1] = "";
//    int idx = 0;
//    while(isdigit(**exp)){
//        num[idx] = **exp;
//        idx++;
//        (*exp)++;
//    }
//    num[idx] = '\0';
//    strcpy(token.token_val,num);
//    token.token_type = INT;
//    return token;
//}
///*
// * Returns token of type operator, parenthesis, comma or equal sign
// * */
//token sign_parser(char **exp){
//    token token;
//    if(**exp=='='){
//        token.token_type = EQUAL;
//    }
//    else if(**exp=='+'){
//        token.token_type = SUM;
//    }
//    else if(**exp=='-'){
//        token.token_type = MINUS;
//    }
//    else if(**exp=='*'){
//        token.token_type = MULTI;
//    }
//    else if(**exp=='&'){
//        token.token_type = B_AND;
//    }
//    else if(**exp=='|'){
//        token.token_type = B_OR;
//    }
//    else if(**exp=='('){
//        token.token_type = OPEN_P;
//    }
//    else if(**exp==')'){
//        token.token_type = CLOSE_P;
//    }
//    else if(**exp==','){
//        token.token_type = COMMA;
//    }
//    else if(**exp=='%'){
//        token.token_type = COMMENT;
//    }
//    strcpy(token.token_val, (char[2]){**exp,'\0'});
//    (*exp)++;
//    return token;
//}
//
//token eol_parser(){
//    token token;
//    token.token_type = EOL;
//    strcpy(token.token_val,"<EOL>");
//    return token;
//}
///*
// * Returns token of type variable
// * */
//token lhs_parser(char* exp){
//
//}
//
///*
// * Return 0 on success
// * Return -1 on error
// * */
//int lexer(char *p, int length, token* tokens, char* p_equal){ // Initialize p_equal to NULL
//    int idx = 0;
//    p_equal = NULL; //for safety
//    token token;
//    for(int i = 0; i<length; i++){
//        if(*p=='\n'){
//            token = eol_parser();
//            tokens[idx] = token;
//            break;
//        }
//        else if(isspace(*p)){
//            p += 1;
//            continue;
//        }
//        else if(isalpha(*p)){
//            token = func_and_var_parser(&p);
//        }
//        else if(isdigit(*p)){
//            token = int_parser(&p);
//        }
//        else if(is_sign(*p)){
//            if(*p=='='){
//                if(p_equal==NULL){
//                    p_equal = p;
//                }
//                else{
//                    return -1;
//                }
//            }
//
//            token = sign_parser(&p);
//
//            if(token.token_type==COMMENT){
//                token = eol_parser();
//                tokens[idx] = token;
//                break;
//            }
//        }
//        else{
//            return -1;
//        }
//        tokens[idx] = token;
//        idx++;
//    }
//    return 0;
//}
//
///*
//
//CALCULATE FUNCTION AND HELPERS
//
//*/
//
//token *find_left_side(token *tokens) {
//    token *left_valid_token = tokens - 1;
//    while (left_valid_token->token_type != INT) {
//        left_valid_token--;
//    };
//    return left_valid_token;
//}
//token *find_right_side(token *tokens) {
//    token *right_valid_token = tokens + 1;
//    while (right_valid_token->token_type != INT) {
//        right_valid_token++;
//    };
//    return right_valid_token;
//}
//
//void calculate_opr(token *tokens, token_type type, int i) {
//    token *left_side = find_left_side(tokens + i);
//    token *right_side = find_right_side(tokens + i);
//    int left_value;
//    sscanf(left_side->token_val, "%d", &left_value);
//    int right_value;
//    sscanf(right_side->token_val, "%d", &right_value);
//    int opr_result = 0;
//    switch(type) {
//        case MULTI:
//        {
//         opr_result = left_value * right_value;
//         break;
//        }
//        case SUM:
//        {
//         opr_result = left_value + right_value;
//         break;
//        }
//        case MINUS:
//        {
//         opr_result = left_value - right_value;
//         break;
//        }
//        case B_AND:
//        {
//         opr_result = left_value & right_value;
//         break;
//        }
//        case B_OR:
//        {
//         opr_result = left_value | right_value;
//         break;
//        }
//        default:
//        {
//            printf("Error! something wrong with calculate_opr()");
//        }
//    }
//
//    char string_result[16];
//    sprintf(string_result,"%d", opr_result);
//    strcpy(left_side->token_val, string_result);
//    tokens[i].token_type = PRST;
//    right_side->token_type = PRST;
//}
//
// int calculate (token *tokens) {
////     Loop for parentheses
//    token *open_p;
//    int j;
//    for(j = 0; tokens[j].token_type != CLOSE_P &&  tokens[j].token_type != EOL; j++) {
//        if (tokens[j].token_type == OPEN_P) {
//            open_p = tokens + j;
//            int parenthesis_result = calculate(tokens + j + 1);
//            char string_result[16];
//            sprintf(string_result,"%d", parenthesis_result);
//            open_p->token_type = INT;
//            strcpy(open_p->token_val, string_result);
//            (open_p + 1)->token_type = PRST;
//
//        }
//    }
//
////     Loop for multiplication
//
//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == MULTI) {
//            calculate_opr(tokens, MULTI, i);
//        }
//    }
////     Loop for addition and substraction
//
//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == SUM) {
//            calculate_opr(tokens, SUM, i);
//        }
//        if (tokens[i].token_type == MINUS) {
//            calculate_opr(tokens, MINUS, i);
//        }
//
//    }
////     Loop for binary operations
//
//    for(int i = 0; tokens[i].token_type != CLOSE_P &&  tokens[i].token_type != EOL; i++) {
//        if (tokens[i].token_type == B_AND) {
//            calculate_opr(tokens, B_AND, i);
//        }
//        if (tokens[i].token_type == B_OR) {
//            calculate_opr(tokens, B_OR, i);
//
//        }
//    }
////     Detect close paranthesis
//    token *close_p;
//    if (tokens[j].token_type == CLOSE_P) {
//        close_p = tokens + j;
//        close_p->token_type = PRST;
//    }
//    int result;
//
//    //sscanf((tokens->token_type == INT)? tokens->token_val : "0", "%d", &result);
//    sscanf(tokens->token_val, "%d", &result);
//    return result;
//
//
// }
//
//int main() {
//    char** vars = calloc(128 ,sizeof(char*));
//    int last_var_idx = 0;
//    char line[256+1] = "";
//    printf("%s ",">");
//    while(fgets(line, sizeof(line), stdin)){
//        //printf("%s\n",line);
//        if(line == NULL){
//            break;
//        }
//        char *p = line;
//        token tokens[256];
//        char *equal_p = NULL;
//        if(lexer(p, strlen(p),tokens,equal_p)!=-1){
//            int result = calculate(tokens);
//            printf("%d\n", result);
////            token* token = tokens;
////            int a = 0;
////            while((*token).token_type!=EOL){
////                printf("%d:     ",a);
////                printf("%s --> %d\n",(*token).token_val, (*token).token_type);
////                a++;
////                token += 1;
////            }
//        }
//        else{
//            printf("Error!\n");
//        }
//        printf("%s ",">");
//    }
//}
//











//#include <stdio.h>
//#include <ctype.h>
//#include <string.h>
//#include <stdlib.h>
//
//// Grouping integral constants
//typedef enum {
//    TOKEN_TYPE_NUMBER,
//    TOKEN_TYPE_OPERATOR,
//    TOKEN_TYPE_FUNCTION,
//    TOKEN_TYPE_PUNCTUATION
//} TokenType;
//
//// Grouping variables
//typedef struct {
//    TokenType type;
//    char value[64];
//} Token;
//
//Token parse_number(char *number) {
//    Token token;
//    token.type = TOKEN_TYPE_NUMBER;
//    int j = 0;
//    while (isdigit(number[j])) {
//        token.value[j] = number[j];
//        j++;
//    }
//    token.value[j] = '\0';
//
//    return token;
//}
//
//
//Token parse_alpha(char *word) {
//    Token token;
//    //! Check whether upcoming string is a two-letter length reserved keyword
//    char biliteral[3];
//    biliteral[0] = word[0];
//    biliteral[1] = word[1];
//    biliteral[2] = '\0';
//
//    if (strcmp(biliteral, "ls") == 0 || strcmp(biliteral, "rs") == 0 || strcmp(biliteral, "lr") == 0 || strcmp(biliteral, "rr")== 0 ) {
//        int i = 2;
//        while (i < strlen(word)) {
//            if (isspace(word[i])) {
//                i++;
//                continue;
//            }
//            else if (word[i] == '(') {
//                token.type = TOKEN_TYPE_FUNCTION;
//                strcpy(token.value, biliteral);
//                break;
//            }
//            else {
//                break;
//            }
//        }
//    }
//    if (strlen(token.value) != 0) {
//        return token;
//    }
//
//    //! Check whether upcoming string is a three-letter length reserved keyword
//    char triliteral[4];
//    triliteral[0] = word[0];
//    triliteral[1] = word[1];
//    triliteral[2] = word[2];
//    triliteral[3] = '\0';
//
//    if (strcmp(triliteral, "xor") == 0 || strcmp(triliteral, "not") == 0) {
//        int i = 3;
//        while (i < strlen(word)) {
//            if (isspace(word[i])) {
//                i++;
//                continue;
//            }
//            else if (word[i] == '(') {
//                token.type = TOKEN_TYPE_FUNCTION;
//                strcpy(token.value, triliteral);
//                break;
//            }
//            else {
//                break;
//            }
//        }
//    }
//    if (strlen(token.value) != 0) {
//        return token;
//    }
//
//}
//
//Token *get_tokens(char *input) {
//    Token *tokens = (Token *) calloc(256, sizeof(Token));
//    int token_location = 0;
//    int i = 0;
//    while (i < strlen(input)) {
//        char curr_char = input[i];
//        //! If white space, advance
//        if (isspace(curr_char)) {
//            i++;
//            continue;
//        }
//        //! If Number, handle
//        else if(isdigit(curr_char)) {
//            Token number_token = parse_number(input + i);
//            tokens[token_location] = number_token;
//            token_location++;
//            i += strlen(number_token.value);
//            continue;
//        }
//        //! If Operator, handle
//        else if (strchr("*/+-()&|", curr_char)){
//            Token operator_token;
//            operator_token.type = TOKEN_TYPE_OPERATOR;
//            operator_token.value[0] = curr_char;
//            operator_token.value[1] = '\0';
//            tokens[token_location] = operator_token;
//            token_location++;
//            i++;
//            continue;
//        }
//        //! If Letter, handle
//        else if (isalpha(curr_char)) {
//
//            Token alpha_token = parse_alpha(input + i);
//            tokens[token_location] = alpha_token;
//            token_location++;
//            i += strlen(alpha_token.value);
//            continue;
//
//        }
//        else if (curr_char == ',') {
//            Token punctuation_token;
//            punctuation_token.type = TOKEN_TYPE_PUNCTUATION;
//            punctuation_token.value[0] = curr_char;
//            punctuation_token.value[1] = '\0';
//            tokens[token_location] = punctuation_token;
//            token_location++;
//            i++;
//            continue;
//        }
//        //! If not above, throw error
//        else {
//            printf("error!");
//            break;
//        }
//
//    }
//    return tokens;
//
//}
//
//int main()
//{
//	char line[256] = "(rs (8, 1) & (not(8) | ls(6, 2))) + not(2)";
//
//	Token *tokens = get_tokens(line);
//	for (int i = 0; i < 20; i++) {
//       printf("%s", tokens[i].value);
//	}
//	return 0;
//}
