
#ifndef _PST_h
#define _PST_h

#include <memory>

#include "sys/time.h"

/**
 * @brief node 编码
    qi_gong_right          1
    qi_gong_left           2
    real_shen_long_boxing  3
    shen_long_boxing_left  4  长按 s a j
    shen_long_boxing_right 5  长按 s d j
 */

typedef struct {
  int pstcode;
  struct timeval tv;
  struct timezone tz;
} PstData;

int parse_pst(int *pstmsg_, std::shared_ptr<PstData> pstdata);

#endif

