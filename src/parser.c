#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// LinkedList
typedef struct Node {
  void *data;
  struct Node *next;
} Node;

typedef struct LinkedList {
  Node *head;
  Node *tail;
  size_t size;
} LinkedList;

LinkedList *linked_list_create() {
  LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  return list;
}

void linked_list_destroy(LinkedList *list) {
  Node *current = list->head;
  while (current) {
    Node *next = current->next;
    free(current);
    current = next;
  }
  free(list);
}

void linked_list_append(LinkedList *list, void *data) {
  Node *new_node = (Node *)malloc(sizeof(Node));
  new_node->data = data;
  new_node->next = NULL;

  if (list->tail) {
    list->tail->next = new_node;
  } else {
    list->head = new_node; // First element
  }
  list->tail = new_node;
  list->size++;
}

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
  char *start;   // Pointer to the start of the token in the input string
  size_t length; // Length of the token
} Token;

typedef struct Lexer {
  char *input;     // Input JSON string
  size_t position; // Current position in the input string
} Lexer;

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
    return token; // End of input
  }

  // Handle different token types
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
    // Handle string tokens
    size_t start_pos = ++lexer->position; // Skip the opening quote
    while (lexer->input[lexer->position] != '"' &&
           lexer->input[lexer->position] != '\0') {
      lexer->position++;
    }

    if (lexer->input[lexer->position] == '"') {
      token.type = TOKEN_STRING;
      token.start = &lexer->input[start_pos];
      token.length = lexer->position - start_pos;
      lexer->position++; // Skip the closing quote
    }
  } else if ((current_char >= '0' && current_char <= '9') ||
             current_char == '-') {
    // Handle number tokens
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
    // Invalid character, handle error
    // For simplicity, we will just skip it
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

// Object with N children
typedef struct JsonValue JsonValue;

typedef enum {
  JSON_TYPE_NULL,
  JSON_TYPE_TRUE,
  JSON_TYPE_FALSE,
  JSON_TYPE_NUMBER,
  JSON_TYPE_STRING,
  JSON_TYPE_OBJECT, // Represents a JSON object
  JSON_TYPE_ARRAY   // Represents a JSON array
} JsonType;

struct JsonValue {
  JsonType type;        // Type of the JSON value
  char *key;            // Key for the value (if applicable)
  char *value;          // Value as a string
  LinkedList *children; // List of child JsonValues
};

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

void parse_array(Lexer *lexer, JsonValue *parent);

JsonValue *parse_string(Lexer *lexer) {
  Token token = lexer_next_token(lexer);
  assert_token(token, TOKEN_STRING);
  return json_value_create(JSON_TYPE_STRING, NULL, token.start);
}

void parse_object(Lexer *lexer, JsonValue *parent) {
  Token token = lexer_next_token(lexer);
  assert_token(token, TOKEN_STRING);

  Token key_token = token;
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
    parse_array(lexer, child); // Implement array parsing if needed
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
      break; // End of array
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
      continue; // Skip commas
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

int main() {
  /* char *input = */
  /*     "{\"key\": \"value\", \"number\": 123, \"bool\": true, " */
  /*     "\"null_value\": null, \"nested\": {\"inner_key\": \"inner_value\"}, "
   */
  /*     "\"array\": [1, 2, 3, {\"array_object_key\":
   * \"array_object_value\"}]}"; */
  char *input =
      "{\"key\": \"value\", \"number\": 123, \"bool\": true,\"null_value\": "
      "null, \"nested\": {\"inner_key\": \"inner_value\"}, \"array\": [1, 2, "
      "3] ";
  Lexer *lexer = lexer_create(input);
  // Print every token for debugging
  /* Token token; */
  /* while ((token = lexer_next_token(lexer)).type != TOKEN_EOF) { */
  /*   printf("Token: Type=%d, Value='%.*s'\n", token.type, (int)token.length,
   */
  /*          token.start); */
  /* } */

  JsonValue *root = json_value_create(JSON_TYPE_OBJECT, NULL, NULL);
  lexer_next_token(lexer); // Initialize the lexer
  parse_object(lexer, root);

  printf("Children count: %zu\n", root->children->size);
  JsonValue *first_child = (JsonValue *)root->children->head->data;

  print_json(root);

  return 0;
}
