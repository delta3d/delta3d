#ifndef Prof_INC_PROF_LINUX_H
#define Prof_INC_PROF_LINUX_H

typedef unsigned long long Prof_Int64;

#ifdef __cplusplus
  inline
#else
  static
#endif
      void Prof_get_timestamp(Prof_Int64 *result)
      {
         __asm__ volatile (".byte 0x0f, 0x31" : "=A" (result));
      }

#endif
