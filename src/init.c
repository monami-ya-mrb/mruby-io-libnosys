#include "mruby.h"
#include "mruby-io-libnosys/extern.h"

void
mrb_mruby_io_libnosys_gem_init(mrb_state *mrb)
{
  mrb_mruby_io_libnosys_posix_init(mrb);
}

void
mrb_mruby_io_libnosys_gem_final(mrb_state *mrb)
{
  mrb_mruby_io_libnosys_posix_final(mrb);
}

