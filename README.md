# OS Assignment 9 - xv6 memory management
- q1/: vmprint syscall + pttest.c (changed files, changes.diff vs mit-pdos/xv6-public)
- q2/: lazy allocation + page fault handler + getfaults + faulttest.c
- q3/: pagerepl.c (FIFO/LRU simulator, user program)
- screenshots/: real captures of the QEMU VGA console (QEMU screendump) while the tests ran
- logs/: raw serial console logs of the runs
- report.pdf: 1-page report

Each q* folder is a change set on top of https://github.com/mit-pdos/xv6-public.
Apply: `git clone https://github.com/mit-pdos/xv6-public && cd xv6-public && patch -p1 < ../q1/changes.diff`
(the diff is made with `diff -ruN`, so `patch -p1` after cd works with the directory prefix stripped), then `make qemu-nox`.
