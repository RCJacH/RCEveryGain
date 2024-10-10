#pragma once

#include "IGraphicsConstants.h"
#include "widgets/Color.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

struct WidgetColors
{
  WidgetColors() {};

  WidgetColors(Color::HSLA color)
    : mainColor(color)
    , borderColor(color.Scaled(0., -.1f, .2f))
    , labelColor(color.Scaled(0., -.1f, .5f)) {};

  Color::HSLA mainColor;
  Color::HSLA borderColor;
  Color::HSLA labelColor;

  IColor WidgetColors::GetColor() { return mainColor.AsIColor(); }
  IColor WidgetColors::GetBorderColor() { return borderColor.AsIColor(); }
  IColor WidgetColors::GetLabelColor() { return labelColor.AsIColor(); }

  WidgetColors WidgetColors::HoverColors() { return WidgetColors(mainColor.Adjusted(0, -.05f, .05f)); }
  WidgetColors WidgetColors::PressColors() { return WidgetColors(mainColor.Adjusted(0, -.1f, .1f)); }
  WidgetColors WidgetColors::DisabledColors() { return WidgetColors(mainColor.Adjusted(-90, -.3f, -.2f)); }
};

struct RCStyle
{
  bool hideCursor = DEFAULT_HIDE_CURSOR;
  bool showValue = DEFAULT_SHOW_VALUE;
  bool drawFrame = DEFAULT_DRAW_FRAME;
  Color::HSLA baseColor = Color::HSLA();
  IText valueText = DEFAULT_VALUE_TEXT.WithVAlign(EVAlign::Middle);
  float frameThickness = 2.f;
  WidgetColors mColors;
  WidgetColors mHoverColors;
  WidgetColors mPressColors;
  WidgetColors mDisabledColors;

  /** Create a new RCStyle to configure common styling for IVControls
   * @param showValue Display value text
   * @param baseColors A HSLA base color for the style
   * @param valueText The IText for the value text style
   * @param hideCursor Should the cursor be hidden e.g. when dragging the control
   * @param drawFrame Should the frame be drawn around the bounds of the control or around the handle, where relevant
   * @param frameThickness The thickness of the controls frame elements */
  RCStyle(const Color::HSLA baseColor = Color::HSLA(),
          float valueTextSize = DEFAULT_TEXT_SIZE,
          const char* fontID = nullptr,
          bool hideCursor = DEFAULT_HIDE_CURSOR,
          bool showValue = DEFAULT_SHOW_VALUE,
          bool drawFrame = DEFAULT_DRAW_FRAME,
          float frameThickness = DEFAULT_FRAME_THICKNESS)
    : hideCursor(hideCursor)
    , showValue(showValue)
    , drawFrame(drawFrame)
    , baseColor(baseColor)
    , mColors(WidgetColors(baseColor))
    , mHoverColors(mColors.HoverColors())
    , mPressColors(mColors.PressColors())
    , mDisabledColors(mColors.DisabledColors())
  {
    valueText = valueText.WithFont(fontID).WithSize(valueTextSize);
  }

  RCStyle WithShowValue(bool show = true) const
  {
    RCStyle newStyle = *this;
    newStyle.showValue = show;
    return newStyle;
  }
  RCStyle WithValueTextSize(float newSize) const
  {
    RCStyle newStyle = *this;
    newStyle.valueText = valueText.WithSize(newSize);
    return newStyle;
  }
  RCStyle WithHideCursor(bool hide = true) const
  {
    RCStyle newStyle = *this;
    newStyle.hideCursor = hide;
    return newStyle;
  }
  RCStyle WithColor(Color::HSLA newColor) const
  {
    RCStyle newStyle = *this;
    newStyle.mColors = WidgetColors(newColor);
    newStyle.mHoverColors = newStyle.mColors.HoverColors();
    newStyle.mPressColors = newStyle.mColors.PressColors();
    newStyle.mDisabledColors = newStyle.mColors.DisabledColors();
    return newStyle;
  }
  RCStyle WithFrameThickness(float v) const
  {
    RCStyle newStyle = *this;
    newStyle.frameThickness = v;
    return newStyle;
  }
  RCStyle WithDrawFrame(bool v = true) const
  {
    RCStyle newStyle = *this;
    newStyle.drawFrame = v;
    return newStyle;
  }

  IText GetText() const { return valueText; }

  WidgetColors GetColors(bool isHovered = false, bool isDown = false, bool isDisabled = false)
  {
    if (isDisabled)
      return mDisabledColors;
    if (isDown)
      return mPressColors;
    if (isHovered)
      return mHoverColors;
    return mColors;
  };
};

const RCStyle DEFAULT_RCSTYLE = RCStyle();

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
