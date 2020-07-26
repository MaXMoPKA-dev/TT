/* определяет типы данных */
#include <sys/types.h>
/* "Главный" по сокетам */
#include <sys/socket.h>
/* sockaddr_in struct, sin_family, sin_port, in_addr_t, in_port_t, ...*/
#include <netinet/in.h>
/* to close socket*/
#include <unistd.h>

#include <errno.h>
#include <iostream>
#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengles2.h"
#include "glad/glad.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

const char *vertexShaderSource =
    "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
const char *fragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n\0";

/**@brief Получает от клиента последовательность байт, не длиннее 30 и печатает
 * её на экран по завершении соединения. Клиенту отправляет "Hi, dear!"*/
void create_EBO_and_VBO(std::vector<double> &vertices,
                        std::vector<unsigned int> &indexes, double *val) {
  vertices.push_back(val[0]);
  vertices.push_back(val[1]);
  vertices.push_back(val[2]);
  if (vertices.size() / 3 == 2) {
    indexes.push_back(vertices.size() / 3 - 1);
  } else {
    int i = vertices.size() / 3 - 2;
    int j = vertices.size() / 3 - 1;
    indexes.push_back(i);
    indexes.push_back(j);
  }
}
int main() {

  std::vector<double> vertices = {0, 0, 0};
  std::vector<unsigned int> indexes = {0};

  SDL_Window *window;
  SDL_GLContext gl_context;

  const int init_result = SDL_Init(SDL_INIT_EVERYTHING);
  if (init_result != 0) {
    const char *err_message = SDL_GetError();
    std::cout << "error: failed call SDL_Init: " << err_message << std::endl;
    return 0;
  }

  window =
      SDL_CreateWindow("title", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       SCR_WIDTH, SCR_HEIGHT, ::SDL_WINDOW_OPENGL);

  if (window == nullptr) {
    const char *err_message = SDL_GetError();
    std::cout << "error: failed call SDL_CreateWindow: " << err_message
              << std::endl;
    SDL_Quit();
    return 0;
  }

  int gl_major_version = 3;
  int gl_minor_version = 3;
  int gl_context_profile = SDL_GL_CONTEXT_PROFILE_CORE;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, gl_context_profile);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, gl_major_version);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, gl_minor_version);

  gl_context = SDL_GL_CreateContext(window);
  if (gl_context == nullptr) {
    std::string msg("can't create opengl context: ");
    msg += SDL_GetError();
    std::cout << msg << std::endl;
    return 0;
  }

  gl_major_version = 0;
  gl_minor_version = 0;

  int result =
      SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &gl_major_version);

  result = SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &gl_minor_version);

  if (gladLoadGLES2Loader(SDL_GL_GetProcAddress) == 0) {
    std::clog << "error: failed to initialize glad" << std::endl;
  }

  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  // check for shader compile errors
  int success;
  char infoLog[512];
  glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // fragment shader
  int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  // check for shader compile errors
  glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n"
              << infoLog << std::endl;
  }
  // link shaders
  int shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  // check for linking errors
  glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n"
              << infoLog << std::endl;
  }
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  /*создаём сокет*/
  int s = socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) {
    perror("Error calling socket");
    return 0;
  }

  /*определяем прослушиваемый порт и адрес*/
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(18666);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
    perror("Error calling bind");
    return 0;
  }

  /*помечаем сокет, как пассивный - он будет слушать порт*/
  if (listen(s, 5)) {
    perror("Error calling listen");
    return 0;
  }

  /*начинаем слушать, для соединения создаём другой сокет, в котором можем
   * общаться.*/
  while (true) {
    int s1 = accept(s, NULL, NULL);

    if (s1 < 0) {
      perror("Error calling accept");
      return 0;
    }

    /*читаем данные из сокета*/
    double buffer[3];
    int counter = 0;
    while (true) {
      /*следует помнить, что данные поступают неравномерно*/
      int rc = recv(s1, &buffer, sizeof(buffer), 0);
      if (rc < 0) {
        /*чтение может быть прервано системным вызовом, это нормально*/
        if (errno == EINTR)
          continue;
        perror("Can't receive data.");
        return 0;
      }
      if (rc == 0)
        break;
      create_EBO_and_VBO(vertices, indexes, buffer);
      glBindVertexArray(VAO);

      glBindBuffer(GL_ARRAY_BUFFER, VBO);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices[0],
                   GL_STATIC_DRAW);

      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), &indexes[0],
                   GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                            (void *)0);
      glEnableVertexAttribArray(0);

      // note that this is allowed, the call to glVertexAttribPointer registered
      // VBO as the vertex attribute's bound vertex buffer object so afterwards
      // we can safely unbind
      glBindBuffer(GL_ARRAY_BUFFER, 0);

      // remember: do NOT unbind the EBO while a VAO is active as the bound
      // element buffer object IS stored in the VAO; keep the EBO bound.
      // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

      // You can unbind the VAO afterwards so other VAO calls won't accidentally
      // modify this VAO, but this rarely happens. Modifying other VAOs requires
      // a call to glBindVertexArray anyways so we generally don't unbind VAOs
      // (nor VBOs) when it's not directly necessary.
      glBindVertexArray(0);
      std::cout << buffer[0] << "-----" << buffer[1] << "-----" << buffer[2]
                << std::endl;
      glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      // draw our first triangle
      glUseProgram(shaderProgram);
      glBindVertexArray(VAO); // seeing as we only have a single VAO there's no
                              // need to bind it every time, but we'll do so to
                              // keep things a bit more organized
      glDrawElements(GL_LINES, indexes.size(), GL_UNSIGNED_INT, 0);
      SDL_GL_SwapWindow(window);

      glClear(GL_COLOR_BUFFER_BIT);
      char response[] = "Hi, dear!";
      if (sendto(s1, response, sizeof(response), 0, (struct sockaddr *)&addr,
                 sizeof(addr)) < 0)
        perror("Error sending response");
      printf("Response send\n");
    }
    close(s1);
  }
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteProgram(shaderProgram);
  return 0;
}
