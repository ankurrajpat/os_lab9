// Q2: user test for lazy allocation and the page fault handler.
#include "types.h"
#include "stat.h"
#include "user.h"

#define PG 4096

int
main(void)
{
  char *base;
  int i, n, pid;
  int pages[5] = {0, 3, 7, 15, 31};   // 5 different pages out of 32

  printf(1, "faults at start: %d\n", getfaults());
  base = sbrk(32 * PG);               // lazy: no memory allocated yet
  printf(1, "sbrk(32*4096) done, faults so far: %d (expected 0)\n", getfaults());

  for(i = 0; i < 5; i++){
    base[pages[i] * PG] = 'x';        // first touch -> page fault -> allocation
    printf(1, "wrote page %d (addr 0x%x), faults = %d\n",
           pages[i], (uint)(base + pages[i] * PG), getfaults());
  }
  n = getfaults();
  printf(1, "page faults handled: %d\n", n);
  if(n == 5)
    printf(1, "PASS: exactly 5 faults\n");
  else
    printf(1, "FAIL: expected 5\n");

  // Access above the allocated size: only the child must die.
  pid = fork();
  if(pid == 0){
    char *bad = base + 32 * PG + 10 * PG;   // beyond sz
    printf(1, "child: touching invalid address 0x%x\n", (uint)bad);
    *bad = 1;
    printf(1, "child: SHOULD NOT PRINT\n");
    exit();
  }
  wait();
  printf(1, "parent: child was killed, parent still running\n");
  base[pages[0] * PG] = 'y';          // parent still works (page already mapped)
  printf(1, "parent: done, faults = %d\n", getfaults());
  exit();
}
