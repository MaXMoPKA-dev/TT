#include <iostream>
typedef unsigned int uint_t;

int degree_of_two(uint_t deg);

int main() {
  uint_t degree = 31; // our degree no more than 31, cause maximum use uint32_t
  uint_t number;      // our number
  number = degree_of_two(degree); // call function to find degree of two
  std::cout << "degree: " << degree << std::endl  // show degree
            << "number: " << number << std::endl; // show result
  return 0;
}
int degree_of_two(uint_t deg) { // function for found degree of two
  uint_t t = 1;                 // binary cod of t is 0 0 .. 1
  t <<= deg; // shift 1 in binary cod of t in left on number of degree
  return t;
}
