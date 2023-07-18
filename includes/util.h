#ifndef __UTIL_H__
#define __UTIL_H__

#define ERR_MSG(msg)                                                           \
  do {                                                                         \
    fprintf(stderr, "line: %d\n", __LINE__);                                   \
    perror(msg);                                                               \
  } while (0)

#endif // !__UTIL_H__
