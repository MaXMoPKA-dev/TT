#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
/* hton, ntoh и проч. */
#include <arpa/inet.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <memory.h>
#include <stdio.h>
#include <unistd.h>

/* timer class
 *----------------------------------------------------------------------------*/
class timer {
public:
  timer() : m_beg(clock_t::now()) {}

  void reset() { m_beg = clock_t::now(); }

  float elapsed() const {
    return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
  }

private:
  // Type aliases to make accessing nested type easier
  using clock_t = std::chrono::high_resolution_clock;
  using second_t = std::chrono::duration<float, std::ratio<1>>;

  std::chrono::time_point<clock_t> m_beg;
};

/* function to create value
 *----------------------------------------------------------------------------*/
void create_value(bool &is_random, timer &timer_, double *val) {
  double value;
  [[maybe_unused]] double delta_time = timer_.elapsed();
  timer_.reset();
  if (is_random) {
    int r = rand() % 100;
    value = sin(7 * delta_time / 4) * r * 100000000;

  } else {
    value = sin(7 * delta_time / 4) * 100000000;
    /*std::cout << std::endl
                  << "-----" << value << "-----" << delta_time << "-----"
                  << std::endl;*/
  }
  val[0] = delta_time; // x - direction
  val[1] = value;      // y - direction
  val[2] = 0;          // z - direction
}

int main() {
  srand(time(NULL));
  while (true) {
    /*объявляем сокет*/
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
      perror("Error calling socket");
      return 0;
    }

    /*соединяемся по определённому порту с хостом*/
    struct sockaddr_in peer;
    peer.sin_family = AF_INET;
    peer.sin_port = htons(18666);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");
    int result = connect(s, (struct sockaddr *)&peer, sizeof(peer));
    if (result) {
      perror("Error calling connect");
      return 0;
    }

    /*посылаем данные
     *
     * Если быть точным, данные не посланы, а записаны где-то в стеке, когда и
     * как они будут отправлены реализации стека TCP/IP виднее. Зато мы сразу
     * получаем управление, не дожидаясь у моря погоды.*/

    // данные -- массив чисел;
    timer timer_;
    bool is_random = false;
    double value[3];
    create_value(is_random, timer_, value);
    result = send(s, &value, sizeof(value), 0);
    if (result <= 0) {
      perror("Error calling send");
      return 0;
    }
    /* закрываем соединения для посылки данных */
    if (shutdown(s, 1) < 0) {
      perror("Error calling shutdown");
      return 0;
    }

    /* читаем ответ сервера */
    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(0, &allreads);
    FD_SET(s, &allreads);
    while (true) {
      readmask = allreads;
      if (select(s + 1, &readmask, NULL, NULL, NULL) <= 0) {
        perror("Error calling select");
        return 0;
      }
      if (FD_ISSET(s, &readmask)) {
        char buffer[20];
        memset(buffer, 0, 20 * sizeof(char));
        int result = recv(s, buffer, sizeof(buffer) - 1, 0);
        if (result < 0) {
          perror("Error calling recv");
          return 0;
        }
        if (result == 0) {
          perror("Server disconnected");
          return 0;
        }
        if (strncmp(buffer, "Hi, dear!", 9) == 0) {
          printf("Got answer. Success.\n");
          break;
        } else {
          perror("Wrong answer!");
          break;
        }
      }
      if (FD_ISSET(0, &readmask)) {
        printf("No server response");
        return 0;
      }
    }
    close(s);
  }

  return 0;
}
