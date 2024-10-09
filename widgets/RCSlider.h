#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/DrawRect.h"
#include "widgets/RCSliderControl.h"
#include "widgets/WidgetColors.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

const IVStyle RC_DEFAULT_STYLE =
  DEFAULT_STYLE.WithShowLabel(false).WithShowValue(true).WithDrawFrame(true).WithFrameThickness(2.f).WithValueText(DEFAULT_VALUE_TEXT.WithSize(16.0).WithFGColor(COLOR_BLACK));


class RCSlider : public RCSliderControl, public IVectorBase, public DrawRect
{
public:
  RCSlider(const IRECT& bounds,
           int paramIdx = kNoParameter,
           const Color::HSLA& color = Color::HSLA{},
           DirectionType dir = DirectionType::Horizontal,
           const IVStyle& style = RC_DEFAULT_STYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  RCSlider(const IRECT& bounds,
           IActionFunction aF,
           const Color::HSLA& color = Color::HSLA{},
           DirectionType dir = DirectionType::Horizontal,
           const IVStyle& style = RC_DEFAULT_STYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  virtual ~RCSlider() {}

  virtual void Draw(IGraphics& g);
  virtual void DrawTrack(IGraphics& g, WidgetColors color);
  virtual void DrawValue(IGraphics& g, WidgetColors color);

  void SetActive(const bool active) { mActive = active; };


protected:
  DirectionType mDirectionType;
  bool mActive = false;
};

RCSlider::RCSlider(const IRECT& bounds, int paramIdx, const Color::HSLA& color, DirectionType dir, const IVStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControl(bounds, paramIdx, DrawRect::ToEDirection(dir), gearing)
  , IVectorBase(style)
  , DrawRect(color)
  , mDirectionType(dir)
{
}

RCSlider::RCSlider(const IRECT& bounds, IActionFunction aF, const Color::HSLA& color, DirectionType dir, const IVStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControl(bounds, aF, DrawRect::ToEDirection(dir), gearing)
  , IVectorBase(style)
  , DrawRect(color)
  , mDirectionType(dir)
{
}

void RCSlider::Draw(IGraphics& g)
{
  ControlState state = mMouseControl.IsLPressed() ? ControlState::kPressed : mMouseControl.IsHovering() ? ControlState::kHovered : ControlState::kNormal;
  WidgetColors color = GetColors(state);
  DrawTrack(g, color);
  DrawValue(g, color);
}

void RCSlider::DrawTrack(IGraphics& g, WidgetColors color)
{
  const float border_width = mStyle.drawFrame ? mStyle.frameThickness : 0.f;
  if (border_width)
  {
    const IRECT borderBounds = mRECT.GetPadded(-(border_width * .5f));
    g.DrawRect(color.GetBorderColor(), borderBounds, &mBlend, border_width);
  }

  const IRECT contentBounds = mRECT.GetPadded(-border_width);
  IRECT valueBounds;
  EDirection fracDirection;
  const double pct = GetValue();
  switch (mDirectionType)
  {
  case DirectionType::Horizontal:
  case DirectionType::Vertical:
    fracDirection = (mDirectionType == DirectionType::Horizontal) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, pct);
    break;
  case DirectionType::HorizontalSplit:
  case DirectionType::VerticalSplit:
    fracDirection = (mDirectionType == DirectionType::HorizontalSplit) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, 0.5, pct > .5).FracRect(fracDirection, abs(0.5 - pct) * 2., pct < .5);
    break;
  case DirectionType::Filled:
    valueBounds = contentBounds;
    break;
  }
  g.FillRect(color.GetColor(), valueBounds, &mBlend);
}

void RCSlider::DrawValue(IGraphics& g, WidgetColors color)
{
  if (!mStyle.showValue)
    return;

  mStyle.valueText.mFGColor = color.GetLabelColor();
  g.DrawText(mStyle.valueText, mValueStr.Get(), mRECT, &mBlend);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
