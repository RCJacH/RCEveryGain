#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/WidgetColors.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class DrawRect
{
public:
  enum ControlState
  {
    kNormal,
    kHovered,
    kPressed,
    kDisabled,
  };

  enum DirectionType
  {
    Horizontal,
    HorizontalSplit,
    Vertical,
    VerticalSplit,
    Filled,
  };

  DrawRect(const Color::HSLA color);

  virtual ~DrawRect() {}

  EDirection ToEDirection(DirectionType dir);

protected:
  float mBorderWidth;
  WidgetColors mColors;
  WidgetColors mHoverColors;
  WidgetColors mPressColors;

  WidgetColors GetColors(ControlState state)
  {
    switch (state)
    {
    case ControlState::kNormal:
      return mColors;
    case ControlState::kHovered:
      return mHoverColors;
    case ControlState::kPressed:
      return mPressColors;
    }
  };
};

EDirection DrawRect::ToEDirection(DirectionType dir)
{
  switch (dir)
  {
  case DirectionType::Horizontal:
  case DirectionType::HorizontalSplit:
  case DirectionType::Filled:
    return EDirection::Horizontal;
  case DirectionType::Vertical:
  case DirectionType::VerticalSplit:
    return EDirection::Vertical;
  }
  return EDirection::Horizontal;
};

DrawRect::DrawRect(const Color::HSLA color)
  : mColors(WidgetColors(color))
  , mHoverColors(mColors.HoverColors())
  , mPressColors(mColors.PressColors()) {};


END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
