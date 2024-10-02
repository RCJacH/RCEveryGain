#pragma once

#include "IControl.h"
#include "Widgets/Color.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

struct WidgetColors
{
  WidgetColors() {};

  WidgetColors(Color::HSLA color)
    : mainColor(color)
    , borderColor(color.Scaled(0., -.1, .2))
    , labelColor(color.Scaled(0., -.1, .5)) {};

  Color::HSLA mainColor;
  Color::HSLA borderColor;
  Color::HSLA labelColor;

  IColor WidgetColors::GetColor() { return mainColor.AsIColor(); }
  IColor WidgetColors::GetBorderColor() { return borderColor.AsIColor(); }
  IColor WidgetColors::GetLabelColor() { return labelColor.AsIColor(); }

  WidgetColors WidgetColors::HoverColors() { return WidgetColors(mainColor.Adjusted(0, -.05, .05)); }
  WidgetColors WidgetColors::PressColors() { return WidgetColors(mainColor.Adjusted(0, -.1, .1)); }
  WidgetColors WidgetColors::DisabledColors() { return WidgetColors(mainColor.Adjusted(-90, -.3, -.2)); }
};
END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
