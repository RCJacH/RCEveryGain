#include "IControl.h"
#include "IControls.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A vector "tab" multi switch control. Click tabs to cycle through states. */
class SVGTabSwitchControl : public IVTabSwitchControl
{
public:
  /** Constructs a vector tab switch control, with an action function (no parameter)
   * @param bounds The control's bounds
   * @param aF An action function to execute when a button is clicked \see IActionFunction
   * @param offImage An SVG for the off state of the button
   * @param onImage An SVG for the on state of the button
   * @param options An initializer list of CStrings for the button labels. The size of the list decides the number of buttons.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param textOffset The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGTabSwitchControl(const IRECT& bounds,
                      IActionFunction aF,
                      const ISVG& offImage,
                      const ISVG& onImage,
                      const std::vector<const char*>& options = {},
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  /** Constructs a vector tab switch control, linked to a parameter
   * @param bounds The control's bounds
   * @param paramIdx The parameter index to link this control to
   * @param offImage An SVG for the off state of the button
   * @param onImage An SVG for the on state of the button
   * @param options An initializer list of CStrings for the button labels to override the parameter display text labels. Supply an empty {} list if you don't want to do that.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param textOffset The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGTabSwitchControl(const IRECT& bounds,
                      int paramIdx,
                      const ISVG& offImage,
                      const ISVG& onImage,
                      const std::vector<const char*>& options = {},
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  /** Constructs an SVG button control, with an action function and a single image, with color overrides
   * @param bounds The control's bounds
   * @param aF An action function to execute when a button is clicked \see IActionFunction
   * @param image An SVG for the on/off state of the button
   * @param colors Colors to replace the SVG's fill/stroke in the off/on/mouse-over-off/mouse-over-on states
   * @param colorReplacement Should the fill or stroke in the SVG be colored
   * @param options An initializer list of CStrings for the button labels. The size of the list decides the number of buttons.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param textOffset The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGTabSwitchControl(const IRECT& bounds,
                      IActionFunction aF,
                      const ISVG& image,
                      const std::array<IColor, 4> colors = {COLOR_BLACK, COLOR_WHITE, COLOR_DARK_GRAY, COLOR_LIGHT_GRAY},
                      EColorReplacement colorReplacement = EColorReplacement::Fill,
                      const std::vector<const char*>& options = {},
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  /** Constructs an SVG button control, with an action function and a single image, with color overrides
   * @param bounds The control's bounds
   * @param paramIdx The parameter index to link this control to
   * @param image An SVG for the on/off state of the button
   * @param colors Colors to replace the SVG's fill/stroke in the off/on/mouse-over-off/mouse-over-on states
   * @param colorReplacement Should the fill or stroke in the SVG be colored
   * @param options An initializer list of CStrings for the button labels. The size of the list decides the number of buttons.
   * @param label The IVControl label CString
   * @param gap Distance between each button
   * @param style The styling of this vector control \see IVStyle
   * @param textOffset The styling of this vector control \see IVStyle
   * @param direction The direction of the buttons */
  SVGTabSwitchControl(const IRECT& bounds,
                      int paramIdx,
                      const ISVG& image,
                      const std::array<IColor, 4> colors = {COLOR_BLACK, COLOR_WHITE, COLOR_DARK_GRAY, COLOR_LIGHT_GRAY},
                      EColorReplacement colorReplacement = EColorReplacement::Fill,
                      const std::vector<const char*>& options = {},
                      const char* label = "",
                      const float gap = 0.f,
                      const IVStyle& style = DEFAULT_STYLE,
                      const std::array<float, 4> textOffsets = defaultTextOffsets,
                      EDirection direction = EDirection::Horizontal);

  virtual ~SVGTabSwitchControl() { mTabLabels.Empty(true); }

  virtual void DrawButton(IGraphics& g, const IRECT& bounds, bool pressed, bool mouseOver, ETabSegment segment, bool disabled) override;
  virtual void DrawButtonText(IGraphics& g, const IRECT& bounds, bool pressed, bool mouseOver, ETabSegment segment, bool disabled, const char* textStr) override;

  void OnResize() override;

private:
  static constexpr std::array<float, 4> defaultTextOffsets = {0.f, 0.f, 0.f, 0.f};

protected:
  ISVG mOffSVG;
  ISVG mOnSVG;
  float mGap;
  std::array<float, 4> mTextOffset;
  std::array<IColor, 4> mColors;
  EColorReplacement mColorReplacement = EColorReplacement::None;
};

SVGTabSwitchControl::SVGTabSwitchControl(const IRECT& bounds,
                                         IActionFunction aF,
                                         const ISVG& offImage,
                                         const ISVG& onImage,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, kNoParameter, options, label, style, EVShape::Rectangle, direction)
  , mOffSVG(offImage)
  , mOnSVG(onImage)
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

SVGTabSwitchControl::SVGTabSwitchControl(const IRECT& bounds,
                                         int paramIdx,
                                         const ISVG& offImage,
                                         const ISVG& onImage,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, paramIdx, options, label, style, EVShape::Rectangle, direction)
  , mOffSVG(offImage)
  , mOnSVG(onImage)
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

SVGTabSwitchControl::SVGTabSwitchControl(const IRECT& bounds,
                                         IActionFunction aF,
                                         const ISVG& image,
                                         const std::array<IColor, 4> colors,
                                         EColorReplacement colorReplacement,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, aF, options, label, style, EVShape::Rectangle, direction)
  , mOffSVG(image)
  , mOnSVG(image)
  , mColors(colors)
  , mColorReplacement(colorReplacement)
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

SVGTabSwitchControl::SVGTabSwitchControl(const IRECT& bounds,
                                         int paramIdx,
                                         const ISVG& image,
                                         const std::array<IColor, 4> colors,
                                         EColorReplacement colorReplacement,
                                         const std::vector<const char*>& options,
                                         const char* label,
                                         float gap,
                                         const IVStyle& style,
                                         const std::array<float, 4> textOffsets,
                                         EDirection direction)
  : IVTabSwitchControl(bounds, paramIdx, options, label, style, EVShape::Rectangle, direction)
  , mOffSVG(image)
  , mOnSVG(image)
  , mColors(colors)
  , mColorReplacement(colorReplacement)
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

void SVGTabSwitchControl::DrawButton(IGraphics& g, const IRECT& r, bool pressed, bool mouseOver, ETabSegment segment, bool disabled)
{
  IColor* pOnColorFill = nullptr;
  IColor* pOffColorFill = nullptr;
  IColor* pOnColorStroke = nullptr;
  IColor* pOffColorStroke = nullptr;

  switch (mColorReplacement)
  {
  case EColorReplacement::None:
    break;
  case EColorReplacement::Fill:
    pOnColorFill = mMouseIsOver ? &mColors[3] : &mColors[1];
    pOffColorFill = mMouseIsOver ? &mColors[2] : &mColors[0];
    break;
  case EColorReplacement::Stroke:
    pOnColorStroke = mMouseIsOver ? &mColors[3] : &mColors[1];
    pOffColorStroke = mMouseIsOver ? &mColors[2] : &mColors[0];
    break;
  }

  if (pressed)
    g.DrawStretchedSVG(mOnSVG, r, &mBlend, pOnColorStroke, pOnColorFill);
  else
    g.DrawStretchedSVG(mOffSVG, r, &mBlend, pOffColorStroke, pOffColorFill);
}

void SVGTabSwitchControl::DrawButtonText(IGraphics& g, const IRECT& r, bool pressed, bool mouseOver, ETabSegment segment, bool disabled, const char* textStr)
{
  if (CStringHasContents(textStr))
  {
    float h = pressed ? mTextOffset[2] : mTextOffset[0];
    float v = pressed ? mTextOffset[3] : mTextOffset[1];
    g.DrawText(mStyle.valueText, textStr, r.GetOffset(h < 0.f ? h : 0.f, v < 0.f ? v : 0.f, h > 0.f ? -h : 0.f, v > 0.f ? -v : 0.f), &mBlend);
  }
}

void SVGTabSwitchControl::OnResize()
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

    mButtons.Add(buttonBound);
  }

  SetDirty(false);
}
END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
