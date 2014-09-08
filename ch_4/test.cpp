#include <iostream>

int main(int argc, char** argv)
{
auto adder = [](int x) 
{
 		return [=](int y) 
          { 
          	return x + y; 
          };
    };

    auto addthree = adder(3);
    std::cout << addthree(4) << std::endl;

    return 0;
}
