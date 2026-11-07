#include <stdlib.h>

void shuffle(int *a, int n) {
  for (int i = n - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int t = a[i];
    a[i] = a[j];
    a[j] = t;
  }
}