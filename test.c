#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Grouping integral constants
typedef enum {
    TOKEN_TYPE_NUMBER,
    TOKEN_TYPE_OPERATOR,
    TOKEN_TYPE_FUNCTION,
    TOKEN_TYPE_PUNCTUATION
} TokenType;

// Grouping variables
typedef struct {
    TokenType type;
    char value[64];
} Token;

Token parse_number(char *number) {
    Token token;
    token.type = TOKEN_TYPE_NUMBER;
    int j = 0;
    while (isdigit(number[j])) {
        token.value[j] = number[j];
        j++;
    }
    token.value[j] = '\0';

    return token;
}


Token parse_alpha(char *word) {
    Token token;
    //! Check whether upcoming string is a two-letter length reserved keyword
    char biliteral[3];
    biliteral[0] = word[0];
    biliteral[1] = word[1];
    biliteral[2] = '\0';

    if (strcmp(biliteral, "ls") == 0 || strcmp(biliteral, "rs") == 0 || strcmp(biliteral, "lr") == 0 || strcmp(biliteral, "rr")== 0 ) {
        int i = 2;
        while (i < strlen(word)) {
            if (isspace(word[i])) {
                i++;
                continue;
            }
            else if (word[i] == '(') {
                token.type = TOKEN_TYPE_FUNCTION;
                strcpy(token.value, biliteral);
                break;
            }
            else {
                break;
            }
        }
    }
    if (strlen(token.value) != 0) {
        return token;
    }

    //! Check whether upcoming string is a three-letter length reserved keyword
    char triliteral[4];
    triliteral[0] = word[0];
    triliteral[1] = word[1];
    triliteral[2] = word[2];
    triliteral[3] = '\0';

    if (strcmp(triliteral, "xor") == 0 || strcmp(triliteral, "not") == 0) {
        int i = 3;
        while (i < strlen(word)) {
            if (isspace(word[i])) {
                i++;
                continue;
            }
            else if (word[i] == '(') {
                token.type = TOKEN_TYPE_FUNCTION;
                strcpy(token.value, triliteral);
                break;
            }
            else {
                break;
            }
        }
    }
    if (strlen(token.value) != 0) {
        return token;
    }

}

Token *get_tokens(char *input) {
    Token *tokens = (Token *) calloc(256, sizeof(Token));
    int token_location = 0;
    int i = 0;
    while (i < strlen(input)) {
        char curr_char = input[i];
        //! If white space, advance
        if (isspace(curr_char)) {
            i++;
            continue;
        }
        //! If Number, handle
        else if(isdigit(curr_char)) {
            Token number_token = parse_number(input + i);
            tokens[token_location] = number_token;
            token_location++;
            i += strlen(number_token.value);
            continue;
        }
        //! If Operator, handle
        else if (strchr("*/+-()&|", curr_char)){
            Token operator_token;
            operator_token.type = TOKEN_TYPE_OPERATOR;
            operator_token.value[0] = curr_char;
            operator_token.value[1] = '\0';
            tokens[token_location] = operator_token;
            token_location++;
            i++;
            continue;
        }
        //! If Letter, handle
        else if (isalpha(curr_char)) {

            Token alpha_token = parse_alpha(input + i);
            tokens[token_location] = alpha_token;
            token_location++;
            i += strlen(alpha_token.value);
            continue;

        }
        else if (curr_char == ',') {
            Token punctuation_token;
            punctuation_token.type = TOKEN_TYPE_PUNCTUATION;
            punctuation_token.value[0] = curr_char;
            punctuation_token.value[1] = '\0';
            tokens[token_location] = punctuation_token;
            token_location++;
            i++;
            continue;
        }
        //! If not above, throw error
        else {
            printf("error!");
            break;
        }

    }
    return tokens;

}

int main()
{
	char line[100] = "(rs(8, 1) & (not(8) | ls(6, 2))) + not(2)";

	Token *tokens = get_tokens(line);
	for (int i = 0; i < 20; i++) {
       printf("%s , ", tokens[i].value);
	}
	return 0;
}
