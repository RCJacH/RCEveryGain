#include "RCEveryGain.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

RCEveryGain::RCEveryGain(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kShiftMacro)->InitInt("Macro Shift", 0, -8, 8, "Bit");
  GetParam(kShiftMicro)->InitDouble("Micro Shift", 0., -100.0, 100.0, 0.01, "%");
  GetParam(kShiftSize)->InitDouble("Shift Size", 1.0, 0.25, 3.0, 0.25, "");
  GetParam(kFader)->InitDouble("Fader", 100., 0., 100.0, 0.0001, "%");
  GetParam(kFaderCurve)->InitDouble("Fader Curve", 2., 0., 10.0, 0.1, "");
  GetParam(kFaderSmoothing)->InitDouble("Fader Smoothing", 2., 0., 10.0, 0.1, "");
  GetParam(kGainMaster)->InitDouble("Master Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainLeft)->InitDouble("Left Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainRight)->InitDouble("Right Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainCenter)->InitDouble("Center Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainSide)->InitDouble("Side Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kTrimMaster)->InitDouble("Master Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimLeft)->InitDouble("Left Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimRight)->InitDouble("Right Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimCenter)->InitDouble("Center Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimSide)->InitDouble("Side Trim", 0., -6., 6.0, 0.000001, "dB");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(IColor::FromHSLA(0.5972, 0.12, 0.3));
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 0.0, 700.0, 54.0), "RCEveryGain", IText(40.0, COLOR_WHITE), COLOR_TRANSPARENT));
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 56.0, 124.0, 72.0), "S", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 72.0, 124.0, 88.0), "H", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 88.0, 124.0, 104.0), "I", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 104.0, 124.0, 120.0), "F", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
		pGraphics->AttachControl(new ITextControl(IRECT(100.0, 120.0, 124.0, 136.0), "T", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    //pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50), "Hello iPlug 2!", IText(50)));
    // pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(-100), kGain));
  };
#endif
}

#if IPLUG_DSP
void RCEveryGain::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double macro_shift = GetParam(kShiftMacro) -> Value();
  const double micro_shift = GetParam(kShiftMicro) -> Value() / 100;
  const double shift = std::pow(2.0, macro_shift + micro_shift);
  const int nChans = NOutChansConnected();
  
  for (int s = 0; s < nFrames; s++) {
    for (int c = 0; c < nChans; c++) {
      outputs[c][s] = inputs[c][s] * shift;
    }
  }
}
#endif
