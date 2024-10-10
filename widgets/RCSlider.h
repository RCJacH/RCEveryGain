#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/RCStyle.h"
#include "widgets/RCSliderControl.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCSlider : public RCSliderControl, public IVectorBase
{
public:
  enum DirectionType
  {
    Horizontal,
    HorizontalSplit,
    Vertical,
    VerticalSplit,
  };

  EDirection ToEDirection(DirectionType dir)
  {
    switch (dir)
    {
    case DirectionType::Horizontal:
    case DirectionType::HorizontalSplit:
      return EDirection::Horizontal;
    case DirectionType::Vertical:
    case DirectionType::VerticalSplit:
      return EDirection::Vertical;
    }
    return EDirection::Horizontal;
  };

  RCSlider(const IRECT& bounds,
           int paramIdx = kNoParameter,
           const char* label = "",
           const Color::HSLA& color = Color::HSLA{},
           DirectionType dir = DirectionType::Horizontal,
           const RCStyle& style = RC_DEFAULT_STYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  RCSlider(const IRECT& bounds,
           IActionFunction aF,
           const char* label = "",
           const Color::HSLA& color = Color::HSLA{},
           DirectionType dir = DirectionType::Horizontal,
           const RCStyle& style = RC_DEFAULT_STYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  virtual ~RCSlider() {}

  virtual void Draw(IGraphics& g);
  virtual void DrawWidget(IGraphics& g, WidgetColors color);
  virtual void DrawValue(IGraphics& g, WidgetColors color);

  void OnResize() override;
  void SetDirty(bool push, int valIdx = kNoValIdx) override;
  void OnInit() override;

protected:
  DirectionType mDirectionType;
  RCStyle mStyle = RC_DEFAULT_STYLE;
};

RCSlider::RCSlider(const IRECT& bounds, int paramIdx, const char* label, const Color::HSLA& color, DirectionType dir, const RCStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControl(bounds, paramIdx, ToEDirection(dir), gearing, 2.0)
  , IVectorBase(DEFAULT_STYLE)
  , mStyle(style)
  , mDirectionType(dir)
{
  DisablePrompt(!valueIsEditable);
  AttachIControl(this, label);
}

RCSlider::RCSlider(const IRECT& bounds, IActionFunction aF, const char* label, const Color::HSLA& color, DirectionType dir, const RCStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControl(bounds, aF, ToEDirection(dir), gearing, 2.0)
  , IVectorBase(DEFAULT_STYLE)
  , mStyle(style)
  , mDirectionType(dir)
{
  DisablePrompt(!valueIsEditable);
  AttachIControl(this, label);
}

void RCSlider::Draw(IGraphics& g)
{
  auto color = mStyle.GetColors(mMouseControl.IsHovering(), mMouseControl.IsLDown(), IsDisabled());
  DrawWidget(g, color);
  DrawValue(g, color);
}

void RCSlider::DrawWidget(IGraphics& g, WidgetColors color)
{
  const float border_width = mStyle.drawFrame ? mStyle.frameThickness : 0.f;
  if (border_width)
  {
    const IRECT borderBounds = mRECT.GetPadded(-(border_width * .5f));
    g.DrawRect(color.GetBorderColor(), borderBounds, &mBlend, border_width);
  }

  const IRECT contentBounds = mRECT.GetPadded(-border_width);
  IRECT valueBounds;
  IRECT handleBounds;
  EDirection fracDirection;
  const float halfHandleSize = mHandleSize * .5f;
  const double pct = GetValue();
  switch (mDirectionType)
  {
  case DirectionType::Horizontal:
  case DirectionType::Vertical:
    fracDirection = (mDirectionType == DirectionType::Horizontal) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, pct);
    handleBounds = valueBounds.FracRect(fracDirection, 0.0, true);
    break;
  case DirectionType::HorizontalSplit:
  case DirectionType::VerticalSplit:
    fracDirection = (mDirectionType == DirectionType::HorizontalSplit) ? EDirection::Horizontal : EDirection::Vertical;
    valueBounds = contentBounds.FracRect(fracDirection, 0.5, pct >= .5).FracRect(fracDirection, abs(0.5 - pct) * 2., pct < .5);
    handleBounds = valueBounds.FracRect(fracDirection, 0.0, pct >= .5);
    break;
  }
  g.FillRect(color.GetColor(), valueBounds, &mBlend);

  if (mHandleSize > 0.f && pct != 0. && pct != 1.)
  {
    switch (fracDirection)
    {
    case EDirection::Horizontal:
      handleBounds = handleBounds.GetHPadded(halfHandleSize);
      break;
    case EDirection::Vertical:
      handleBounds = handleBounds.GetVPadded(halfHandleSize);
      break;
    }
    g.FillRect(color.GetBorderColor(), handleBounds, &mBlend);
  }
}

void RCSlider::DrawValue(IGraphics& g, WidgetColors color)
{
  if (!mStyle.showValue)
    return;

  const IText& text = mStyle.GetText().WithFGColor(color.GetLabelColor());
  g.DrawText(text, mValueStr.Get(), mRECT, &mBlend);
}

void RCSlider::OnResize()
{
  SetTargetRECT(MakeRects(mRECT));
  SetDirty(false);
}

void RCSlider::SetDirty(bool push, int valIdx)
{
  RCSliderControl::SetDirty(push);

  const IParam* pParam = GetParam();

  if (pParam)
    pParam->GetDisplayWithLabel(mValueStr);
}

void RCSlider::OnInit()
{
  const IParam* pParam = GetParam();

  if (pParam)
    pParam->GetDisplayWithLabel(mValueStr);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
