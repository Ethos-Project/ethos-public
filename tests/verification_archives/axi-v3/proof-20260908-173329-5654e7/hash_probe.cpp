#include <iostream>
#include <iterator>
#include "sha3.hpp"
int main(){std::string s((std::istreambuf_iterator<char>(std::cin)),{});std::cout<<Axi::crypto::SHA3_256::hash(s);}
