
#ifndef TOY_LEXER_SOURCE_H
#define TOY_LEXER_SOURCE_H

#include <stdio.h>
#include <stack>

// extern char *yytext;

typedef struct sourcefile
{
  char *name;
  char *path;
  FILE *file;
  YY_BUFFER_STATE buffer;
} sourcefile;

std::stack<sourcefile> sourceFiles;

char *get_path(char *);
char *get_name(char *);

void include_file(char *file_path)
{
  FILE *file = fopen(file_path, "r");

  if (file == NULL)
  {
    // error
    return;
  }

  YY_BUFFER_STATE buffer = yy_create_buffer(file, YY_BUF_SIZE);

  sourcefile sourcefile = {get_name(file_path), get_path(file_path), file, YY_CURRENT_BUFFER};
  sourceFiles.push(sourcefile);

  yy_switch_to_buffer(buffer);
}

void pop_file()
{
  if (sourceFiles.empty())
  {
    yyterminate();
  }

  fclose(sourceFiles.top().file);
  yy_delete_buffer(YY_CURRENT_BUFFER);

  sourceFiles.pop();

  yy_switch_to_buffer(sourceFiles.top().buffer);
}

#endif // TOY_LEXER_SOURCE_H
