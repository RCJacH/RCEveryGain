#pragma once

#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/RCStyle.h"
#include "widgets/RCSliderControlBase.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCSlider : public RCSliderControlBase
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
           DirectionType dir = DirectionType::Horizontal,
           const RCStyle& style = DEFAULT_RCSTYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  RCSlider(const IRECT& bounds,
           IActionFunction aF,
           const char* label = "",
           DirectionType dir = DirectionType::Horizontal,
           const RCStyle& style = DEFAULT_RCSTYLE,
           bool valueIsEditable = false,
           double gearing = 1.0);

  virtual ~RCSlider() {}

  virtual void Draw(IGraphics& g);
  virtual void DrawWidget(IGraphics& g, WidgetColorSet color);
  virtual void DrawBorder(IGraphics& g, WidgetColorSet color, IRECT bounds, float borderWidth);
  virtual void DrawHandle(IGraphics& g, WidgetColorSet color, IRECT bounds, EDirection dir, double pct);
  virtual void DrawValueText(IGraphics& g, WidgetColorSet color, IRECT bounds, EDirection dir, double pct);

  void OnResize() override;
  void SetDirty(bool push, int valIdx = kNoValIdx) override;
  void OnInit() override;

protected:
  DirectionType mDirectionType;
  RCStyle mStyle = DEFAULT_RCSTYLE;
  WDL_String mValueStr;
};

RCSlider::RCSlider(const IRECT& bounds, int paramIdx, const char* label, DirectionType dir, const RCStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControlBase(bounds, paramIdx, ToEDirection(dir), gearing, 2.0)
  , mStyle(style)
  , mDirectionType(dir)
{
  DisablePrompt(!valueIsEditable);
}

RCSlider::RCSlider(const IRECT& bounds, IActionFunction aF, const char* label, DirectionType dir, const RCStyle& style, bool valueIsEditable, double gearing)
  : RCSliderControlBase(bounds, aF, ToEDirection(dir), gearing, 2.0)
  , mStyle(style)
  , mDirectionType(dir)
{
  DisablePrompt(!valueIsEditable);
}

void RCSlider::Draw(IGraphics& g)
{
  auto color = mStyle.GetColors(mMouseControl.IsHovering(), mMouseControl.IsLDown(), IsDisabled());
  DrawWidget(g, color);
}

void RCSlider::DrawWidget(IGraphics& g, WidgetColorSet color)
{
  const float borderWidth = mStyle.drawFrame ? mStyle.frameThickness : 0.f;
  const IRECT contentBounds = mRECT.GetPadded(-borderWidth);
  IRECT valueBounds;
  IRECT handleBounds;
  IRECT textBounds;
  EDirection fracDirection;
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

  DrawBorder(g, color, mRECT, borderWidth);
  g.FillRect(color.GetColor(), valueBounds, &mBlend);
  DrawHandle(g, color, handleBounds, fracDirection, pct);
  DrawValueText(g, color, contentBounds, fracDirection, pct);
}

void RCSlider::DrawBorder(IGraphics& g, WidgetColorSet color, IRECT bounds, float borderWidth)
{
  if (!borderWidth)
    return;

  const IRECT borderBounds = mRECT.GetPadded(-(borderWidth * .5f));
  g.DrawRect(color.GetBorderColor(), borderBounds, &mBlend, borderWidth);
}

void RCSlider::DrawHandle(IGraphics& g, WidgetColorSet color, IRECT bounds, EDirection dir, double pct)
{
  if (!mHandleSize || !pct || pct == 1.)
    return;

  const float halfHandleSize = mHandleSize * .5f;
  switch (dir)
  {
  case EDirection::Horizontal:
    bounds = bounds.GetHPadded(halfHandleSize);
    break;
  case EDirection::Vertical:
    bounds = bounds.GetVPadded(halfHandleSize);
    break;
  }
  g.FillRect(color.GetBorderColor(), bounds, &mBlend);
}

void RCSlider::DrawValueText(IGraphics& g, WidgetColorSet color, IRECT bounds, EDirection dir, double pct)
{
  if (!mStyle.showValue)
    return;

  switch (dir)
  {
  case EDirection::Horizontal:
    bounds = bounds.FracRectHorizontal(0.5, pct < 0.5);
    break;
  case EDirection::Vertical:
    bounds = bounds.FracRectVertical(0.5, pct < 0.5);
    break;
  }
  const IText& text = mStyle.GetText().WithFGColor(color.GetLabelColor());
  g.DrawText(text, mValueStr.Get(), bounds, &mBlend);
}

void RCSlider::OnResize()
{
  mMouseControl.ReleaseAll();
  SetDirty(false);
}

void RCSlider::SetDirty(bool push, int valIdx)
{
  RCSliderControlBase::SetDirty(push);

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
