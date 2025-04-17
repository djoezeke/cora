#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cassert>
#include <stack>
#include <vector>
#include <string>

#include "GetOpt.h"

#include "config.hpp"

extern int yylex();
extern int yylex_destroy();
extern FILE *yyin;
extern char *yytext;

void usage();

int main(int argc, char **argv)
{
    std::string filename;
    if (argc == 1)
    {
        filename = "./test.toy";
    }

    bool verbose = false;
    bool quiet = false;
    bool debug = false;
    int opt = getopt(argc, argv, "hi:vqd");

    if (opt == -1)
    {
        std::cout << "No options provided. Using default file: " << filename << "\n";
    }
    else
    {
        if (opt == '?')
        {
            std::cout << opterr << "\n";
            usage();
            return 1;
        }
    }

    while (true)
    {
        int c = getopt(argc, argv, "hi:vqd");

        switch (c)
        {
        case 'i':
            filename = optarg;
            break;
        case 'v':
            verbose = true;
            break;
        case 'q':
            quiet = true;
            break;
        case 'd':
            debug = true;
            break;
        case 'h':
            verbose = true;
            usage();
            return 1;
        case '?':
            std::cout << opterr << "\n";
            usage();
            return 1;
        case -1:
            std::cout << "No options provided. Using default file: " << filename << "\n";
            break;
        default:
            break;
        }
    }

    if (!quiet)
    {
        std::cout << "toylex version: " << TOY_MAJOR_VER << "." << TOY_MINOR_VER << "." << TOY_REVISION_VER << "\n";
    }

    if (verbose)
    {
        std::cout << "Verbose mode enabled.\n";
        std::cout << "Debug mode: " << (debug ? "enabled" : "disabled") << "\n";
        std::cout << "Quiet mode: " << (quiet ? "enabled" : "disabled") << "\n";
    }

    if (debug)
    {
        std::cout << "Debug mode enabled.\n";
    }

    if (quiet)
    {
        std::cout << "Quiet mode enabled.\n";
    }

    if (filename.empty())
    {
        std::cout << "No file name provided. Using default file: " << filename << "\n";
        filename = "./test.toy";
    }

    yyin = fopen(filename.c_str(), "r+");

    if (yyin == nullptr)
    {
        std::cout << "File " << filename << "not found. Abort" << std::endl;
        return -1;
    }

    int kind;

    while ((kind = yylex()) != 0)
    {
        printf("%d:%s\n", kind, yytext);
    }

    if (yyin != nullptr)
        fclose(yyin);
    yylex_destroy();
    return 0;
}

void usage()
{
    std::cout << "Usage:\n";
    std::cout << "toylex filename -h -d -v -q path1\n";
    std::cout << "\t-h this help text.\n";
    std::cout << "\t-d debug code generation. Disables the code optimizer pass.\n";
    std::cout << "\t-v be more verbose.\n";
    std::cout << "\t-q be quiet.\n";
}
