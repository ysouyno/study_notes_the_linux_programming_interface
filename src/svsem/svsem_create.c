#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

static void usage_error(const char *prog_name, const char *msg)
{
  if (msg != NULL) {
    fprintf(stderr, "%s", msg);
  }

  fprintf(stderr, "Usage: %s [-cx] {-f pathname | -k | -p} "
          "num-sems [octal-perms]\n", prog_name);
  fprintf(stderr, "    -c          Use IPC_CREAT flag\n");
  fprintf(stderr, "    -x          Use IPC_EXCL flag\n");
  fprintf(stderr, "    -f pathname Generate key using ftok()\n");
  fprintf(stderr, "    -k key      Use `key` as key\n");
  fprintf(stderr, "    -p          Use IPC_PRIVATE key\n");

  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int flags, semid, num_sems, opt;
  int num_key_flags;
  unsigned int perms;
  long lkey;
  key_t key;

  while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {
    switch (opt) {
    case 'c':
      flags |= IPC_CREAT;
      break;
    case 'f':
      key = ftok(optarg, 1);
      if (key == -1) {
        errExit("ftok");
      }

      num_key_flags++;
      break;
    case 'k': // -k key (octal, decimal or hexadecimal)
      if (sscanf(optarg, "%li", &lkey) != 1) {
        cmdLineErr("-k option requires a numeric argument\n");
      }

      key = lkey;
      num_key_flags++;
      break;
    case 'p':
      key = IPC_PRIVATE;
      num_key_flags++;
      break;
    case 'x':
      flags |= IPC_EXCL;
      break;
    default:
      usage_error(argv[0], NULL);
    }
  }

  if (num_key_flags != 1) {
    usage_error(argv[0], "Exactly one of the options -f, -k, "
                "or -p must be supplied\n");
  }

  if (optind >= argc) {
    usage_error(argv[0], "Must specify number of semaphore\n");
  }

  num_sems = getInt(argv[optind], 0, "num-sems");

  perms = (argc <= optind + 1) ? (S_IRUSR | S_IWUSR) :
    getInt(argv[optind + 1], GN_BASE_8, "octal-perms");

  semid = semget(key, num_sems, flags | perms);
  if (semid == -1) {
    errExit("semget");
  }

  printf("%d\n", semid);
  exit(EXIT_SUCCESS);
}
