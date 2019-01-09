#include <stdio.h>
#include <svdpi.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <map>
#include <vector>
#include <string>
#include <thread>
#include <queue>

// state is global
int id = 0;
std::vector<std::string> names;
std::vector<int> ports;
std::vector<int> connections;
std::vector<int> server_sockets;
std::vector<std::queue<uint64_t>> rx_queues;


extern "C" {
int dpi_comm_init(char* name, int port, int wait_for_connection) {
  int error;
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in address;
  socklen_t address_length = sizeof(address);

  if (!sock) {
    printf("failed to create socket.\n");
    return -1;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(port);

  error = bind(sock, reinterpret_cast<struct sockaddr *>(&address), address_length);

  if (error == -1) {
    printf("%s: failed to bind to port %d\n", name, port);
    return -1;
  }

  error = listen(sock,1);

  if (error == -1) {
    printf("%s: failed to listen\n");
    return -1;
  }

  if (wait_for_connection == 0) {
    printf("%s: warning this feature is not implemented, will block anyways.", name);
  }

  printf("%s: Waiting for connection on port: %d...\n", name, port);
  int connection = accept(sock, reinterpret_cast<struct sockaddr *>(&address), &address_length);


  if (connection == -1) {
    printf("%s: failed to accept connection.");
    return -1;
  }


  printf("Client connected to %s.\n", name);
  names.push_back(std::string(name));
  ports.push_back(port);
  server_sockets.push_back(sock); // TODO: We need to close them at some point.
  connections.push_back(connection);
  rx_queues.push_back(std::queue<uint64_t>());

  return id++;
}

int dpi_comm_tx(int id, uint64_t data) {
  if (id < 0) {
    return 0; // tx_ready
  }

  int connection = connections[id];
  int size = send(connection, &data, 8, 0);

  return 1; // tx_ready
}

int dpi_comm_rx(int id, int rx_ready, uint64_t* rx_data) {
  if (id < 0) {
    return 0; // rx_valid
  }

  int length = 0;
  int connection = connections[id];

  ioctl(connection, FIONREAD, &length);

  if (length >= 8) {
    uint64_t data = 0;
    read(connection, &data, 8);
    rx_queues[id].push(data);
  }

  if (rx_queues[id].empty()) {
    *rx_data = 0;
    return 0; // rx_valid
  }

  *rx_data = rx_queues[id].front();

  if (rx_ready) {
    rx_queues[id].pop();
  }

  return 1; // rx_valid
}
}
