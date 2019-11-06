#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include "tlpi_hdr.h"

static void usage_error(const char *prog_name, const char *msg)
{
  if (msg == NULL) {
    fprintf(stderr, "%s", msg);
  }

  fprintf(stderr, "Usage: %s [-cv] {-f pathname | -k key | -p} "
          "[octal-perms]\n", prog_name);
  fprintf(stderr, "    -c          Use IPC_CREAT flag\n");
  fprintf(stderr, "    -x          Use IPC_EXCL flag\n");
  fprintf(stderr, "    -f pathname Generate key using ftok()\n");
  fprintf(stderr, "    -k key      Use 'key' as key\n");
  fprintf(stderr, "    -p          Use IPC_PRIVATE key\n");

  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
  int num_key_flags; // Count -f, -k, and -p options
  int flags, msqid, opt;
  unsigned int perms;
  long lkey;
  key_t key;

  // Parse command-line options and arguments

  num_key_flags = 0;
  flags = 0;

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
      usage_error(argv[0], "Bad option\n");
    }
  }

  if (num_key_flags != 1) {
    usage_error(argv[0], "Exactly one of the options -f, -k, "
                "or -p must be supplied\n");
  }

  perms = (optind == argc) ? (S_IRUSR | S_IWUSR) :
    getInt(argv[optind], GN_BASE_8, "octal-perms");

  msqid = msgget(key, perms);
  if (msqid == -1) {
    errExit("msgget");
  }

  printf("%d\n", msqid);
  exit(EXIT_SUCCESS);
}
