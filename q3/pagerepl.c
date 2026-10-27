// Q3: FIFO and LRU page replacement simulator for xv6.
//
// Usage:
//   pagerepl <frames> <p,p,p,...> [fifo|lru]   trace both (or only one) algorithm
//   pagerepl -t <p,p,p,...>           table for 3, 4 and 5 frames
// The reference string is one argument with comma-separated page numbers
// (the xv6 shell only allows 10 arguments), e.g. 7,0,1,2,0,3,0,4,...
// Plain space-separated pages also work when there are few of them.
//
// For every reference the frame contents and HIT/FAULT are printed.
// xv6 has no floating point, so hit ratios are printed as percentages
// with two decimals using integer arithmetic.
#include "types.h"
#include "stat.h"
#include "user.h"

#define MAXF 16
#define MAXR 64

static int refs[MAXR];
static int nrefs;

static void
printframes(int *frames, int nf)
{
  int i;
  printf(1, "[");
  for(i = 0; i < nf; i++){
    if(frames[i] < 0)
      printf(1, " -");
    else
      printf(1, " %d", frames[i]);
  }
  printf(1, " ]");
}

// lru = 0 -> FIFO, lru = 1 -> LRU. Returns number of page faults.
static int
simulate(int nf, int lru, int verbose)
{
  int frames[MAXF];
  int stamp[MAXF];      // FIFO: load time; LRU: last use time
  int i, t, hit, victim, faults = 0, next = 0;

  for(i = 0; i < nf; i++){
    frames[i] = -1;
    stamp[i] = 0;
  }
  if(verbose)
    printf(1, "--- %s, %d frames ---\n", lru ? "LRU" : "FIFO", nf);

  for(t = 0; t < nrefs; t++){
    hit = -1;
    for(i = 0; i < nf; i++)
      if(frames[i] == refs[t]){
        hit = i;
        break;
      }
    if(hit >= 0){
      if(lru)
        stamp[hit] = t + 1;       // refresh recency on hit (LRU only)
    } else {
      faults++;
      if(next < nf){
        victim = next++;          // a free frame is still available
      } else {
        victim = 0;               // evict the smallest stamp:
        for(i = 1; i < nf; i++)   // oldest loaded (FIFO) / least recent (LRU)
          if(stamp[i] < stamp[victim])
            victim = i;
      }
      frames[victim] = refs[t];
      stamp[victim] = t + 1;
    }
    if(verbose){
      printf(1, "ref %d: ", refs[t]);
      printframes(frames, nf);
      printf(1, "  %s\n", hit >= 0 ? "HIT" : "FAULT");
    }
  }
  if(verbose)
    printf(1, "%s total page faults: %d\n", lru ? "LRU" : "FIFO", faults);
  return faults;
}

// print hits/total as a percentage with two decimals
static void
printratio(int hits, int total)
{
  int x = hits * 10000 / total;
  printf(1, "%d.%d%d%%", x / 100, (x / 10) % 10, x % 10);
}

int
main(int argc, char *argv[])
{
  int i, nf, start, f, ff = 0, lf = 0, only = 0;

  if(argc < 3){
    printf(2, "usage: pagerepl <frames> <p,p,p,...> | pagerepl -t <p,p,p,...>\n");
    exit();
  }
  if(argv[1][0] == '-' && argv[1][1] == 't'){
    nf = 0;
    start = 2;
  } else {
    nf = atoi(argv[1]);
    start = 2;
    if(nf < 1 || nf > MAXF){
      printf(2, "frames must be 1..%d\n", MAXF);
      exit();
    }
  }
  if(argc > 3 && argv[argc-1][0] == 'f'){ only = 1; argc--; }   // "fifo"
  else if(argc > 3 && argv[argc-1][0] == 'l'){ only = 2; argc--; } // "lru"
  for(i = start; i < argc; i++){
    char *c = argv[i];
    while(*c && nrefs < MAXR){
      if(*c >= '0' && *c <= '9'){
        int v = 0;
        while(*c >= '0' && *c <= '9')
          v = v * 10 + (*c++ - '0');
        refs[nrefs++] = v;
      } else
        c++;                        // skip commas / separators
    }
  }

  printf(1, "reference string (%d refs):", nrefs);
  for(i = 0; i < nrefs; i++)
    printf(1, " %d", refs[i]);
  printf(1, "\n");

  if(nf > 0){
    if(only != 2)
      ff = simulate(nf, 0, 1);
    if(only != 1)
      lf = simulate(nf, 1, 1);
    if(only == 0)
      printf(1, "\nSUMMARY with %d frames: FIFO = %d faults, LRU = %d faults\n",
             nf, ff, lf);
  } else {
    printf(1, "\nframes | FIFO faults  hit ratio | LRU faults  hit ratio\n");
    printf(1, "-------+------------------------+-----------------------\n");
    for(f = 3; f <= 5; f++){
      ff = simulate(f, 0, 0);
      lf = simulate(f, 1, 0);
      printf(1, "   %d   |     %d        ", f, ff);
      printratio(nrefs - ff, nrefs);
      printf(1, "    |     %d        ", lf);
      printratio(nrefs - lf, nrefs);
      printf(1, "\n");
    }
  }
  exit();
}
