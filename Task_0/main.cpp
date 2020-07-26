#include <iostream>

int swap_bytes_in_uint16_t(std::uint16_t arg);

int main() {
  std::uint16_t number = 1;                      // all bite from left to right
  std::cout << "arg -- " << number << std::endl; // show arg before change bytes
  number = swap_bytes_in_uint16_t(number); // call function to change bytes
  std::cout << "arg -- " << number << std::endl; // show arg after change bytes
  return 0;
}

int swap_bytes_in_uint16_t(std::uint16_t arg) {
  std::uint8_t first_byte(0), second_byte(0); // where I save bytes of uint16_t
  first_byte = arg;       // save first byte (look from left direction)
  second_byte = arg >> 8; // save second byte
  arg = ((std::uint16_t)first_byte << 8) | second_byte; // change bytes' place
  return arg;                                           // return new uint16_t
}
