#include "fatso.h"
#include "internal.h"
#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h> // strlen

int
fatso_install(struct fatso* f, int argc, char* const* argv) {
  int r = 0;

  r = fatso_load_project(f);
  if (r != 0) goto out;

  char* packages_git_dir;
  asprintf(&packages_git_dir, "%s/packages/.git", fatso_home_directory(f));
  if (!fatso_directory_exists(packages_git_dir)) {
    fprintf(stderr, "Repository is empty. Please run `fatso sync`.\n");
    r = 1;
    goto out;
  }
  fatso_free(packages_git_dir);

  r = fatso_load_or_generate_dependency_graph(f);
  if (r != 0) goto out;

  r = fatso_install_dependencies(f);
  if (r != 0) goto out;

out:
  return r;
}

enum install_status {
  INSTALL_STATUS_OK,
  INSTALL_STATUS_WORKING,
  INSTALL_STATUS_ERROR,
};

static void
print_install_progress(struct fatso* f, struct fatso_package* p, enum install_status is, const char* fmt, ...) {
  const char* color;
  switch (is) {
    case INSTALL_STATUS_OK: color = GREEN; break;
    case INSTALL_STATUS_WORKING: color = YELLOW; break;
    case INSTALL_STATUS_ERROR: color = RED; break;
  }
  const char* version = fatso_version_string(&p->version);
  printf("\33[2K\r%s%s %s" RESET ": ", color, p->name, version);
  va_list ap;
  va_start(ap, fmt);
  vprintf(fmt, ap);
  va_end(ap);
}

int
fatso_package_download_and_unpack(struct fatso* f, struct fatso_package* p) {
  print_install_progress(f, p, INSTALL_STATUS_WORKING, "Downloading...");
  return 0;
}

int
fatso_package_build(struct fatso* f, struct fatso_package* p) {
  print_install_progress(f, p, INSTALL_STATUS_WORKING, "Building...");
  return 0;
}

int
fatso_package_install_products(struct fatso* f, struct fatso_package* p) {
  print_install_progress(f, p, INSTALL_STATUS_WORKING, "Configuring...");
  print_install_progress(f, p, INSTALL_STATUS_WORKING, "Installing...");
  return 0;
}

int
fatso_package_install(struct fatso* f, struct fatso_package* p) {
  int r;

  r = fatso_package_download_and_unpack(f, p);
  if (r != 0)
    goto out;

  r = fatso_package_build(f, p);
  if (r != 0)
    return r;

  r = fatso_package_install_products(f, p);
  if (r != 0)
    return r;

  // r = fatso_package_append_build_flags(f, p);
  // if (r != 0)
  //   return r;

  print_install_progress(f, p, INSTALL_STATUS_OK, "OK");
out:
  printf("\n");
  return r;
}

int
fatso_install_dependencies(struct fatso* f) {
  for (size_t i = 0; i < f->project->install_order.size; ++i) {
    struct fatso_package* p = f->project->install_order.data[i];
    int r = fatso_package_install(f, p);
    if (r != 0)
      return r;
  }
  return 0;
}
