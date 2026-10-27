// Q1: user test for the vmprint() system call.
// Prints the page table at start-up and again after sbrk(4 * 4096);
// exactly four new valid pages must appear.
#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  int before, after;

  printf(1, "=== vmprint at start-up ===\n");
  before = vmprint();

  sbrk(4 * 4096);

  printf(1, "=== vmprint after sbrk(4 * 4096) ===\n");
  after = vmprint();

  printf(1, "pages before = %d, after = %d, new pages = %d\n",
         before, after, after - before);
  if(after - before == 4)
    printf(1, "PASS: exactly four new pages appeared\n");
  else
    printf(1, "FAIL\n");
  exit();
}
