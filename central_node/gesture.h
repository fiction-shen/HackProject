#ifndef _GST_h
#define _GST_h

#include "sys/time.h"
#include <memory>

typedef struct {
  int gstcode;
  struct timeval tv;
  struct timezone tz;
} GstData;

int parse_gst(int *gstmsg_, std::shared_ptr<GstData> gstdata);

#endif