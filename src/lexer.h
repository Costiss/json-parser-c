#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum {
  TOKEN_NULL,
  TOKEN_TRUE,
  TOKEN_FALSE,
  TOKEN_NUMBER,
  TOKEN_STRING,
  TOKEN_LBRACE,   // {
  TOKEN_RBRACE,   // }
  TOKEN_LBRACKET, // [
  TOKEN_RBRACKET, // ]
  TOKEN_COLON,    // :
  TOKEN_COMMA,    // ,
  TOKEN_EOF       // End of file
} TokenType;

typedef struct Token {
  TokenType type;
  char *start;
  size_t length;
} Token;

typedef struct Lexer {
  char *input;
  size_t position;
} Lexer;

Lexer *lexer_create(char *input);
void lexer_destroy(Lexer *lexer);
Token lexer_next_token(Lexer *lexer);
void assert_token(Token token, TokenType expected);

#endif
