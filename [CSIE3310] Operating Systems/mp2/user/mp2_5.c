#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/vm.h"
#include "user/user.h"

#define PGSIZE 4096

int main(int argc, char *argv[]) {
  int result = 0;

  char *page1 = sbrk(PGSIZE * 3);
  char *page2 = page1 + PGSIZE;
  char *page3 = page2 + PGSIZE;

  /* Trigger page fault */
  *page2 = 1;

  printf("# After page fault (1)\n");
  vmprint();
  printf("\n");

  /* madvise(MADV_DONTNEED) */
  result = madvise(page1, PGSIZE * 2, MADV_DONTNEED);
  if (result != 0) {
    printf("error: return value of madvise() is incorrect.\n");
    exit(-1);
  }

  printf("# After madvise(DONTNEED) (1)\n");
  vmprint();
  printf("\n");

  /* Trigger page fault on swapped page */
  if (*page2 != 1) {
    printf("error: data integrity test failed\n");
    exit(1);
  }

  printf("# After page fault (2)\n");
  vmprint();
  printf("\n");

  /* madvise(MADV_WILLNEED) */
  result = madvise(page2 - 1, 2, MADV_WILLNEED);
  if (result != 0) {
    printf("error: return value of madvise() is incorrect.\n");
    exit(-1);
  }

  printf("# After madvise(WILLNEED) (1)\n");
  vmprint();
  printf("\n");

  /* madvise(MADV_WILLNEED) */
  result = madvise(page3 + PGSIZE - 1, 1, MADV_WILLNEED);
  if (result != 0) {
    printf("error: return value of madvise() is incorrect.\n");
    exit(-1);
  }

  printf("# After madvise(WILLNEED) (2)\n");
  vmprint();
  printf("\n");

  /* Write value */
  *(page3 + PGSIZE - 1) = 7;

  /* madvise(MADV_DONTNEED) */
  result = madvise(page1 + PGSIZE - 1, PGSIZE + 2, MADV_DONTNEED);
  if (result != 0) {
    printf("error: return value of madvise() is incorrect.\n");
    exit(-1);
  }

  printf("# After madvise(DONTNEED) (2)\n");
  vmprint();
  printf("\n");

  /* madvise(MADV_DONTNEED) */
  result = madvise(page2 - 1, 2, MADV_DONTNEED);
  if (result != 0) {
    printf("error: return value of madvise() is incorrect.\n");
    exit(-1);
  }

  printf("# After madvise(DONTNEED) (3)\n");
  vmprint();
  printf("\n");

  /* Trigger page fault on swapped page again */
  if (*(page3 + PGSIZE - 1) != 7) {
    printf("error: data integrity test failed\n");
    exit(1);
  }

  printf("# After page fault (3)\n");
  vmprint();

  exit(0);
}
