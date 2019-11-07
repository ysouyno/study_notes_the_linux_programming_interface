#include <sys/types.h>
#include <sys/sem.h>
#include <ctype.h>
#include "curr_time.h"
#include "tlpi_hdr.h"

// Maximum operations that we permit for a single semop()
#define MAX_SEMOPS 1000

static void usage_error(const char *prog_name)
{
  fprintf(stderr, "Usage: %s semid op[,op...] ...\n\n", prog_name);
  fprintf(stderr, "`op` is either: <sem#>{+|-}<value>[n][u]\n");
  fprintf(stderr, "            or: <sem#>=0[n]\n");
  fprintf(stderr, "       \"n\" means include IPC_NOWAIT in `op`\n");
  fprintf(stderr, "       \"u\" means include SEM_UNDO in `op`\n\n");
  fprintf(stderr, "The operations in each argument are "
          "performed in a single semop() call\n\n");
  fprintf(stderr, "e.g.: %s 12345 0+1,1-2un\n", prog_name);
  fprintf(stderr, "      %s 12345 0=0n 1+1,2-1u 1=0\n", prog_name);

  exit(EXIT_FAILURE);
}

// Parse comma-delimited operations in `arg`, returning them in the
// array `sops`. Return number of operations as function result.

static int parse_ops(char *arg, struct sembuf sops[])
{
  char *comma, *sign, *remaining, *flags;
  int num_ops; // Number of operations in `arg`

  for (num_ops = 0, remaining = arg; ; ++num_ops) {
    if (num_ops >= MAX_SEMOPS) {
      cmdLineErr("Too many operations (maximum = %ld): \"%s\"\n",
                 MAX_SEMOPS, arg);
    }

    if (*remaining == '\0') {
      fatal("Trailing comma or empty argument: \"%s\"", arg);
    }

    if (!isdigit((unsigned char)*remaining)) {
      cmdLineErr("Expected initial digit: \"%s\"\n", arg);
    }

    sops[num_ops].sem_num = strtol(remaining, &sign, 10);

    if (*sign == '\0' || strchr("+-=", *sign) == NULL) {
      cmdLineErr("Expected `+`, `-`, or `=` in \"%s\"\n", arg);
    }

    if (!isdigit((unsigned char)*(sign + 1))) {
      cmdLineErr("Expected digit after `%c` in \"%s\"\n", *sign, arg);
    }

    sops[num_ops].sem_op = strtol(sign + 1, &flags, 10);

    if (*sign == '-') { // Reverse sign of operation
      sops[num_ops].sem_op = - sops[num_ops].sem_op;
    }
    else if (*sign == '=') { // Should be `=0`
      if (sops[num_ops].sem_op != 0) {
        cmdLineErr("Expected \"=0\" in \"%s\"\n", arg);
      }
    }

    sops[num_ops].sem_flg = 0;
    for (; ; flags++) {
      if (*flags == 'n') {
        sops[num_ops].sem_flg |= IPC_NOWAIT;
      }
      else if (*flags == 'u') {
        sops[num_ops].sem_flg |= SEM_UNDO;
      }
      else {
        break;
      }
    }

    if (*flags != ',' && *flags != '\0') {
      cmdLineErr("Bad trailing character (%c) in \"%s\"\n", *flags, arg);
    }

    comma = strchr(remaining, ',');
    if (comma == NULL) {
      break; // No comma -> no more ops
    }
    else {
      remaining = comma + 1;
    }
  }

  return num_ops + 1;
}

int main(int argc, char *argv[])
{
  struct sembuf sops[MAX_SEMOPS];
  int ind, nsops;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usage_error(argv[0]);
  }

  for (ind = 2; argv[ind] != NULL; ++ind) {
    nsops = parse_ops(argv[ind], sops);

    printf("%5ld, %s: about to semop()  [%s]\n", (long)getpid(),
           currTime("%T"), argv[ind]);

    if (semop(getInt(argv[1], 0, "semid"), sops, nsops) == -1) {
      errExit("semop (PID = %ld)", (long)getpid());
    }

    printf("%5ld, %s: semop() completed [%s]\n", (long)getpid(),
           currTime("%T"), argv[ind]);
  }

  exit(EXIT_SUCCESS);
}
