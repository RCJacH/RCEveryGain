#include "RCEveryGain.h"
#include "IControls.h"
#include "IPlug_include_in_plug_src.h"

RCEveryGain::RCEveryGain(const InstanceInfo& info)
  : iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  fader_amp = 0.0;

  GetParam(kShiftMacro)->InitInt("Macro Shift", 0, -8, 8, "Bit");
  GetParam(kShiftMicro)->InitDouble("Micro Shift", 0., -100.0, 100.0, 0.01, "%");
  GetParam(kShiftSize)->InitDouble("Shift Size", 1.0, 0.25, 3.0, 0.25, "");
  GetParam(kFader)->InitDouble("Fader", 100., 0., 100.0, 0.0001, "%");
  GetParam(kFaderCurve)->InitDouble("Fader Curve", 2., 0., 10.0, 0.1, "");
  GetParam(kFaderSmoothing)->InitDouble("Fader Smoothing", 2., 0., 10.0, 0.1, "");
  GetParam(kGainMaster)->InitDouble("Master Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainLeft)->InitDouble("Left Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainRight)->InitDouble("Right Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainMid)->InitDouble("Mid Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kGainSide)->InitDouble("Side Gain", 0., -60., 60.0, 0.1, "dB");
  GetParam(kTrimMaster)->InitDouble("Master Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimLeft)->InitDouble("Left Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimRight)->InitDouble("Right Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimMid)->InitDouble("Mid Trim", 0., -6., 6.0, 0.000001, "dB");
  GetParam(kTrimSide)->InitDouble("Side Trim", 0., -6., 6.0, 0.000001, "dB");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() { return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT)); };

  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(IColor::FromHSLA(0.5972, 0.12, 0.3));
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();

    // General Layout
    const IRECT center = b.GetMidHPadded(300);
    const IRECT upper = center.FracRectVertical(0.618 * 0.382, true);
    const IRECT lower = center.GetReducedFromTop(upper.H());
    const IRECT shift = upper.FracRectVertical(0.618);
    const IRECT header = upper.GetReducedFromBottom(shift.H());
    const IRECT volume = lower.FracRectHorizontal(0.618, true);
    const IRECT fader = lower.GetReducedFromRight(volume.W());
    const IRECT gain = volume.FracRectVertical(0.5, true);
    const IRECT trim = volume.FracRectVertical(0.5);

    // Shift Section
    const IRECT shift_inner = shift.GetPadded(-4.0);
    const IRECT shift_header = shift_inner.GetFromLeft(24.0);
    const IRECT shift_labels = shift_inner.FracRectHorizontal(0.1, true);
    const IRECT shift_controls = shift_inner.GetReducedFromLeft(shift_header.W()).GetReducedFromRight(shift_labels.W());
    const IRECT shift_control_macro = shift_controls.FracRectVertical(1. / 3., true);
    const IRECT shift_control_micro = shift_controls.GetReducedFromTop(shift_control_macro.H()).FracRectVertical(0.5, true);
    const IRECT shift_control_size = shift_controls.FracRectVertical(1. / 3.);
    const IRECT shift_label_macro = shift_labels.FracRectVertical(1. / 3., true);
    const IRECT shift_label_micro = shift_labels.GetReducedFromTop(shift_label_macro.H()).FracRectVertical(0.5, true);
    const IRECT shift_label_size = shift_labels.FracRectVertical(1. / 3.);

    const IVStyle shift_macro_style = IVStyle::IVStyle().WithShowLabel(false).WithShowValue(true).WithRoundness(0.0);

    pGraphics->AttachControl(new ITextControl(header, "RCEveryGain", IText(40.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(IRECT(100.0, 56.0, 124.0, 72.0), "S", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(IRECT(100.0, 72.0, 124.0, 88.0), "H", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(IRECT(100.0, 88.0, 124.0, 104.0), "I", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(IRECT(100.0, 104.0, 124.0, 120.0), "F", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(IRECT(100.0, 120.0, 124.0, 136.0), "T", IText(20.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new IVSliderControl(shift_control_macro, kShiftMacro, "", shift_macro_style, true, iplug::igraphics::EDirection::Horizontal));
    pGraphics->AttachControl(new IVSliderControl(shift_control_micro, kShiftMicro, "", shift_macro_style, true, iplug::igraphics::EDirection::Horizontal));
    pGraphics->AttachControl(new IVSliderControl(shift_control_size, kShiftSize, "", shift_macro_style, true, iplug::igraphics::EDirection::Horizontal));
    pGraphics->AttachControl(new ITextControl(shift_label_macro, "MACRO", IText(16.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(shift_label_micro, "MICRO", IText(16.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new ITextControl(shift_label_size, "SIZE", IText(16.0, COLOR_WHITE), COLOR_TRANSPARENT));

    // Fader Section
    const IRECT fader_inner = fader.GetPadded(-4.0);
    const IRECT fader_non_fader = fader_inner.FracRectHorizontal(0.382);
    const IRECT fader_fader = fader_inner.GetReducedFromLeft(fader_non_fader.W());
    const IRECT fader_knobs = fader_non_fader.FracRectVertical(0.618, true);
    const IRECT fader_smooth = fader_knobs.FracRectVertical(0.5);
    const IRECT fader_curve = fader_knobs.FracRectVertical(0.5, true);
    const IRECT fader_header = fader_non_fader.GetReducedFromTop(fader_knobs.H());

    const IVStyle fader_knob_style = IVStyle::IVStyle();
    pGraphics->AttachControl(new IVSliderControl(fader_fader, kFader, "", shift_macro_style, true, iplug::igraphics::EDirection::Vertical));
    pGraphics->AttachControl(new ITextControl(fader_header, "FADER", IText(24.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new IVKnobControl(fader_curve.GetPadded(-4.), kFaderCurve, "Curve", fader_knob_style));
    pGraphics->AttachControl(new IVKnobControl(fader_smooth.GetPadded(-4.), kFaderSmoothing, "Smooth", fader_knob_style));



    // Gain Section

    const IRECT gain_inner = gain.GetHPadded(-4.);
    const IRECT gain_top_row = gain_inner.FracRectVertical(0.5, true);
    const IRECT gain_bottom_row = gain_inner.FracRectVertical(0.5);
    const IRECT gain_label = gain_top_row.FracRectHorizontal(1. / 3.);
    const IRECT gain_mid = gain_top_row.GetReducedFromLeft(gain_label.W()).FracRectHorizontal(0.5);
    const IRECT gain_side = gain_top_row.FracRectHorizontal(1. / 3., true);
    const IRECT gain_left = gain_bottom_row.FracRectHorizontal(1. / 3.);
    const IRECT gain_master = gain_bottom_row.GetReducedFromLeft(gain_left.W()).FracRectHorizontal(0.5);
    const IRECT gain_right = gain_bottom_row.FracRectHorizontal(1. / 3., true);

    const IVStyle gain_style = IVStyle::IVStyle();

    pGraphics->AttachControl(new ITextControl(gain_label.GetPadded(-8.), "GAIN", IText(24.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new IVKnobControl(gain_mid.GetPadded(-8.), kGainMid, "", gain_style));
    pGraphics->AttachControl(new IVKnobControl(gain_side.GetPadded(-8.), kGainSide, "", gain_style));
    pGraphics->AttachControl(new IVKnobControl(gain_left.GetPadded(-8.), kGainLeft, "", gain_style));
    pGraphics->AttachControl(new IVKnobControl(gain_master.GetPadded(-8.), kGainMaster, "", gain_style));
    pGraphics->AttachControl(new IVKnobControl(gain_right.GetPadded(-8.), kGainRight, "", gain_style));

    // Trim Section

    const IRECT trim_inner = trim.GetHPadded(-4.);
    const IRECT trim_top_row = trim_inner.FracRectVertical(0.5, true);
    const IRECT trim_bottom_row = trim_inner.FracRectVertical(0.5);
    const IRECT trim_side = trim_top_row.FracRectHorizontal(1. / 3.);
    const IRECT trim_mid = trim_top_row.GetReducedFromLeft(trim_side.W()).FracRectHorizontal(0.5);
    const IRECT trim_label = trim_top_row.FracRectHorizontal(1. / 3., true);
    const IRECT trim_left = trim_bottom_row.FracRectHorizontal(1. / 3.);
    const IRECT trim_master = trim_bottom_row.GetReducedFromLeft(trim_left.W()).FracRectHorizontal(0.5);
    const IRECT trim_right = trim_bottom_row.FracRectHorizontal(1. / 3., true);

    const IVStyle trim_style = IVStyle::IVStyle();

    pGraphics->AttachControl(new ITextControl(trim_label.GetPadded(-8.), "TRIM", IText(24.0, COLOR_WHITE), COLOR_TRANSPARENT));
    pGraphics->AttachControl(new IVKnobControl(trim_mid.GetPadded(-8.), kTrimMid, "", trim_style));
    pGraphics->AttachControl(new IVKnobControl(trim_master.GetPadded(-8.), kTrimMaster, "", trim_style));
    pGraphics->AttachControl(new IVKnobControl(trim_left.GetPadded(-8.), kTrimLeft, "", trim_style));
    pGraphics->AttachControl(new IVKnobControl(trim_side.GetPadded(-8.), kTrimSide, "", trim_style));
    pGraphics->AttachControl(new IVKnobControl(trim_right.GetPadded(-8.), kTrimRight, "", trim_style));
  };
#endif
}

#if IPLUG_DSP
void RCEveryGain::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double macro_shift = GetParam(kShiftMacro)->Value();
  const double micro_shift = GetParam(kShiftMicro)->Value() / 100;
  const double shift_size = GetParam(kShiftSize)->Value();
  const double fader_pct = GetParam(kFader)->Value() / 100;
  const double fader_curve = GetParam(kFaderCurve)->Value();
  const double fader_smoothing = GetParam(kFaderCurve)->Value() * 400.0;
  const double gain_master = GetParam(kGainMaster)->Value();
  const double trim_master = GetParam(kTrimMaster)->Value();
  const double gain_left = GetParam(kGainLeft)->Value();
  const double trim_left = GetParam(kTrimLeft)->Value();
  const double gain_right = GetParam(kGainRight)->Value();
  const double trim_right = GetParam(kTrimRight)->Value();
  const double gain_mid = GetParam(kGainMid)->Value();
  const double trim_mid = GetParam(kTrimMid)->Value();
  const double gain_side = GetParam(kGainSide)->Value();
  const double trim_side = GetParam(kTrimSide)->Value();

  double shift_amp = std::pow(2.0, (macro_shift + micro_shift) * shift_size);
  double fader_smoothing_inv = 1.0 / (fader_smoothing + 1.0);
  double fader_target = std::pow(2.0, std::pow(fader_pct, fader_curve)) - 1.0;
  double master_amp = DBToAmp(gain_master) * DBToAmp(trim_master);
  double left_amp = DBToAmp(gain_left) * DBToAmp(trim_left);
  double right_amp = DBToAmp(gain_right) * DBToAmp(trim_right);
  double mid_amp = DBToAmp(gain_mid) * DBToAmp(trim_mid);
  double side_amp = DBToAmp(gain_side) * DBToAmp(trim_side);

  const int nChans = NOutChansConnected();

  for (int s = 0; s < nFrames; s++)
  {
    fader_amp = ((fader_amp * fader_smoothing) + fader_target) * fader_smoothing_inv;
    double gain = shift_amp * fader_amp * master_amp;

    double spl0 = inputs[0][s];
    double spl1 = inputs[1][s];

    double center = (spl0 + spl1) * 0.5 * mid_amp;
    double side = (spl0 - spl1) * 0.5 * side_amp;

    outputs[0][s] = (center + side) * gain * left_amp;
    outputs[1][s] = (center - side) * gain * right_amp;
  }
}
#endif
