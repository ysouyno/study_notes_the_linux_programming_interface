#include <mqueue.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <limits.h>
#include "tlpi_hdr.h"

#define SERVER_MQ_NAME "/mq_file_server"
#define CLIENT_MQ_NAME "/mq_file_client_%ld"
#define CLIENT_MQ_NAME_LEN (sizeof(CLIENT_MQ_NAME) + 20)

struct requ_msg
{
  char client_mq_name[PATH_MAX];
  char pathname[PATH_MAX];
};

#define REQU_SIZE PATH_MAX * 2

#define DATA_SIZE 4096

struct resp_msg
{
  long type;
  char data[DATA_SIZE];
};

#define RESP_SIZE DATA_SIZE + sizeof(long)

#define RESP_MT_FAILURE 1
#define RESP_MT_DATA    2
#define RESP_MT_END     3
