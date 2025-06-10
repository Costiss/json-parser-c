#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "linked_list.h"
#include <stddef.h>

typedef struct JsonValue JsonValue;

typedef enum {
  JSON_TYPE_NULL,
  JSON_TYPE_TRUE,
  JSON_TYPE_FALSE,
  JSON_TYPE_NUMBER,
  JSON_TYPE_STRING,
  JSON_TYPE_OBJECT,
  JSON_TYPE_ARRAY
} JsonType;

struct JsonValue {
  JsonType type;
  char *key;
  char *value;
  LinkedList *children;
};

JsonValue *json_value_create(JsonType type, const char *key, const char *value);
JsonValue *parse_string(Lexer *lexer);
void parse_object(Lexer *lexer, JsonValue *parent);
void parse_array(Lexer *lexer, JsonValue *parent);
void print_json(JsonValue *root);
void json_value_destroy(JsonValue *json_value);

#endif
