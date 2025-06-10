#include "parser.h"
#include <stdio.h>
#include <stdlib.h>

char *read_file_into_buffer(const char *filename) {
  FILE *file = fopen(filename, "rb");
  if (!file) {
    perror("Error opening file");
    return NULL;
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Error seeking file");
    fclose(file);
    return NULL;
  }

  long file_size = ftell(file);
  if (file_size == -1) {
    perror("Error getting file size");
    fclose(file);
    return NULL;
  }

  rewind(file);

  char *buffer = (char *)malloc(file_size + 1);
  if (!buffer) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t bytes_read = fread(buffer, 1, file_size, file);
  if (bytes_read != (size_t)file_size) {
    perror("Error reading file");
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[file_size] = '\0';

  fclose(file);
  return buffer;
}

int main() {
  char *input = read_file_into_buffer("input.json");

  Lexer *lexer = lexer_create(input);

  JsonValue *root = json_value_create(JSON_TYPE_OBJECT, NULL, NULL);
  lexer_next_token(lexer);

  parse_object(lexer, root);
  print_json(root);

  json_value_destroy(root);
  lexer_destroy(lexer);

  return 0;
}
