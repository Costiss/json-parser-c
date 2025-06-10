#include "parser.h"
#include "lexer.h"
#include "linked_list.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JsonValue *json_value_create(JsonType type, const char *key,
                             const char *value) {
  JsonValue *json_value = (JsonValue *)malloc(sizeof(JsonValue));
  json_value->type = type;
  json_value->key = key ? strdup(key) : NULL;
  json_value->children = linked_list_create();

  switch (type) {
  case JSON_TYPE_STRING:
  case JSON_TYPE_NUMBER:
    json_value->value = value ? strdup(value) : NULL;
    break;
  default:
    json_value->value = NULL;
    break;
  }

  switch (type) {
  case JSON_TYPE_OBJECT:
  case JSON_TYPE_ARRAY:
    json_value->children = linked_list_create();
    break;
  default:
    json_value->children = NULL;
    break;
  }

  return json_value;
}

void json_value_destroy(JsonValue *json_value) {
  if (!json_value)
    return;

  if (json_value->key)
    free((char *)json_value->key);
  if (json_value->value)
    free((char *)json_value->value);

  if (json_value->children) {
    Node *node = json_value->children->head;
    while (node) {
      json_value_destroy((JsonValue *)node->data);
      node = node->next;
    }
    linked_list_destroy(json_value->children);
  }

  free(json_value);
}

JsonValue *parse_string(Lexer *lexer) {
  Token token = lexer_next_token(lexer);
  assert_token(token, TOKEN_STRING);
  return json_value_create(JSON_TYPE_STRING, NULL, token.start);
}

void parse_object(Lexer *lexer, JsonValue *parent) {
  Token token = lexer_next_token(lexer);
  assert_token(token, TOKEN_STRING);

  char *key = strndup(token.start, token.length);

  token = lexer_next_token(lexer);
  assert_token(token, TOKEN_COLON);

  token = lexer_next_token(lexer);
  if (token.type == TOKEN_STRING) {
    char *value = strndup(token.start, token.length);
    linked_list_append(parent->children,
                       json_value_create(JSON_TYPE_STRING, key, value));

  } else if (token.type == TOKEN_NUMBER) {
    char *value = strndup(token.start, token.length);
    linked_list_append(parent->children,
                       json_value_create(JSON_TYPE_NUMBER, key, value));

  } else if (token.type == TOKEN_TRUE) {
    linked_list_append(parent->children,
                       json_value_create(JSON_TYPE_TRUE, key, "true"));

  } else if (token.type == TOKEN_FALSE) {
    linked_list_append(parent->children,
                       json_value_create(JSON_TYPE_FALSE, key, "false"));

  } else if (token.type == TOKEN_NULL) {
    linked_list_append(parent->children,
                       json_value_create(JSON_TYPE_NULL, key, "null"));

  } else if (token.type == TOKEN_LBRACE) {
    JsonValue *child = json_value_create(JSON_TYPE_OBJECT, key, NULL);
    child->children = linked_list_create();
    linked_list_append(parent->children, child);
    parse_object(lexer, child);

  } else if (token.type == TOKEN_LBRACKET) {
    JsonValue *child = json_value_create(JSON_TYPE_ARRAY, key, NULL);
    child->children = linked_list_create();
    linked_list_append(parent->children, child);
    parse_array(lexer, child);
  }

  token = lexer_next_token(lexer);
  if (token.type == TOKEN_COMMA) {
    parse_object(lexer, parent);
  }
}

void parse_array(Lexer *lexer, JsonValue *parent) {
  printf("Parsing array...\n");

  while (1) {
    Token token = lexer_next_token(lexer);
    if (token.type == TOKEN_RBRACKET) {
      break;
    }

    if (token.type == TOKEN_STRING) {
      char *value = strndup(token.start, token.length);
      linked_list_append(parent->children,

                         json_value_create(JSON_TYPE_STRING, NULL, value));
    } else if (token.type == TOKEN_NUMBER) {
      char *value = strndup(token.start, token.length);
      linked_list_append(parent->children,
                         json_value_create(JSON_TYPE_NUMBER, NULL, value));

    } else if (token.type == TOKEN_TRUE) {
      linked_list_append(parent->children,
                         json_value_create(JSON_TYPE_TRUE, NULL, "true"));

    } else if (token.type == TOKEN_FALSE) {
      linked_list_append(parent->children,
                         json_value_create(JSON_TYPE_FALSE, NULL, "false"));

    } else if (token.type == TOKEN_NULL) {
      linked_list_append(parent->children,
                         json_value_create(JSON_TYPE_NULL, NULL, "null"));

    } else if (token.type == TOKEN_LBRACE) {
      JsonValue *child = json_value_create(JSON_TYPE_OBJECT, NULL, NULL);
      child->children = linked_list_create();
      linked_list_append(parent->children, child);
      parse_object(lexer, child);

    } else if (token.type == TOKEN_LBRACKET) {
      JsonValue *child = json_value_create(JSON_TYPE_ARRAY, NULL, NULL);
      child->children = linked_list_create();
      linked_list_append(parent->children, child);
      parse_array(lexer, child);

    } else if (token.type == TOKEN_COMMA) {
      continue;

    } else {
      printf("Unexpected token type: %d\n", token.type);
      exit(EXIT_FAILURE);
    }
  }
}

void print_json(JsonValue *root) {
  if (!root)
    return;

  switch (root->type) {
  case JSON_TYPE_OBJECT: {
    printf("{");
    Node *node = root->children->head;
    int first = 1;
    while (node) {
      JsonValue *child = (JsonValue *)node->data;
      if (!first)
        printf(", ");
      if (child->key) {
        printf("\"%s\": ", child->key);
      }
      print_json(child);
      first = 0;
      node = node->next;
    }
    printf("}");
    break;
  }
  case JSON_TYPE_ARRAY: {
    printf("[");
    Node *node = root->children->head;
    int first = 1;
    while (node) {
      JsonValue *child = (JsonValue *)node->data;
      if (!first)
        printf(", ");
      print_json(child);
      first = 0;
      node = node->next;
    }
    printf("]");
    break;
  }
  case JSON_TYPE_STRING:
    printf("\"%s\"", root->value ? root->value : "");
    break;
  case JSON_TYPE_NUMBER:
    printf("%s", root->value ? root->value : "0");
    break;
  case JSON_TYPE_TRUE:
    printf("true");
    break;
  case JSON_TYPE_FALSE:
    printf("false");
    break;
  case JSON_TYPE_NULL:
    printf("null");
    break;
  }
}
