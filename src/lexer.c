#include "lexer.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Lexer *lexer_create(char *input) {
  Lexer *lexer = (Lexer *)malloc(sizeof(Lexer));
  lexer->input = input;
  lexer->position = 0;
  return lexer;
}

void lexer_destroy(Lexer *lexer) { free(lexer); }

Token lexer_next_token(Lexer *lexer) {
  Token token = {TOKEN_EOF, NULL, 0};

  while (lexer->input[lexer->position] == ' ' ||
         lexer->input[lexer->position] == '\n' ||
         lexer->input[lexer->position] == '\t' ||
         lexer->input[lexer->position] == '\r') {
    lexer->position++;
  }

  char current_char = lexer->input[lexer->position];

  if (current_char == '\0') {
    return token;
  }

  if (current_char == '{') {
    token.type = TOKEN_LBRACE;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == '}') {
    token.type = TOKEN_RBRACE;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == '[') {
    token.type = TOKEN_LBRACKET;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == ']') {
    token.type = TOKEN_RBRACKET;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == ':') {
    token.type = TOKEN_COLON;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == ',') {
    token.type = TOKEN_COMMA;
    token.start = &lexer->input[lexer->position++];
    token.length = 1;

  } else if (current_char == '"') {
    size_t start_pos = ++lexer->position;
    while (lexer->input[lexer->position] != '"' &&
           lexer->input[lexer->position] != '\0') {
      lexer->position++;
    }

    if (lexer->input[lexer->position] == '"') {
      token.type = TOKEN_STRING;
      token.start = &lexer->input[start_pos];
      token.length = lexer->position - start_pos;
      lexer->position++;
    }

  } else if ((current_char >= '0' && current_char <= '9') ||
             current_char == '-') {
    size_t start_pos = lexer->position;
    while ((lexer->input[lexer->position] >= '0' &&
            lexer->input[lexer->position] <= '9') ||
           lexer->input[lexer->position] == '.' ||
           lexer->input[lexer->position] == 'e' ||
           lexer->input[lexer->position] == 'E' ||
           lexer->input[lexer->position] == '+' ||
           lexer->input[lexer->position] == '-') {
      lexer->position++;
    }

    token.type = TOKEN_NUMBER;
    token.start = &lexer->input[start_pos];
    token.length = lexer->position - start_pos;

  } else if (strncmp(&lexer->input[lexer->position], "null", 4) == 0) {
    token.type = TOKEN_NULL;
    token.start = &lexer->input[lexer->position];
    token.length = 4;
    lexer->position += 4;

  } else if (strncmp(&lexer->input[lexer->position], "true", 4) == 0) {
    token.type = TOKEN_TRUE;
    token.start = &lexer->input[lexer->position];
    token.length = 4;
    lexer->position += 4;

  } else if (strncmp(&lexer->input[lexer->position], "false", 5) == 0) {
    token.type = TOKEN_FALSE;
    token.start = &lexer->input[lexer->position];
    token.length = 5;
    lexer->position += 5;

  } else {
    // Invalid character
    lexer->position++;
  }

  return token;
}

void assert_token(Token token, TokenType expected) {
  if (token.type != expected) {
    printf("Expected token type %d, but got %d\n", expected, token.type);
    printf("Token value: '%.*s'\n", (int)token.length, token.start);
    exit(EXIT_FAILURE);
  }
}
