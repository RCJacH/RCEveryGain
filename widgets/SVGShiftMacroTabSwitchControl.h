#include "IControl.h"
#include "IControls.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A vector "tab" multi switch control. Click tabs to cycle through states. */
class SVGShiftMacroTabSwitchControl : public IVTabSwitchControl
{
public:
  /** Constructs a vector tab switch control, with an action function (no parameter)
   * @param bounds The control's bounds
   * @param aF An action function to execute when a button is clicked \see IActionFunction
   * @param offImage An SVG for the off state of the button
   * @param onImage An SVG for the on state of the button
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGShiftMacroTabSwitchControl(const IRECT& bounds,
                                IActionFunction aF,
                                const ISVG& offImage,
                                const ISVG& onImage,
                                const char* label = "",
                                const float gap = 0.f,
                                const IVStyle& style = DEFAULT_STYLE,
                                EDirection direction = EDirection::Horizontal);

  /** Constructs a vector tab switch control, linked to a parameter
   * @param bounds The control's bounds
   * @param paramIdx The parameter index to link this control to
   * @param offImage An SVG for the off state of the button
   * @param onImage An SVG for the on state of the button
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGShiftMacroTabSwitchControl(const IRECT& bounds,
                                int paramIdx,
                                const ISVG& offImage,
                                const ISVG& onImage,
                                const char* label = "",
                                const float gap = 0.f,
                                const IVStyle& style = DEFAULT_STYLE,
                                EDirection direction = EDirection::Horizontal);

  virtual ~SVGShiftMacroTabSwitchControl() {}

  virtual void DrawButton(IGraphics& g, const IRECT& bounds, bool pressed, bool mouseOver, ETabSegment segment, bool disabled) override;
  virtual void DrawWidget(IGraphics& g) override;

  void OnResize() override;

protected:
  ISVG mOffSVG;
  ISVG mOnSVG;
  float mGap;
};

SVGShiftMacroTabSwitchControl::SVGShiftMacroTabSwitchControl(
  const IRECT& bounds, IActionFunction aF, const ISVG& offImage, const ISVG& onImage, const char* label, float gap, const IVStyle& style, EDirection direction)
  : IVTabSwitchControl(bounds, kNoParameter, {}, label, style, EVShape::Rectangle, direction)
  , mOffSVG(offImage)
  , mOnSVG(onImage)
  , mGap(gap)
{
  AttachIControl(this, label);
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;
}

SVGShiftMacroTabSwitchControl::SVGShiftMacroTabSwitchControl(
  const IRECT& bounds, int paramIdx, const ISVG& offImage, const ISVG& onImage, const char* label, float gap, const IVStyle& style, EDirection direction)
  : IVTabSwitchControl(bounds, paramIdx, {}, label, style, EVShape::Rectangle, direction)
  , mOffSVG(offImage)
  , mOnSVG(onImage)
  , mGap(gap)
{
  AttachIControl(this, label);
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;
}

void SVGShiftMacroTabSwitchControl::DrawButton(IGraphics& g, const IRECT& r, bool pressed, bool mouseOver, ETabSegment segment, bool disabled) { g.DrawSVG(pressed ? mOnSVG : mOffSVG, r, &mBlend); }

void SVGShiftMacroTabSwitchControl::DrawWidget(IGraphics& g)
{
  int selected = GetSelectedIdx();
  int center = mNumStates / 2;
  int selected_offset = selected - center;
  int selected_offset_abs = abs(selected_offset);
  int selected_offset_sign = (selected_offset > 0) - (selected_offset < 0);
  ETabSegment segment = ETabSegment::Start;

  for (int i = 0; i < mNumStates; i++)
  {
    IRECT r = mButtons.Get()[i];

    if (i > 0)
      segment = ETabSegment::Mid;

    if (i == mNumStates - 1)
      segment = ETabSegment::End;

    int index_offset = i - center;
    int index_offset_abs = abs(index_offset);
    int index_offset_sign = (index_offset > 0) - (index_offset < 0);

    const bool isSelected = i == center || (index_offset_sign == selected_offset_sign && index_offset_abs <= selected_offset_abs);
    const bool isMouseOver = mMouseOverButton == i;
    const bool isDisabled = IsDisabled() || GetStateDisabled(i);

    DrawButton(g, r, isSelected, isMouseOver, segment, isDisabled);
  }
}

void SVGShiftMacroTabSwitchControl::OnResize()
{
  SetTargetRECT(MakeRects(mRECT));

  mButtons.Resize(0);
  float total_gap_size = mGap * (mNumStates - 1);
  float total_size = (mDirection == EDirection::Horizontal) ? mWidgetBounds.W() : mWidgetBounds.H();
  float button_size = (total_size - total_gap_size) / mNumStates;

  for (int i = 0; i < mNumStates; i++)
  {
    IRECT buttonBound;
    float reduce_before = button_size * i + mGap * i;
    int n_remaining = mNumStates - i - 1;
    float reduce_after = button_size * n_remaining + mGap * n_remaining;
    if (mDirection == EDirection::Horizontal)
      buttonBound = mWidgetBounds.GetReducedFromLeft(reduce_before).GetReducedFromRight(reduce_after);
    else
      buttonBound = mWidgetBounds.GetReducedFromTop(reduce_before).GetReducedFromBottom(reduce_after);

    if (buttonBound.H() > buttonBound.W())
      buttonBound.MidVPad(buttonBound.W() * 0.5);
    else if (buttonBound.W() > buttonBound.H())
      buttonBound.MidHPad(buttonBound.H() * 0.5);
    mButtons.Add(buttonBound);
  }

  SetDirty(false);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
