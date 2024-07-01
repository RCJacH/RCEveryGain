#include "RCEveryGain.h"
#include "IPlug_include_in_plug_src.h"
#include "IControls.h"

RCEveryGain::RCEveryGain(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(kShiftMacro)->InitInt("Macro Shift", 0, -8, 8, "Bit");
  GetParam(kShiftMicro)->InitDouble("Micro Shift", 0., -100.0, 100.0, 0.01, "%");

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
