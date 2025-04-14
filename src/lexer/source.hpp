
#ifndef TOY_LEXER_SOURCE_H
#define TOY_LEXER_SOURCE_H

#include <stdio.h>
#include <stack>

typedef struct sourcefile
{
  const char *name; /* Name of File*/
  const char *path; /* File Path */
  FILE *file;       /* File Pointer */
  size_t line;      /* Line Number where it is imported */
  YY_BUFFER_STATE buffer;
} sourcefile;

std::stack<sourcefile> sourceFiles;
std::vector<std::string> sourcePaths;

// void include_file(char *file_path);
void include_file(const char *filename, const char *filepath, FILE *file, size_t line)
{
  // FILE *file = fopen(file_path, "r");

  // if (file == NULL)
  // {
  //   // error
  //   return;
  // }

  YY_BUFFER_STATE buffer = yy_create_buffer(file, YY_BUF_SIZE);

  // sourcefile sourcefile = {get_name(file_path), get_path(file_path), file, YY_CURRENT_BUFFER};
  sourcefile sourcefile = {filename, filepath, file, line, YY_CURRENT_BUFFER};

  sourceFiles.push(sourcefile);

  yy_switch_to_buffer(buffer);
}

void pop_file()
{
  if (sourceFiles.empty())
  {
    // yyterminate();
    return;
  }

  fclose(sourceFiles.top().file);
  yy_delete_buffer(YY_CURRENT_BUFFER);

  sourceFiles.pop();

  yy_switch_to_buffer(sourceFiles.top().buffer);
}

void includee()
{
  /* got the include file name */

  std::string filename = yytext;
  size_t line = yylineno;
  std::size_t pos = filename.find(".toy");
  std::string filepath;

  FILE *file;

  if (pos == std::string::npos)
  {
    filename += ".toy";
  }

  for (auto libpath : sourcePaths)
  {
    file = fopen((libpath + filename).c_str(), "r");
    if (file)
    {
      filepath = libpath;
      break;
    }
  }

  if (!yyin)
  {
    printf("%s in %s line %d\n", (std::string("Failed to load import file ") + filename).c_str(), sourceFiles.top().name, yylineno);
  }
  else
  {
    include_file(filename.c_str(), filepath.c_str(), file, line);
    yylineno = yycolumn = 1;
    yylloc.first_line = yylloc.first_column = yylloc.last_line = yylloc.last_column = 1;
  }
}

#endif // TOY_LEXER_SOURCE_H
