#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <mpi.h>

struct parallel_server {
  int m_server_socket;
  int m_client_socket;
  int m_port;
  char *m_buffer;
  int m_buffer_length;
  int m_rank;
  MPI_Comm m_comm;
  int m_master_rank;

  parallel_server(int rank, MPI_Comm comm, int port = 1234, int buffer_length = 1024, int master_rank = 0);
  ~parallel_server();
  int bind();
  int echo();
};
