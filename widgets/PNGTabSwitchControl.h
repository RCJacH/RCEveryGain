#include "IControl.h"
#include "IControls.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A vector "tab" multi switch control. Click tabs to cycle through states. */
class PNGTabSwitchControl : public IVTabSwitchControl
{
public:
  /** Constructs a vector tab switch control, with an action function (no parameter)
   * @param bounds The control's bounds
   * @param aF An action function to execute when a button is clicked \see IActionFunction
   * @param offBitmap An Bitmap for the off state of the button
   * @param onBitmap An Bitmap for the on state of the button
   * @param options An initializer list of CStrings for the button labels to override the parameter display text labels. Supply an empty {} list if you don't want to do that.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  PNGTabSwitchControl(const IRECT& bounds,
                      IActionFunction aF,
                      const IBitmap& offBitmap,
                      const IBitmap& onBitmap,
                      const std::vector<const char*>& options,
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  /** Constructs a vector tab switch control, linked to a parameter
   * @param bounds The control's bounds
   * @param paramIdx The parameter index to link this control to
   * @param offBitmap An Bitmap for the off state of the button
   * @param onBitmap An Bitmap for the on state of the button
   * @param options An initializer list of CStrings for the button labels to override the parameter display text labels. Supply an empty {} list if you don't want to do that.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  PNGTabSwitchControl(const IRECT& bounds,
                      int paramIdx,
                      const IBitmap& offBitmap,
                      const IBitmap& onBitmap,
                      const std::vector<const char*>& options,
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  virtual ~PNGTabSwitchControl() {}

  virtual void DrawButton(IGraphics& g, const IRECT& bounds, bool pressed, bool mouseOver, ETabSegment segment, bool disabled) override;
  virtual void DrawWidget(IGraphics& g) override;

  void OnResize() override;
  void OnRescale() override
  {
    mOffBitmap = GetUI()->GetScaledBitmap(mOffBitmap);
    mOnBitmap = GetUI()->GetScaledBitmap(mOnBitmap);
  }

private:
  static constexpr std::array<float, 4> defaultTextOffsets = {0.f, 0.f, 0.f, 0.f};

protected:
  IBitmap mOffBitmap;
  IBitmap mOnBitmap;
  float mGap;
  std::array<float, 4> mTextOffset;
};

PNGTabSwitchControl::PNGTabSwitchControl(const IRECT& bounds,
                                         IActionFunction aF,
                                         const IBitmap& offBitmap,
                                         const IBitmap& onBitmap,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, kNoParameter, options, label, style, EVShape::Rectangle, direction)
  , mOffBitmap(offBitmap)
  , mOnBitmap(onBitmap)
  , mGap(gap)
  , mTextOffset(textOffsets)
{
  AttachIControl(this, label);
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;

  for (auto& option : options)
  {
    mTabLabels.Add(new WDL_String(option));
  }
}

PNGTabSwitchControl::PNGTabSwitchControl(const IRECT& bounds,
                                         int paramIdx,
                                         const IBitmap& offBitmap,
                                         const IBitmap& onBitmap,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, paramIdx, options, label, style, EVShape::Rectangle, direction)
  , mOffBitmap(offBitmap)
  , mOnBitmap(onBitmap)
  , mGap(gap)
  , mTextOffset(textOffsets)
{
  AttachIControl(this, label);
  mText = style.valueText;
  mText.mAlign = mStyle.valueText.mAlign = EAlign::Center;
  mText.mVAlign = mStyle.valueText.mVAlign = EVAlign::Middle;

  for (auto& option : options)
  {
    mTabLabels.Add(new WDL_String(option));
  }
}

void PNGTabSwitchControl::DrawButton(IGraphics& g, const IRECT& r, bool pressed, bool mouseOver, ETabSegment segment, bool disabled) { g.DrawFittedBitmap(pressed ? mOnBitmap : mOffBitmap, r); }

void PNGTabSwitchControl::DrawWidget(IGraphics& g)
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

    if (mTabLabels.Get(i))
    {
      float h = isSelected ? mTextOffset[2] : mTextOffset[0];
      float v = isSelected ? mTextOffset[3] : mTextOffset[1];
      DrawButtonText(g, r.GetOffset(h < 0.f ? h : 0.f, v < 0.f ? v : 0.f, h > 0.f ? -h : 0.f, v > 0.f ? -v : 0.f), isSelected, isMouseOver, segment, isDisabled, mTabLabels.Get(i)->Get());
    }
  }
}

void PNGTabSwitchControl::OnResize()
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
      buttonBound.MidVPad(buttonBound.W() * .5f);
    else if (buttonBound.W() > buttonBound.H())
      buttonBound.MidHPad(buttonBound.H() * .5f);
    mButtons.Add(buttonBound);
  }

  SetDirty(false);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
