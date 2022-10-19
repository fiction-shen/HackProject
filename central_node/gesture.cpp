// Copyright 2022 zhiwei.wang
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "gesture.h"

int parse_gst(int *gstmsg_, std::shared_ptr<GstData> gstdata) {
  gstdata->gstcode = static_cast<int>(*gstmsg_);
  //gettimeofday(&gstdata->tv, &gstdata->tz);
  return 0;
};