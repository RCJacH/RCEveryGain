#pragma once

#include "IGraphicsConstants.h"
#include "widgets/Color.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

struct WidgetColorSet
{
  WidgetColorSet() {};

  WidgetColorSet(Color::HSLA color)
    : mainColor(color)
    , borderColor(color.Scaled(0., -.1f, .2f))
    , labelColor(color.Scaled(0., -.1f, color.mL >= .5f ? -.5f : .5f)) {};

  Color::HSLA mainColor;
  Color::HSLA borderColor;
  Color::HSLA labelColor;

  IColor WidgetColorSet::GetColor() { return mainColor.AsIColor(); }
  IColor WidgetColorSet::GetBorderColor() { return borderColor.AsIColor(); }
  IColor WidgetColorSet::GetLabelColor() { return labelColor.AsIColor(); }
};

struct WidgetInteractionColors
{
  WidgetInteractionColors() {};

  WidgetInteractionColors(Color::HSLA color, bool isDisabled = false)
    : normalColors(WidgetColorSet(color))
    , hoverColors(WidgetColorSet(color.Adjusted(0, -.05f, .05f)))
    , pressColors(WidgetColorSet(color.Adjusted(0, -.1f, .1f)))
  {
    if (isDisabled)
      return;

    const float add_l = color.mL >= 0.5 ? -.05f : .1f;
    const WidgetInteractionColors disabled = WidgetInteractionColors(color.Adjusted(-90, -.3f, -add_l), true);
    disabledColors = disabled.normalColors;
    disabledHoverColors = disabled.hoverColors;
    disabledPressColors = disabled.pressColors;
  };

  WidgetColorSet normalColors;
  WidgetColorSet hoverColors;
  WidgetColorSet pressColors;
  WidgetColorSet disabledColors;
  WidgetColorSet disabledHoverColors;
  WidgetColorSet disabledPressColors;

  WidgetColorSet GetColors(bool isHovered = false, bool isDown = false, bool isDisabled = false)
  {
    if (isDisabled)
    {
      if (isDown)
        return disabledPressColors;
      if (isHovered)
        return disabledHoverColors;
      return disabledColors;
    }
    if (isDown)
      return pressColors;
    if (isHovered)
      return hoverColors;
    return normalColors;
  };
};

struct WidgetColors
{
  int mColorCount;
  WidgetInteractionColors* mColors{};

  WidgetColors() {};
  WidgetColors(Color::HSLA color, int count = 1, int hueRange = 0)
    : mColorCount{count}
    , mColors{new WidgetInteractionColors[static_cast<std::size_t>(count)]{}}
  {
    int dHue = static_cast<int>(floor(hueRange / count));
    for (int i = 0; i < count; i++)
    {
      mColors[i] = WidgetInteractionColors(color.Adjusted(dHue * i));
    }
  };

  WidgetInteractionColors Get(int index = 0) { return mColors[index % mColorCount]; };
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
  // WidgetColors mColors;
  // WidgetColors mHoverColors;
  // WidgetColors mPressColors;
  // WidgetColors mDisabledColors;

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
    , mColors(baseColor)
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
  RCStyle WithValueTextFont(const char* fontID) const
  {
    RCStyle newStyle = *this;
    newStyle.valueText = valueText.WithFont(fontID);
    return newStyle;
  }
  RCStyle WithValueTextHAlign(EAlign newAlign) const
  {
    RCStyle newStyle = *this;
    newStyle.valueText = valueText.WithAlign(newAlign);
    return newStyle;
  }
  RCStyle WithValueTextVAlign(EVAlign newAlign) const
  {
    RCStyle newStyle = *this;
    newStyle.valueText = valueText.WithVAlign(newAlign);
    return newStyle;
  }
  RCStyle WithHideCursor(bool hide = true) const
  {
    RCStyle newStyle = *this;
    newStyle.hideCursor = hide;
    return newStyle;
  }
  RCStyle WithColor(Color::HSLA newColor, int count = 1, int hueRange = 0) const
  {
    RCStyle newStyle = *this;
    newStyle.mColors = WidgetColors(newColor, count, hueRange);
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

  WidgetColorSet GetColors(bool isHovered = false, bool isDown = false, bool isDisabled = false, int index = 0) { return mColors.Get(index).GetColors(isHovered, isDown, isDisabled); };
};

const RCStyle DEFAULT_RCSTYLE = RCStyle();

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
