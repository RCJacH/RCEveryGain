#include "RCEveryGain.h"
#include "IControls.h"
#include "IPlug_include_in_plug_src.h"
#include "Widgets/Color.h"
#include "Widgets/RCLabel.h"
#include "Widgets/RCSlider.h"
#include "Widgets/RCStyle.h"

double shift_sizes[10] = {0.25, 0.5, 0.75, 1., 1.25, 1.5, 1.75, 2., 2.5, 3.};

RCEveryGain::RCEveryGain(const InstanceInfo& info)
  : iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  fader_amp = 0.0;

  GetParam(kShiftMacro)->InitInt("Macro Shift", 0, -8, 8, "Bit");
  GetParam(kShiftMicro)->InitDouble("Micro Shift", 0., -100.0, 100.0, 0.01, "%");
  GetParam(kShiftSize)->InitEnum("Shift Size", 3, {"0.25", "0.5", "0.75", "1.0", "1.25", "1.5", "1.75", "2.0", "2.5", "3.0"});
  GetParam(kFader)->InitDouble("Fader", 100., 0., 100.0, 0.0001, "%");
  GetParam(kFaderCurve)->InitDouble("Fader Curve", 2., 0., 10.0, 0.1, "");
  GetParam(kFaderSmoothing)->InitDouble("Fader Smoothing", 2., 0., 10.0, 0.1, "");
  GetParam(kGainMaster)->InitDouble("Master Gain", 0., -12., 12.0, 0.1, "dB");
  GetParam(kGainLeft)->InitDouble("Left Gain", 0., -12., 12.0, 0.1, "dB");
  GetParam(kGainRight)->InitDouble("Right Gain", 0., -12., 12.0, 0.1, "dB");
  GetParam(kGainMid)->InitDouble("Mid Gain", 0., -12., 12.0, 0.1, "dB");
  GetParam(kGainSide)->InitDouble("Side Gain", 0., -12., 12.0, 0.1, "dB");
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
    // const IBitmap backgroundBitmap = pGraphics->LoadBitmap(PNGBACKGROUND_FN);
    // pGraphics->AttachBackground(PNGBACKGROUND_FN);
    const IColor bgIColor = IColor::FromHSLA(.5972f, .12f, .3f);
    const Color::HSLA main_color = Color::HSLA(214, .32f, .3f);
    const RCStyle main_style = DEFAULT_RCSTYLE.WithColor(main_color);
    const Color::HSLA header_color = main_color.Scaled(0, -.8, .6);
    const RCStyle header_style = main_style.WithColor(header_color).WithDrawFrame(false);

    // General Layout
    const IRECT content = pGraphics->GetBounds();
    const IRECT header = content.FracRectVertical(0.382f * 0.2f, true);
    const IRECT controls = content.FracRectHorizontal(0.618f + 0.382f * 0.382f).GetReducedFromTop(header.H());
    const IRECT meter = content.GetReducedFromTop(header.H()).GetReducedFromLeft(controls.W());
    const IRECT shift = controls.FracRectVertical(0.3f, true);
    const IRECT lower = controls.GetReducedFromTop(shift.H());
    const IRECT volume = lower.FracRectHorizontal(0.618f, true);
    const IRECT fader = lower.GetReducedFromRight(volume.W());
    const IRECT gain = volume.SubRectVertical(2, 0);
    const IRECT trim = volume.SubRectVertical(2, 1);


    const IRECT meter_screens = meter.GetPadded(-4.f - 12.f * 1.5f).GetReducedFromBottom(24.f).GetReducedFromLeft(2.f);
    const IRECT input_meter = meter_screens.GetReducedFromRight((meter_screens.W() * .5f) - 4.f);
    const IRECT output_meter = meter_screens.GetReducedFromLeft((meter_screens.W() * .5f) - 4.f);
    const IVStyle meter_style = DEFAULT_STYLE.WithLabelText(DEFAULT_LABEL_TEXT.WithSize(20.0).WithFGColor(COLOR_WHITE))
                                  .WithShowLabel(false)
                                  .WithValueText(DEFAULT_VALUE_TEXT.WithSize(16.0).WithFGColor(COLOR_WHITE))
                                  .WithColor(kFG, COLOR_WHITE.WithOpacity(0.3f));
    pGraphics->AttachControl(new IVPeakAvgMeterControl<2>(input_meter, "Inputs", meter_style), kCtrlTagInputMeter);
    pGraphics->AttachControl(new IVPeakAvgMeterControl<2>(output_meter, "Outputs", meter_style), kCtrlTagOutputMeter);

    // Shift Section
    const IRECT shift_header = shift.GetFromLeft(24.f);
    const IRECT shift_inner = shift.GetReducedFromLeft(24.f).GetVPadded(-6.f - 6.f * 1.6f);
    const IRECT shift_lane_macro = shift_inner.SubRectVertical(3, 0).GetReducedFromBottom(2.f);
    const IRECT shift_lane_micro = shift_inner.SubRectVertical(3, 1).GetVPadded(-2.f);
    const IRECT shift_lane_size = shift_inner.SubRectVertical(3, 2).GetReducedFromTop(2.f);
    const IRECT shift_control_macro = shift_lane_macro.FracRectHorizontal(.88f);
    const IRECT shift_control_micro = shift_lane_micro.FracRectHorizontal(.88f);
    const IRECT shift_control_size = shift_lane_size.FracRectHorizontal(.88f);
    const IRECT shift_label_macro = shift_lane_macro.FracRectHorizontal(.12f, true);
    const IRECT shift_label_micro = shift_lane_micro.FracRectHorizontal(.12f, true);
    const IRECT shift_label_size = shift_lane_size.FracRectHorizontal(.12f, true);
    const IRECT shift_bulbs_macro = shift_control_macro.GetHPadded(-shift_control_macro.H());
    const IRECT shift_macro_minus_button = shift_control_macro.GetFromLeft(shift_control_macro.H());
    const IRECT shift_macro_plus_button = shift_control_macro.GetFromRight(shift_control_macro.H());
    const Color::HSLA shift_color = Color::HSLA(0, .5f, .5f);
    const RCStyle shift_style = main_style.WithValueTextSize(20.f).WithColor(shift_color);
    const RCStyle shift_label_style = header_style;

    pGraphics->EnableMouseOver(true);
    pGraphics->AttachControl(new RCLabel(shift_header, "SHIFT", EDirection::Vertical, header_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(shift_label_macro, "MACRO", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(shift_label_micro, "MICRO", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(shift_label_size, "SIZE", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCSlider(shift_control_macro, kShiftMacro, "", RCSlider::HorizontalSplit, shift_style));
    pGraphics->AttachControl(new RCSlider(shift_control_micro, kShiftMicro, "", RCSlider::HorizontalSplit, shift_style));
    pGraphics->AttachControl(new RCSlider(shift_control_size, kShiftSize, "", RCSlider::Horizontal, shift_style));

    // const IBitmap shiftSizeSwitchOnPNG = pGraphics->LoadBitmap(PNGSHIFTSIZESWITCHON_FN);
    // const IBitmap shiftSizeSwitchOffPNG = pGraphics->LoadBitmap(PNGSHIFTSIZESWITCHOFF_FN);
    // pGraphics->AttachControl(new PNGTabSwitchControl(shift_control_size, kShiftSize, shiftSizeSwitchOffPNG, shiftSizeSwitchOnPNG, {}, "", 0.f, shift_switch_style, shift_button_text_offset));

    // Fader Section
    const IRECT fader_header = fader.GetFromLeft(24.f);
    const IRECT fader_inner = fader.GetReducedFromLeft(24.f).GetVPadded(-2.f).GetReducedFromTop(4.f);
    const IRECT fader_non_fader = fader_inner.FracRectHorizontal(.618f, true);
    const IRECT fader_fader_lane = fader_inner.GetReducedFromRight(fader_non_fader.W() + 2.f);
    const IRECT fader_curve_lane = fader_non_fader.SubRectHorizontal(2, 0).GetHPadded(-2.f);
    const IRECT fader_smooth_lane = fader_non_fader.SubRectHorizontal(2, 1).GetReducedFromLeft(2.f);
    const IRECT fader_control_fader = fader_fader_lane.FracRectVertical(.88f, true);
    const IRECT fader_control_curve = fader_curve_lane.FracRectVertical(.88f, true);
    const IRECT fader_control_smooth = fader_smooth_lane.FracRectVertical(.88f, true);
    const IRECT fader_label_fader = fader_fader_lane.FracRectVertical(.12f);
    const IRECT fader_label_curve = fader_curve_lane.FracRectVertical(.12f);
    const IRECT fader_label_smooth = fader_smooth_lane.FracRectVertical(.12f);

    pGraphics->AttachControl(new RCLabel(fader_header, "FADER", EDirection::Vertical, header_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(fader_label_fader, "VOLUME", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(fader_label_curve, "CURVE", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(fader_label_smooth, "SMOOTH", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCSlider(fader_control_fader, kFader, "", RCSlider::Vertical, shift_style));
    pGraphics->AttachControl(new RCSlider(fader_control_curve, kFaderCurve, "", RCSlider::Vertical, shift_style));
    pGraphics->AttachControl(new RCSlider(fader_control_smooth, kFaderSmoothing, "", RCSlider::Vertical, shift_style));

    // Gain Section

    // const IRECT gain_inner = gain.GetHPadded(-4.);
    // const IRECT gain_label = gain_inner.GetGridCell(0, 2, 3);
    // const IRECT gain_mid = gain_inner.GetGridCell(1, 2, 3);
    // const IRECT gain_side = gain_inner.GetGridCell(2, 2, 3);
    // const IRECT gain_left = gain_inner.GetGridCell(3, 2, 3);
    // const IRECT gain_master = gain_inner.GetGridCell(4, 2, 3);
    // const IRECT gain_right = gain_inner.GetGridCell(5, 2, 3);

    // const IVStyle gain_style = DEFAULT_STYLE.WithLabelText(DEFAULT_LABEL_TEXT.WithSize(16.0).WithFGColor(COLOR_WHITE)).WithValueText(DEFAULT_VALUE_TEXT.WithSize(12.0).WithFGColor(COLOR_WHITE));
    // pGraphics->AttachControl(new IVKnobControl(gain_mid.GetPadded(-8.), kGainMid, "", gain_style));
    // pGraphics->AttachControl(new IVKnobControl(gain_side.GetPadded(-8.), kGainSide, "", gain_style));
    // pGraphics->AttachControl(new IVKnobControl(gain_left.GetPadded(-8.), kGainLeft, "", gain_style));
    // pGraphics->AttachControl(new IVKnobControl(gain_master.GetPadded(-8.), kGainMaster, "", gain_style));
    // pGraphics->AttachControl(new IVKnobControl(gain_right.GetPadded(-8.), kGainRight, "", gain_style));

    // Trim Section

    // const IRECT trim_inner = trim.GetHPadded(-4.);
    // const IRECT trim_side = trim_inner.GetGridCell(0, 2, 3);
    // const IRECT trim_mid = trim_inner.GetGridCell(1, 2, 3);
    // const IRECT trim_label = trim_inner.GetGridCell(2, 2, 3);
    // const IRECT trim_left = trim_inner.GetGridCell(3, 2, 3);
    // const IRECT trim_master = trim_inner.GetGridCell(4, 2, 3);
    // const IRECT trim_right = trim_inner.GetGridCell(5, 2, 3);

    // const IVStyle trim_style = DEFAULT_STYLE.WithLabelText(DEFAULT_LABEL_TEXT.WithSize(16.0).WithFGColor(COLOR_WHITE)).WithValueText(DEFAULT_VALUE_TEXT.WithSize(12.0).WithFGColor(COLOR_WHITE));
    // pGraphics->AttachControl(new IVKnobControl(trim_mid.GetPadded(-8.), kTrimMid, "", trim_style));
    // pGraphics->AttachControl(new IVKnobControl(trim_master.GetPadded(-8.), kTrimMaster, "", trim_style));
    // pGraphics->AttachControl(new IVKnobControl(trim_left.GetPadded(-8.), kTrimLeft, "", trim_style));
    // pGraphics->AttachControl(new IVKnobControl(trim_side.GetPadded(-8.), kTrimSide, "", trim_style));
    // pGraphics->AttachControl(new IVKnobControl(trim_right.GetPadded(-8.), kTrimRight, "", trim_style));
  };
#endif
}

#if IPLUG_DSP
void RCEveryGain::OnIdle()
{
  mInputPeakSender.TransmitData(*this);
  mOutputPeakSender.TransmitData(*this);
}

void RCEveryGain::OnParamChange(int idx)
{
  auto value = GetParam(idx)->Value();
  if (idx == kShiftSize)
  {
    shift_size = shift_sizes[(int)value];
  }
}

void RCEveryGain::OnReset()
{
  mInputPeakSender.Reset(GetSampleRate());
  mOutputPeakSender.Reset(GetSampleRate());
}

void RCEveryGain::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double macro_shift = GetParam(kShiftMacro)->Value();
  const double micro_shift = GetParam(kShiftMicro)->Value() / 100;
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

  mInputPeakSender.ProcessBlock(inputs, nFrames, kCtrlTagInputMeter, 2, 0);
  mOutputPeakSender.ProcessBlock(outputs, nFrames, kCtrlTagOutputMeter, 2, 0);
}
#endif
