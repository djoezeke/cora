#include <Cora/Driver/Driver.h>

#include <iostream>

int main(int argc, char **argv)
{
    return cora::driver::Driver{}.run(argc, argv,
                                      std::cout, std::cerr);
}
