/* demo_dynamic.c - demonstrate dynamic loading
 * and use of the "hello" routine */

/* need dlfcn.h to dynamically load libraries */
#include <dlfcn.h>

#include <stdlib.h>
#include <stdio.h>

/* we don't heed to include libhello.h.
 * we do need to specify something related;
 * we need to specify a type that will hold the value
 * we're going to get from dlsym() */

typedef void (*simple_demo_function)(void);

int main(void)
{
  const char *error;
  void *module;
  simple_demo_function demo_function;

  /* load dynamically loaded library */
  module = dlopen("libhello.so", RTLD_LAZY);
  if (!module) {
    fprintf(stderr, "Couldn't open libhello.so: %s\n",
            dlerror());
    exit(1);
  }

  /* get symbol */
  dlerror();
  demo_function = dlsym(module, "hello");
  if ((error = dlerror())) {
    fprintf(stderr, "Couldn't find hello: %s\n", error);
    exit(1);
  }

  /* call the function in DL library */
  (*demo_function)();

  /* close things cleanly */
  dlclose(module);
  return 0;
}
