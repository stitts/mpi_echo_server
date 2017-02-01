#include "parallel_server.hpp"

parallel_server::parallel_server(int rank, MPI_Comm comm, int port, int buffer_length, int master_rank)
  : m_server_socket(-1), m_client_socket(-1), m_port(port), m_buffer(NULL),
    m_buffer_length(buffer_length), m_rank(rank), m_comm(comm), m_master_rank(master_rank) {
  m_buffer = new char[m_buffer_length];
}

parallel_server::~parallel_server() {
  if (m_rank == m_master_rank) {
    if (m_server_socket > 0) close(m_server_socket);
    if (m_client_socket > 0) close(m_client_socket);
  }
  delete[] m_buffer;
}


int parallel_server::echo() {
  int length;
  int status = 0;

  if (m_rank == m_master_rank) {
    if ((length = read(m_client_socket, m_buffer, m_buffer_length)) < 1) {
      fprintf(stderr, "empty message\n");
      status = 1;
    }
  }
  MPI_Bcast(&status, 1, MPI_INT, m_master_rank, m_comm);
  if (status != 0) return status;
  MPI_Bcast(&length, 1, MPI_INT, m_master_rank, m_comm);
  MPI_Bcast(m_buffer, length, MPI_CHAR, m_master_rank, m_comm);
  m_buffer[length] = '\0';
  printf("%d - %s\n", m_rank, m_buffer);
  if (m_rank == m_master_rank) {
    if (write(m_client_socket, m_buffer, length) != length) {
      fprintf(stderr, "client is down\n");
      close(m_client_socket);
      m_client_socket = -1;
      status = 1;
    }
  }
  MPI_Bcast(&status, 1, MPI_INT, m_master_rank, m_comm);
  return status;
}


int parallel_server::bind() {
  int status = 0;

  if (m_rank == m_master_rank) {
    struct sockaddr_in addr;

    // create listening socket
    if ((m_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      fprintf(stderr, "problem creating socket: %s", strerror(errno));
      status = -1;
      goto bcast_bind;
    }

    // setup address
    addr.sin_family = AF_INET;
    addr.sin_port = htons(m_port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind port
    if (::bind(m_server_socket, (const struct sockaddr *)&addr, sizeof(struct sockaddr_in)) == -1) {
      fprintf(stderr, "problem binding\n");
      status = 1;
      goto bcast_bind;
    }

    // start listening
    if (listen(m_server_socket, 2) == -1) {
      fprintf(stderr, "problem listening\n");
      status = 1;
      goto bcast_bind;
    }

    // client address info
    struct sockaddr_in addr_client;
    socklen_t len = sizeof(struct sockaddr_in);

    fprintf(stderr, "listening for client\n");
    if ((m_client_socket = accept(m_server_socket, (struct sockaddr *)&addr_client, &len)) < 0) {
      fprintf(stderr, "bad client socket\n");
      status = 1;
      goto bcast_bind;
    }
    // print when we get a connection
    fprintf(stderr, "got connection: [%s, %d]\n", inet_ntoa(addr_client.sin_addr), ntohs(addr_client.sin_port));
  }

bcast_bind:
  if (status > 0) close(m_server_socket);
  MPI_Bcast(&status, 1, MPI_INT, m_master_rank, m_comm);
  return status;
}


int main(int argc, char** argv) {
  MPI_Init(&argc, &argv);
  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  parallel_server p(rank, MPI_COMM_WORLD);
  p.bind();
  while (p.echo() == 0) {}

  return 0;
}
