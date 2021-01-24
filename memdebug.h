/* Memory utils */

static int allocs = 0;
static int frees = 0;

void* malloc_checked(size_t size, char* file, int line) {
  void* ptr = malloc(size);
  if (ptr == NULL) {
    fprintf(stderr, "[%s:%d] malloc(%lu) failed", file, line, size);
    exit(EXIT_FAILURE);
  }

  return ptr;
}

void* my_malloc(size_t size, char* file, int line) {
  void* ptr = malloc_checked(size, file, line);
  allocs++;
  return ptr;
}

void my_free(void* ptr, char* file, int line) {
  if (ptr == NULL) {
    fprintf(stderr, "[%s:%d] attempt to free null ptr\n", file, line);
    exit(EXIT_FAILURE);
  }

  free(ptr);
  frees++;
}

#define malloc_checked(size) malloc_checked(size, __FILE__, __LINE__)
#define malloc(size) my_malloc(size, __FILE__, __LINE__)
#define free(ptr) my_free(ptr, __FILE__, __LINE__)

