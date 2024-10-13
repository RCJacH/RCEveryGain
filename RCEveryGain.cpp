#include "RCEveryGain.h"
#include "IControls.h"
#include "IPlug_include_in_plug_src.h"
#include "Widgets/Color.h"
#include "Widgets/RCLabel.h"
#include "Widgets/RCSlider.h"
#include "Widgets/RCStyle.h"
#include "widgets/RCMeterControl.h"
#include "widgets/RCTabSwitchControl.h"

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
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    pGraphics->LoadFont("FiraSans-Regular", FIRASANS_REGULAR_FN);
    pGraphics->LoadFont("FiraSans-Medium", FIRASANS_MEDIUM_FN);
    const Color::HSLA main_color = Color::HSLA(213, .36f, .32f);
    const RCStyle main_style = DEFAULT_RCSTYLE.WithColor(main_color).WithValueTextSize(18.f).WithValueTextFont("FiraSans-Regular");
    const Color::HSLA header_color = main_color.Scaled(0, -.8f, .3f);
    const RCStyle header_style = main_style.WithColor(header_color).WithDrawFrame(false).WithValueTextSize(16.f).WithValueTextFont("FiraSans-Medium");
    pGraphics->AttachPanelBackground(main_color.Scaled(0, -.55f, -.05f).AsIColor());

    auto AddPanelBG = [&](const IRECT bounds, const Color::HSLA color) { pGraphics->AttachControl(new IPanelControl(bounds, color.Scaled(0, -.6f, -.05f).AsIColor())); };
    auto GetSectionColor = [&](int dHue) { return main_color.Adjusted(dHue, 0.f, .05f); };
    auto GetSectionLabelColor = [&](const Color::HSLA color) { return color.Scaled(0, -.8f, .6f); };

    // General Layout
    const IRECT content = pGraphics->GetBounds();
    const IRECT header = content.FracRectVertical(0.382f * 0.382 * 0.618f, true);
    const IRECT controls = content.FracRectHorizontal(0.618f + 0.382f * 0.382f).GetReducedFromTop(header.H());
    const IRECT meter = content.GetReducedFromTop(header.H()).GetReducedFromLeft(controls.W());
    const IRECT shift = controls.FracRectVertical(0.3f, true);
    const IRECT lower = controls.GetReducedFromTop(shift.H());
    const IRECT volume = lower.FracRectHorizontal(0.618f, true);
    const IRECT fader = lower.GetReducedFromRight(volume.W());
    const IRECT gain = volume.SubRectVertical(2, 0);
    const IRECT trim = volume.SubRectVertical(2, 1);

    // TITLE Section
    const IBitmap titleBitmap = pGraphics->LoadBitmap(PNGTITLE_FN);
    pGraphics->AttachControl(
      new IBButtonControl(header.GetReducedFromLeft(24.f).GetReducedFromTop(8.f).GetFromLeft(titleBitmap.W()).GetMidVPadded(titleBitmap.H()), titleBitmap, [](IControl* pCaller) {
        pCaller->SetAnimation(
          [](IControl* pCaller) {
            auto progress = pCaller->GetAnimationProgress();
            if (progress > 1.)
            {
              pCaller->OnEndAnimation();
              return;
            }
            pCaller->SetValue(Clip(progress + .5, 0., 1.));
          },
          100);
      }));

    // Meter Section
    const IRECT meter_content = meter.GetHPadded(-8.f).GetReducedFromTop(8.f);
    const IRECT meter_input_lane = meter_content.SubRectHorizontal(2, 0).GetReducedFromRight(8.f);
    const IRECT meter_output_lane = meter_content.SubRectHorizontal(2, 1).GetReducedFromLeft(8.f);
    const IRECT meter_input_label = meter_input_lane.GetFromBottom(24.f);
    const IRECT meter_output_label = meter_output_lane.GetFromBottom(24.f);
    const IRECT meter_input = meter_input_lane.GetReducedFromBottom(24.f);
    const IRECT meter_output = meter_output_lane.GetReducedFromBottom(24.f);
    const Color::HSLA meter_color = main_color.Scaled(0, -.5f, -.25f);
    const RCStyle meter_rcstyle = main_style.WithColor(meter_color);
    const RCStyle meter_label_style = header_style.WithColor(GetSectionLabelColor(meter_color));
    const auto meter_colors = meter_rcstyle.GetColors(false, true);

    AddPanelBG(meter, meter_color);
    const IRECT meter_screens = meter.GetPadded(-4.f - 12.f * 1.5f).GetReducedFromBottom(24.f).GetReducedFromLeft(2.f);
    const IVStyle meter_style = DEFAULT_STYLE.WithShowLabel(false)
                                  .WithValueText(meter_rcstyle.valueText.WithFGColor(meter_rcstyle.GetColors(false, false).GetLabelColor().WithContrast(-.16f)))
                                  .WithColor(kX1, meter_rcstyle.GetColors().GetLabelColor())
                                  .WithColor(kX2, meter_rcstyle.GetColors().labelColor.Scaled(0, .8f).WithHue(0).AsIColor())
                                  .WithColor(kHL, meter_rcstyle.GetColors(false, false, true).GetBorderColor().WithOpacity(.25f))
                                  .WithColor(kFG, meter_rcstyle.GetColors(false, true).GetLabelColor())
                                  .WithColor(kFR, meter_rcstyle.GetColors(false, true).GetBorderColor());
    pGraphics->AttachControl(new RCLabel(meter_input_label, "INPUTS", EDirection::Horizontal, meter_label_style));
    pGraphics->AttachControl(new RCLabel(meter_output_label, "OUTPUTS", EDirection::Horizontal, meter_label_style));
    pGraphics->AttachControl(new RCPeakAvgMeterControl<2>(meter_input, meter_style), kCtrlTagInputMeter);
    pGraphics->AttachControl(new RCPeakAvgMeterControl<2>(meter_output, meter_style), kCtrlTagOutputMeter);

    // Shift Section
    const IRECT shift_header = shift.GetFromLeft(24.f);
    const IRECT shift_inner = shift.GetReducedFromLeft(24.f).GetVPadded(-8.f);
    const IRECT shift_lane_macro = shift_inner.SubRectVertical(3, 0).GetReducedFromBottom(2.f);
    const IRECT shift_lane_micro = shift_inner.SubRectVertical(3, 1).GetVPadded(-2.f);
    const IRECT shift_lane_size = shift_inner.SubRectVertical(3, 2).GetReducedFromTop(2.f);
    const IRECT shift_control_macro = shift_lane_macro.FracRectHorizontal(.88f);
    const IRECT shift_control_micro = shift_lane_micro.FracRectHorizontal(.88f);
    const IRECT shift_control_size = shift_lane_size.FracRectHorizontal(.88f);
    const IRECT shift_label_macro = shift_lane_macro.FracRectHorizontal(.1f, true);
    const IRECT shift_label_micro = shift_lane_micro.FracRectHorizontal(.1f, true);
    const IRECT shift_label_size = shift_lane_size.FracRectHorizontal(.1f, true);
    const IRECT shift_bulbs_macro = shift_control_macro.GetHPadded(-shift_control_macro.H());
    const IRECT shift_macro_minus_button = shift_control_macro.GetFromLeft(shift_control_macro.H());
    const IRECT shift_macro_plus_button = shift_control_macro.GetFromRight(shift_control_macro.H());
    const Color::HSLA shift_color = GetSectionColor(120);
    const RCStyle shift_style = main_style.WithColor(shift_color);
    const RCStyle shift_header_style = header_style.WithColor(GetSectionLabelColor(shift_color));
    const RCStyle shift_label_style = shift_header_style.WithValueTextHAlign(EAlign::Near);

    pGraphics->EnableMouseOver(true);
    AddPanelBG(shift, shift_color);
    pGraphics->AttachControl(new RCLabel(shift_header, "SHIFT", EDirection::Vertical, shift_header_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(shift_label_macro, "MACRO", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(shift_label_micro, "MICRO", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCLabel(shift_label_size, "SIZE", EDirection::Horizontal, shift_label_style));
    pGraphics->AttachControl(new RCSlider(shift_control_macro, kShiftMacro, "", RCSlider::HorizontalSplit, shift_style));
    pGraphics->AttachControl(new RCSlider(shift_control_micro, kShiftMicro, "", RCSlider::HorizontalSplit, shift_style));
    pGraphics->AttachControl(new RCTabSwitchControl(shift_control_size, kShiftSize, {}, "", shift_style.WithValueTextFont("FiraSans-Medium")));

    // Fader Section
    const IRECT fader_header = fader.GetFromLeft(24.f);
    const IRECT fader_inner = fader.GetReducedFromLeft(24.f).GetVPadded(-2.f).GetReducedFromTop(8.f).GetReducedFromRight(8.f);
    const IRECT fader_non_fader = fader_inner.FracRectHorizontal(.382f, true);
    const IRECT fader_fader_lane = fader_inner.GetReducedFromRight(fader_non_fader.W() + 2.f);
    const IRECT fader_curve_lane = fader_non_fader.GetReducedFromLeft(2.f).SubRectVertical(2, 0);
    const IRECT fader_smooth_lane = fader_non_fader.GetReducedFromLeft(2.f).SubRectVertical(2, 1);
    const IRECT fader_fader_label = fader_fader_lane.GetFromBottom(24.f);
    const IRECT fader_curve_label = fader_curve_lane.GetFromBottom(24.f);
    const IRECT fader_smooth_label = fader_smooth_lane.GetFromBottom(24.f);
    const IRECT fader_fader_control = fader_fader_lane.GetReducedFromBottom(24.f);
    const IRECT fader_curve_control = fader_curve_lane.GetReducedFromBottom(24.f);
    const IRECT fader_smooth_control = fader_smooth_lane.GetReducedFromBottom(24.f);
    const Color::HSLA fader_color = GetSectionColor(-90);
    const RCStyle fader_style = main_style.WithColor(fader_color);
    const RCStyle fader_label_style = header_style.WithColor(GetSectionLabelColor(fader_color));

    AddPanelBG(fader, fader_color);
    pGraphics->AttachControl(new RCLabel(fader_header, "FADER", EDirection::Vertical, fader_label_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(fader_fader_label, "VOLUME", EDirection::Horizontal, fader_label_style));
    pGraphics->AttachControl(new RCLabel(fader_curve_label, "CURVE", EDirection::Horizontal, fader_label_style));
    pGraphics->AttachControl(new RCLabel(fader_smooth_label, "SMOOTH", EDirection::Horizontal, fader_label_style));
    pGraphics->AttachControl(new RCSlider(fader_fader_control, kFader, "", RCSlider::Vertical, fader_style));
    pGraphics->AttachControl(new RCSlider(fader_curve_control, kFaderCurve, "", RCSlider::Vertical, fader_style));
    pGraphics->AttachControl(new RCSlider(fader_smooth_control, kFaderSmoothing, "", RCSlider::Vertical, fader_style));

    // Gain Section

    const IRECT gain_header = gain.GetFromRight(24.f);
    const IRECT gain_inner = gain.GetReducedFromRight(24.f).GetVPadded(-8.f);
    const IRECT gain_l_lane = gain_inner.SubRectVertical(5, 0).GetVPadded(-2.f);
    const IRECT gain_r_lane = gain_inner.SubRectVertical(5, 1).GetVPadded(-2.f);
    const IRECT gain_m_lane = gain_inner.SubRectVertical(5, 2).GetVPadded(-2.f);
    const IRECT gain_s_lane = gain_inner.SubRectVertical(5, 3).GetVPadded(-2.f);
    const IRECT gain_g_lane = gain_inner.SubRectVertical(5, 4).GetVPadded(-2.f);
    const IRECT gain_l_control = gain_l_lane.GetReducedFromLeft(24.f);
    const IRECT gain_r_control = gain_r_lane.GetReducedFromLeft(24.f);
    const IRECT gain_m_control = gain_m_lane.GetReducedFromLeft(24.f);
    const IRECT gain_s_control = gain_s_lane.GetReducedFromLeft(24.f);
    const IRECT gain_g_control = gain_g_lane.GetReducedFromLeft(24.f);
    const IRECT gain_l_label = gain_l_lane.GetFromLeft(24.f);
    const IRECT gain_r_label = gain_r_lane.GetFromLeft(24.f);
    const IRECT gain_m_label = gain_m_lane.GetFromLeft(24.f);
    const IRECT gain_s_label = gain_s_lane.GetFromLeft(24.f);
    const IRECT gain_g_label = gain_g_lane.GetFromLeft(24.f);
    const Color::HSLA gain_color = GetSectionColor(150);
    const RCStyle gain_style = main_style.WithColor(gain_color);
    const RCStyle gain_label_style = header_style.WithColor(GetSectionLabelColor(gain_color));

    AddPanelBG(gain, gain_color);
    pGraphics->AttachControl(new RCLabel(gain_header, "GAIN", EDirection::Vertical, gain_label_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(gain_l_label, "L", EDirection::Horizontal, gain_label_style));
    pGraphics->AttachControl(new RCLabel(gain_r_label, "R", EDirection::Horizontal, gain_label_style));
    pGraphics->AttachControl(new RCLabel(gain_m_label, "M", EDirection::Horizontal, gain_label_style));
    pGraphics->AttachControl(new RCLabel(gain_s_label, "S", EDirection::Horizontal, gain_label_style));
    pGraphics->AttachControl(new RCLabel(gain_g_label, "G", EDirection::Horizontal, gain_label_style));
    pGraphics->AttachControl(new RCSlider(gain_l_control, kGainLeft, "", RCSlider::HorizontalSplit, gain_style));
    pGraphics->AttachControl(new RCSlider(gain_r_control, kGainRight, "", RCSlider::HorizontalSplit, gain_style));
    pGraphics->AttachControl(new RCSlider(gain_m_control, kGainMid, "", RCSlider::HorizontalSplit, gain_style));
    pGraphics->AttachControl(new RCSlider(gain_s_control, kGainSide, "", RCSlider::HorizontalSplit, gain_style));
    pGraphics->AttachControl(new RCSlider(gain_g_control, kGainMaster, "", RCSlider::HorizontalSplit, gain_style));

    // Trim Section

    const IRECT trim_header = trim.GetFromLeft(24.f);
    const IRECT trim_inner = trim.GetReducedFromLeft(24.f).GetVPadded(-8.f);
    const IRECT trim_l_lane = trim_inner.SubRectVertical(5, 0).GetVPadded(-2.f);
    const IRECT trim_r_lane = trim_inner.SubRectVertical(5, 1).GetVPadded(-2.f);
    const IRECT trim_m_lane = trim_inner.SubRectVertical(5, 2).GetVPadded(-2.f);
    const IRECT trim_s_lane = trim_inner.SubRectVertical(5, 3).GetVPadded(-2.f);
    const IRECT trim_g_lane = trim_inner.SubRectVertical(5, 4).GetVPadded(-2.f);
    const IRECT trim_l_control = trim_l_lane.GetReducedFromRight(24.f);
    const IRECT trim_r_control = trim_r_lane.GetReducedFromRight(24.f);
    const IRECT trim_m_control = trim_m_lane.GetReducedFromRight(24.f);
    const IRECT trim_s_control = trim_s_lane.GetReducedFromRight(24.f);
    const IRECT trim_g_control = trim_g_lane.GetReducedFromRight(24.f);
    const IRECT trim_l_label = trim_l_lane.GetFromRight(24.f);
    const IRECT trim_r_label = trim_r_lane.GetFromRight(24.f);
    const IRECT trim_m_label = trim_m_lane.GetFromRight(24.f);
    const IRECT trim_s_label = trim_s_lane.GetFromRight(24.f);
    const IRECT trim_g_label = trim_g_lane.GetFromRight(24.f);
    const Color::HSLA trim_color = GetSectionColor(0);
    const RCStyle trim_style = main_style.WithColor(trim_color);
    const RCStyle trim_label_style = header_style.WithColor(GetSectionLabelColor(trim_color));

    AddPanelBG(trim, trim_color);
    pGraphics->AttachControl(new RCLabel(trim_header, "TRIM", EDirection::Vertical, trim_label_style, 1.6f));
    pGraphics->AttachControl(new RCLabel(trim_l_label, "L", EDirection::Horizontal, trim_label_style));
    pGraphics->AttachControl(new RCLabel(trim_r_label, "R", EDirection::Horizontal, trim_label_style));
    pGraphics->AttachControl(new RCLabel(trim_m_label, "M", EDirection::Horizontal, trim_label_style));
    pGraphics->AttachControl(new RCLabel(trim_s_label, "S", EDirection::Horizontal, trim_label_style));
    pGraphics->AttachControl(new RCLabel(trim_g_label, "G", EDirection::Horizontal, trim_label_style));
    pGraphics->AttachControl(new RCSlider(trim_l_control, kTrimLeft, "", RCSlider::HorizontalSplit, trim_style));
    pGraphics->AttachControl(new RCSlider(trim_r_control, kTrimRight, "", RCSlider::HorizontalSplit, trim_style));
    pGraphics->AttachControl(new RCSlider(trim_m_control, kTrimMid, "", RCSlider::HorizontalSplit, trim_style));
    pGraphics->AttachControl(new RCSlider(trim_s_control, kTrimSide, "", RCSlider::HorizontalSplit, trim_style));
    pGraphics->AttachControl(new RCSlider(trim_g_control, kTrimMaster, "", RCSlider::HorizontalSplit, trim_style));
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
