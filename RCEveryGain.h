#pragma once

#include "IPlug_include_in_plug_hdr.h"

const int kNumPresets = 1;

enum EParams
{
  kShiftMacro = 0,
  kShiftMicro,
  kShiftSize,
  kFader,
  kFaderCurve,
  kFaderSmoothing,
  kGainMaster,
  kGainLeft,
  kGainRight,
  kGainMid,
  kGainSide,
  kTrimMaster,
  kTrimLeft,
  kTrimRight,
  kTrimMid,
  kTrimSide,
  kNumParams
};

using namespace iplug;
using namespace igraphics;

class RCEveryGain final : public Plugin
{
public:
  RCEveryGain(const InstanceInfo& info);
  double fader_amp;

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};
