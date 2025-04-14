#ifndef TOY_LEXER_LOCATION_H
#define TOY_LEXER_LOCATION_H

#include <stdint.h>
#include <stddef.h>

typedef struct Location
{
    // char *file_path;
    size_t start_line;
    size_t end_line;
    size_t start_column;
    size_t end_column;

} YYLTYPE, Location;

///////////////////////////////////////////////////////////////////////////////
/// \brief Create a Location Object
///
/// \param startline Location Start Line
/// \param endline Location End Line
/// \param startcolon Location Start Column
/// \param endcolon Location End Column
//
/// \return Location Object
///////////////////////////////////////////////////////////////////////////////
Location location(size_t startline, size_t endline, size_t startcolumn, size_t endcolumn)
{
    Location loc;
    loc.start_line = startline;
    loc.end_line = endline;
    loc.start_column = startcolumn;
    loc.end_column = endcolumn;

    return loc;
};

#endif // TOY_LEXER_LOCATION_H
