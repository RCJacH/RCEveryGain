#pragma once

#include "IControl.h"
#include "widgets/Color.h"
#include "widgets/RCStyle.h"
#include "widgets/RCSwitchControlBase.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A vector "tab" multi switch control. Click tabs to cycle through states. */
class RCTabSwitchControl : public RCSwitchControlBase
{
public:
  enum class ETabSegment
  {
    Start,
    Mid,
    End
  };

  enum class DirectionType
  {
    Horizontal,
    HorizontalFilled,
    Vertical,
    VerticalFilled,
  };

  /** Constructs a vector tab switch control, with an action function (no parameter)
   * @param bounds The control's bounds
   * @param aF An action function to execute when a button is clicked \see IActionFunction
   * @param options An initializer list of CStrings for the button labels. The size of the list decides the number of buttons.
   * @param label The IVControl label CString
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  RCTabSwitchControl(const IRECT& bounds,
                     IActionFunction aF,
                     const std::vector<const char*>& options = {},
                     const char* label = "",
                     const RCStyle& style = DEFAULT_RCSTYLE,
                     DirectionType direction = DirectionType::Horizontal);

  /** Constructs a vector tab switch control, linked to a parameter
   * @param bounds The control's bounds
   * @param paramIdx The parameter index to link this control to
   * @param options An initializer list of CStrings for the button labels to override the parameter display text labels. Supply an empty {} list if you don't want to do that.
   * @param label The IVControl label CString
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  RCTabSwitchControl(const IRECT& bounds,
                     int paramIdx,
                     const std::vector<const char*>& options = {},
                     const char* label = "",
                     const RCStyle& style = DEFAULT_RCSTYLE,
                     DirectionType direction = DirectionType::Horizontal);

  virtual ~RCTabSwitchControl() { mTabLabels.Empty(true); }
  void Draw(IGraphics& g) override;
  void OnInit() override;

  virtual void DrawWidget(IGraphics& g);
  virtual void DrawButton(IGraphics& g, const IRECT& bounds, WidgetColorSet color, bool selected, bool pressed, bool mouseOver, ETabSegment segment, bool disabled);
  virtual void DrawBorder(IGraphics& g, WidgetColorSet color, IRECT bounds, float borderWidth);
  virtual void DrawButtonText(IGraphics& g, const IRECT& bounds, WidgetColorSet color, bool selected, bool pressed, bool mouseOver, ETabSegment segment, bool disabled, const char* textStr);

  void OnMouseOver(float x, float y, const IMouseMod& mod) override;
  void OnMouseOut() override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
  void OnResize() override;
  virtual bool IsHit(float x, float y) const override;

  /** returns the label string on the selected tab */
  const char* GetSelectedLabelStr() const;

protected:
  /** @return the index of the entry at the given point or -1 if no entry was hit */
  virtual int GetButtonForPoint(float x, float y) const;
  EDirection GetDirection() const
  {
    switch (mDirectionType)
    {
    case DirectionType::Horizontal:
    case DirectionType::HorizontalFilled:
      return EDirection::Horizontal;
    case DirectionType::Vertical:
    case DirectionType::VerticalFilled:
      return EDirection::Vertical;
    }
  };
  bool IsFilled() const
  {
    switch (mDirectionType)
    {
    case DirectionType::Horizontal:
    case DirectionType::Vertical:
      return false;
    case DirectionType::HorizontalFilled:
    case DirectionType::VerticalFilled:
      return true;
    }
  };

  DirectionType mDirectionType;
  EDirection mDirection;
  RCStyle mStyle;
  int mMouseOverButton = -1;
  int mMousePressButton = -1;
  WDL_TypedBuf<IRECT> mButtons;
  WDL_PtrList<WDL_String> mTabLabels;
};

RCTabSwitchControl::RCTabSwitchControl(const IRECT& bounds, IActionFunction aF, const std::vector<const char*>& options, const char* label, const RCStyle& style, DirectionType direction)
  : RCSwitchControlBase(bounds, kNoParameter, aF, static_cast<int>(options.size()))
  , mDirectionType(direction)
  , mStyle(style)
{
  mDirection = GetDirection();
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;

  for (auto& option : options)
  {
    mTabLabels.Add(new WDL_String(option));
  }
}

RCTabSwitchControl::RCTabSwitchControl(const IRECT& bounds, int paramIdx, const std::vector<const char*>& options, const char* label, const RCStyle& style, DirectionType direction)
  : RCSwitchControlBase(bounds, paramIdx, nullptr, (int)options.size())
  , mDirectionType(direction)
  , mStyle(style)
{
  mDirection = GetDirection();
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;

  for (auto& option : options)
  {
    mTabLabels.Add(new WDL_String(option));
  }
}

void RCTabSwitchControl::OnInit()
{
  RCSwitchControlBase::OnInit();

  const IParam* pParam = GetParam();

  if (pParam && mTabLabels.GetSize() == 0) // don't add param display text based labels if allready added via ctor
  {
    for (int i = 0; i < mNumStates; i++)
    {
      mTabLabels.Add(new WDL_String(GetParam()->GetDisplayText(i)));
    }
  }
}
void RCTabSwitchControl::Draw(IGraphics& g) { DrawWidget(g); }

void RCTabSwitchControl::DrawWidget(IGraphics& g)
{
  const int selected = GetSelectedIdx();
  ETabSegment segment = ETabSegment::Start;

  for (int i = 0; i < mNumStates; i++)
  {
    IRECT r = mButtons.Get()[i];

    if (i > 0)
      segment = ETabSegment::Mid;

    if (i == mNumStates - 1)
      segment = ETabSegment::End;

    const bool isPressed = mMousePressButton == i;
    const bool isSelected = i == selected;
    const bool isMouseOver = mMouseOverButton == i;
    const bool isDisabled = IsDisabled() || GetStateDisabled(i);
    const WidgetColorSet color = mStyle.GetColors(!isDisabled && !isSelected && isMouseOver, !isDisabled && !isSelected && isPressed, isDisabled || !isSelected, i);

    DrawButton(g, r, color, isSelected, isPressed, isMouseOver, segment, isDisabled);

    if (mTabLabels.Get(i))
    {
      DrawButtonText(g, r, color, isSelected, isPressed, isMouseOver, segment, isDisabled, mTabLabels.Get(i)->Get());
    }
  }
}

void RCTabSwitchControl::DrawButton(IGraphics& g, const IRECT& r, WidgetColorSet color, bool isSelected, bool isPressed, bool isMouseOver, ETabSegment segment, bool isDisabled)
{
  const float borderWidth = mStyle.drawFrame ? mStyle.frameThickness : 0.f;
  DrawBorder(g, color, r, borderWidth);
  const IRECT contentBounds = r.GetPadded(-borderWidth);
  g.FillRect(color.GetColor(), contentBounds, &mBlend);
}

void RCTabSwitchControl::DrawBorder(IGraphics& g, WidgetColorSet color, IRECT bounds, float borderWidth)
{
  if (!borderWidth)
    return;

  const IRECT borderBounds = bounds.GetPadded(-(borderWidth * .5f));
  g.DrawRect(color.GetBorderColor(), borderBounds, &mBlend, borderWidth);
}


void RCTabSwitchControl::DrawButtonText(
  IGraphics& g, const IRECT& r, WidgetColorSet color, bool isSelected, bool isPressed, bool isMouseOver, ETabSegment segment, bool isDisabled, const char* textStr)
{
  if (CStringHasContents(textStr))
  {
    const IText& text = mStyle.GetText().WithFGColor(color.GetLabelColor());
    g.DrawText(text, textStr, r, &mBlend);
  }
}

bool RCTabSwitchControl::IsHit(float x, float y) const { return GetButtonForPoint(x, y) > -1; }

void RCTabSwitchControl::OnMouseOver(float x, float y, const IMouseMod& mod)
{
  mMouseOverButton = GetButtonForPoint(x, y);

  RCSwitchControlBase::OnMouseOver(x, y, mod);

  SetDirty(false);
}

void RCTabSwitchControl::OnMouseOut()
{
  mMouseOverButton = -1;
  RCSwitchControlBase::OnMouseOut();
  SetDirty(false);
}

void RCTabSwitchControl::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  mMousePressButton = GetButtonForPoint(x, y);
  SetDirty(false);
}

void RCTabSwitchControl::OnMouseUp(float x, float y, const IMouseMod& mod)
{
  const int index = GetButtonForPoint(x, y);
  const bool trigger = index == mMousePressButton;
  mMousePressButton = -1;
  mMouseOverButton = index;
  if (!trigger)
  {
    SetDirty(false);
    return;
  }

  if (index > -1)
    SetValue(((double)index * (1. / (double)(mNumStates - 1))));
  SetDirty(true);
}


void RCTabSwitchControl::OnResize()
{
  mButtons.Resize(0);
  const float total_size = (mDirection == EDirection::Horizontal) ? mRECT.W() : mRECT.H();
  const float individual_size = (mDirection == EDirection::Vertical) ? mRECT.W() : mRECT.H();
  const bool isFilled = IsFilled();
  const float button_size = (isFilled) ? total_size / mNumStates : individual_size;
  const float total_gap_size = (isFilled) ? 0.f : total_size - (individual_size * mNumStates);
  const float gap = (isFilled) ? 0.f : total_gap_size / (mNumStates - 1);
  IRECT remainingBound = mRECT;
  IRECT buttonBound;

  for (int i = 0; i < mNumStates; i++)
  {
    if (mDirection == EDirection::Horizontal)
    {
      buttonBound = remainingBound.ReduceFromLeft(button_size);
      if (gap)
        remainingBound.ReduceFromLeft(gap);
    }
    else
    {
      buttonBound = remainingBound.ReduceFromBottom(button_size);
      if (gap)
        remainingBound.ReduceFromBottom(gap);
    }
    mButtons.Add(buttonBound);
  }

  SetDirty(false);
}


int RCTabSwitchControl::GetButtonForPoint(float x, float y) const
{
  for (int i = 0; i < mNumStates; i++)
  {
    if (mButtons.Get()[i].Contains(x, y))
      return i;
  }

  return -1;
}

const char* RCTabSwitchControl::GetSelectedLabelStr() const { return mTabLabels.Get(GetSelectedIdx())->Get(); }

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
