#pragma once

#include "oead/types.h"

namespace oead::audio {
struct DspAdpcmParam {
  std::array<std::array<uint16_t, 2>, 8> coefficients;
  uint16_t predictor_scale;
  int16_t yn1;
  int16_t yn2;

  OEAD_DEFINE_FIELDS(DspAdpcmParam, coefficients, predictor_scale, yn1, yn2);
};

struct DspAdpcmLoopParam {
  uint16_t loop_predictor_scale;
  int16_t loop_yn1;
  int16_t loop_yn2;
  
  OEAD_DEFINE_FIELDS(DspAdpcmLoopParam, loop_predictor_scale, loop_yn1,
                     loop_yn2);
};

struct DspAdpcmInfo {
  DspAdpcmParam param;
  DspAdpcmLoopParam loop_param;

  OEAD_DEFINE_FIELDS(DspAdpcmInfo, param, loop_param);
};
} // namespace oead::audio