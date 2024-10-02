#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/WidgetColors.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class DrawRect : public IVectorBase
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

  DrawRect(const Color::HSLA color, const IVStyle& style);

  virtual ~DrawRect() {}

  virtual void DrawTrack(IGraphics& g, const IRECT& rect, ControlState state, float pct, DirectionType direction);
  virtual void DrawValue(IGraphics& g, const IRECT& rect, ControlState state, DirectionType direction);

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

DrawRect::DrawRect(const Color::HSLA color, const IVStyle& style)
  : IVectorBase(style, false, true)
  , mColors(WidgetColors(color))
  , mHoverColors(mColors.HoverColors())
  , mPressColors(mColors.PressColors())
{
}

void DrawRect::DrawTrack(IGraphics& g, const IRECT& rect, ControlState state = ControlState::kNormal, float pct = 1.f, DirectionType direction = DirectionType::Horizontal)
{
  const float border_width = mStyle.drawFrame ? mStyle.frameThickness : 0.f;
  const IRECT contentBounds = rect.GetPadded(-border_width);
  WidgetColors color = GetColors(state);
  IBlend blend = mControl->GetBlend();
  if (border_width)
    g.DrawRect(color.GetBorderColor(), rect, &blend);


  IRECT valueBounds;
  switch (direction)
  {
  case DirectionType::Horizontal:
  case DirectionType::Vertical:
    const EDirection fracDirection = (direction == DirectionType::Horizontal) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, pct);
    break;
  case DirectionType::HorizontalSplit:
  case DirectionType::VerticalSplit:
    const EDirection fracDirection = (direction == DirectionType::HorizontalSplit) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, 0.5, pct > .5f).FracRect(fracDirection, pct - .5f, pct < .5f);
    break;
  case DirectionType::Filled:
    valueBounds = contentBounds;
    break;
  }
  g.FillRect(color.GetColor(), valueBounds, &blend);
};

void DrawRect::DrawValue(IGraphics& g, const IRECT& rect, ControlState state, DirectionType direction)
{
  if (!mStyle.showValue)
    return;

  WidgetColors color = GetColors(state);

  IRECT valueBounds = rect;
  IBlend blend = mControl->GetBlend();
  mStyle.valueText.mFGColor = color.GetLabelColor();
  g.DrawText(mStyle.valueText, mValueStr.Get(), mValueBounds, &blend);
};


END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
