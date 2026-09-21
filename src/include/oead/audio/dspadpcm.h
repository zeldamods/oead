#pragma once

#include "oead/types.h"

namespace oead::audio {
struct DspAdpcmParam {
  std::array<std::array<u16, 2>, 8> coefficients;
  u16 predictor_scale;
  s16 yn1;
  s16 yn2;

  OEAD_DEFINE_FIELDS(DspAdpcmParam, coefficients, predictor_scale, yn1, yn2);
};

struct DspAdpcmLoopParam {
  u16 loop_predictor_scale;
  s16 loop_yn1;
  s16 loop_yn2;

  OEAD_DEFINE_FIELDS(DspAdpcmLoopParam, loop_predictor_scale, loop_yn1,
                     loop_yn2);
};

struct DspAdpcmInfo {
  DspAdpcmParam param;
  DspAdpcmLoopParam loop_param;

  OEAD_DEFINE_FIELDS(DspAdpcmInfo, param, loop_param);
};
} // namespace oead::audio
