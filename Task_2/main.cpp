#include <iostream>
/* function to find broadcast address
 *--------------------------------------------------------------------------*/
void finding_a_broadcast_adress(std::uint8_t *ip_, std::uint8_t *subnet_,
                                std::uint8_t *broadcast);

/* functions to validat subnet mask, IP-address, broadcast address
 *--------------------------------------------------------------------------*/
void data_validation_of_subnet(std::uint8_t *data);

void data_validation_of_ip(std::uint8_t *data);

void data_validation_of_broadcast(std::uint8_t *data);

/*function to show information about subnet mask, IP-address, broadcast
 * address
 *--------------------------------------------------------------------------*/
void cout_of_inform(std::uint8_t *inf) {
  for (int i = 0; i < 4; i++) {
    std::cout
        << inf[i] + 0
        << "."; // +0 beacouse in std::cout std::uint8_t look like unsigned char
  }
}

int main() {
  /* I use massive of uint8_t because format of subnet mask, IP-address,
   *broadcast address is XXX.YYY.ZZZ.WWW. In massive it looks like
   * {XXX, YYY, ZZZ, WWW}, and in memmory it looks like uint32_t.
   *--------------------------------------------------------------------------*/
  std::uint8_t ip[4] = {192, 168, 3, 1};
  std::uint8_t subnet[4] = {255, 255, 255, 0};
  std::uint8_t broadcast[4] = {0, 0, 0, 0};
  /* show information about IP-address
   *--------------------------------------------------------------------------*/
  std::cout << "IP is ";
  cout_of_inform(ip);
  /* show information about subnet mask
   *--------------------------------------------------------------------------*/
  std::cout << std::endl << "Subnet mask is ";
  cout_of_inform(subnet);
  /* find the broadcast address
   *--------------------------------------------------------------------------*/
  finding_a_broadcast_adress(ip, subnet, broadcast);
  /* show information about broadcast address
   *--------------------------------------------------------------------------*/
  std::cout << std::endl << "Broadcast address is ";
  cout_of_inform(broadcast);
  std::cout << std::endl;
  return 0;
}

void finding_a_broadcast_adress(std::uint8_t *ip_, std::uint8_t *subnet_,
                                std::uint8_t *broadcast_) {
  data_validation_of_ip(ip_);
  data_validation_of_subnet(subnet_);
  for (int i = 0; i < 4; i++) {
    broadcast_[i] = ip_[i] | (~subnet_[i]);
  }
  data_validation_of_broadcast(broadcast_);
}

void data_validation_of_subnet(std::uint8_t *data) {
  /*Because subnet mask for IPv4 can be only 1 of 32 values
   * I check if this subnet mask matches one of these values
   *--------------------------------------------------------------------------*/
  int i = 0;
  if (data[i] != 255 &&
      (data[i] == 0 || data[i] == 128 || data[i] == 192 || data[i] == 224 ||
       data[i] == 240 || data[i] == 248 || data[i] == 252 ||
       data[i] == 254)) { // first object in array
    i++;
    if (data[i] != 0) {
      std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
      cout_of_inform(data);
      return;
    }
    i++;
    if (data[i] != 0) {
      std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
      cout_of_inform(data);
      return;
    }
    i++;
    if (data[i] != 0) {
      std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
      cout_of_inform(data);
      return;
    }
  } else if (data[i] == 255) { // the same first object
    i++;
    if (data[i] != 255 &&
        (data[i] == 0 || data[i] == 128 || data[i] == 192 || data[i] == 224 ||
         data[i] == 240 || data[i] == 248 || data[i] == 252 ||
         data[i] == 254)) { // the second object
      i++;
      if (data[i] != 0) { // the third object
        std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
        cout_of_inform(data);
        return;
      }
      i++;
      if (data[i] != 0) { // the fourth object
        std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
        cout_of_inform(data);
        return;
      }
    } else if (data[i] == 255) { // the second object
      i++;
      if (data[i] != 255 &&
          (data[i] == 0 || data[i] == 128 || data[i] == 192 || data[i] == 224 ||
           data[i] == 240 || data[i] == 248 || data[i] == 252 ||
           data[i] == 254)) { // the third object
        i++;
        if (data[i] != 0) { // the fourth object
          std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
          cout_of_inform(data);
          return;
        }
      } else if (data[i] == 255) { // the third object
        i++;
        if (data[i] == 0 || data[i] == 128 || data[i] == 192 ||
            data[i] == 224 || data[i] == 240 || data[i] == 248 ||
            data[i] == 252 || data[i] == 254 ||
            data[i] == 255) { // the fourth object
          // TODO
          return;
        } else if (data[i] != 0 || data[i] != 128 || data[i] != 192 ||
                   data[i] != 224 || data[i] != 240 || data[i] != 248 ||
                   data[i] != 252 || data[i] != 254 || data[i] != 255) {
          std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
          cout_of_inform(data);
          return;
        }
      } else if (data[i] != 0 || data[i] != 128 || data[i] != 192 ||
                 data[i] != 224 || data[i] != 240 || data[i] != 248 ||
                 data[i] != 252 || data[i] != 254 || data[i] != 255) {
        std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
        cout_of_inform(data);
        return;
      }
    } else if (data[i] != 0 || data[i] != 128 || data[i] != 192 ||
               data[i] != 224 || data[i] != 240 || data[i] != 248 ||
               data[i] != 252 || data[i] != 254 || data[i] != 255) {
      std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
      cout_of_inform(data);
      return;
    }
  } else if (data[i] != 0 || data[i] != 128 || data[i] != 192 ||
             data[i] != 224 || data[i] != 240 || data[i] != 248 ||
             data[i] != 252 || data[i] != 254 || data[i] != 255) {
    std::cout << std::endl << "Invalid data in subnet mask " << std::endl;
    cout_of_inform(data);
    return;
  }
}
void data_validation_of_ip(std::uint8_t *data) {
  /*Because IP-address for IPv4 can be whatever
   * I check that the constituent addresses are > 0 and < 256
   *--------------------------------------------------------------------------*/
  for (int i = 0; i < 4; i++) {
    if (data[i] < 0 || data[i] > 255) {
      std::cout << "Invalid data in IP " << std::endl;
      cout_of_inform(data);
      return;
    }
  }
}

void data_validation_of_broadcast(std::uint8_t *data) {
  /*Broadcast like IP-address
   *--------------------------------------------------------------------------*/
  for (int i = 0; i < 4; i++) {
    if (data[i] < 0 || data[i] > 255) {
      std::cout << "Invalid data in broadcast address " << std::endl;
      cout_of_inform(data);
      return;
    }
  }
}
