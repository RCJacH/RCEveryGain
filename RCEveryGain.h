#pragma once

#include "IControls.h"
#include "IPlug_include_in_plug_hdr.h"
#include "ISender.h"

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

enum ECtrlTags
{
  kCtrlTagInputMeter = 0,
  kCtrlTagOutputMeter,
  kCtrlShiftMacroTabSwitch,
  kCtrlShiftSizeTabSwitch,
  kNumCtrlTags
};

using namespace iplug;
using namespace igraphics;

class RCEveryGain final : public Plugin
{
public:
  RCEveryGain(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd

  void OnParamChange(int idx) override;
  void OnIdle() override;
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
  void OnReset() override;

  IPeakAvgSender<2> mInputPeakSender;
  IPeakAvgSender<2> mOutputPeakSender;
#endif

private:
  double fader_amp;
  double shift_size;
};
