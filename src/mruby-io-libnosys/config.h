#ifndef MRUBY_IO_LIBNOSYS_CONFIG_H__
#define MRUBY_IO_LIBNOSYS_CONFIG_H__

#ifndef MRUBY_IO_LIBNOSYS_DRIVE_MAX
# define MRUBY_IO_LIBNOSYS_DRIVE_MAX (10)
#endif

#ifndef MRUBY_IO_LIBNOSYS_FD_MAX
# define MRUBY_IO_LIBNOSYS_FD_MAX (10)
#endif

/* assertion */
#if MRUBY_IO_LIBNOSYS_DRIVE_MAX > 10
# error canot set more than 11 to MRUBY_IO_LIBNOSYS_DRIVE_MAX
#endif
#if MRUBY_IO_LIBNOSYS_DRIVE_MAX <= 0
# error MRUBY_IO_LIBNOSYS_DRIVE_MAX should be positive value.
#endif

#if MRUBY_IO_LIBNOSYS_FD_MAX <= 0
# error MRUBY_IO_LIBNOSYS_FD_MAX should be positive value.
#endif

#endif /* MRUBY_IO_LIBNOSYS_CONFIG_H__ */
