#include "fatso.h"
#include "internal.h"

#include <stdio.h>

static void
display_info_for_project(const struct fatso_project* p) {
  printf("Project: %s\nAuthor: %s\nVersion: %s\nToolchain: %s\n", p->name, p->author, fatso_version_string(&p->version), p->toolchain);

  printf("Dependencies:\n");
  // TODO: Use consolidated dependencies.
  for (size_t i = 0; i < p->base_environment.num_dependencies; ++i) {
    const struct fatso_dependency* dep = &p->base_environment.dependencies[i];
    printf("  %s", dep->name);
    if (dep->num_constraints) {
      printf(" (");
      for (size_t i = 0; i < dep->num_constraints; ++i) {
        const struct fatso_constraint* c = &dep->constraints[i];
        const char* vs = fatso_version_string(&c->version);
        printf("%s%s%s", fatso_version_requirement_to_string(c->version_requirement), vs ? " " : "", vs ? vs : "");
        if (i + 1 != dep->num_constraints) {
          printf(", ");
        }
      }
      printf(")");
    }
    printf("\n");
  }

  printf("Defines:\n");
  // TODO: Use consolidated defines.
  for (size_t i = 0; i < p->base_environment.num_defines; ++i) {
    const struct fatso_define* def = &p->base_environment.defines[i];
    printf("  %s: %s\n", def->key, def->value);
  }
}

int
fatso_info(struct fatso* f, int argc, char const* argv[]) {
  int r;
  if (argc == 1) {
    r = fatso_load_project(f);
    if (r != 0)
      goto out;

    display_info_for_project(f->project);
  } else {
    fprintf(stderr, "NIY: Package info\n");
    r = 1;
  }

out:
  return r;
}
