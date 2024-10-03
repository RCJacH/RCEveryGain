#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"
#include "Widgets/DrawRect.h"
#include "widgets/WidgetColors.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCSlider : public ISliderControlBase, public DrawRect
{
public:
  RCSlider(const IRECT& bounds,
           int paramIdx = kNoParameter,
           const Color::HSLA& color,
           const IVStyle& style = DEFAULT_STYLE,
           bool valueIsEditable = false,
           DirectionType dir = DirectionType::Horizontal,
           double gearing = DEFAULT_GEARING);

  RCSlider(const IRECT& bounds,
           IActionFunction aF,
           const Color::HSLA& color,
           const IVStyle& style = DEFAULT_STYLE,
           bool valueIsEditable = false,
           DirectionType dir = DirectionType::Horizontal,
           double gearing = DEFAULT_GEARING);

  virtual ~RCSlider() {}

  virtual void Draw(IGraphics& g);

  void SetActive(const bool active) { mActive = active; };
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
  void OnMouseOver(float x, float y, const IMouseMod& mod) override;
  void OnMouseOut() override
  {
    mValueMouseOver = false;
    ISliderControlBase::OnMouseOut();
  }
  bool IsHit(float x, float y) const override;
  void OnResize() override;
  void SetDirty(bool push, int valIdx = kNoValIdx) override;
  void OnInit() override;

protected:
  IRECT mRECT = IRECT();
  DirectionType mDirection;
  bool mActive = false;
  bool mValueMouseOver = false;
};

RCSlider::RCSlider(const IRECT& bounds, int paramIdx, const Color::HSLA& color, const IVStyle& style, bool valueIsEditable = false, DirectionType dir, double gearing)
  : ISliderControlBase(bounds, paramIdx, DrawRect::ToEDirection(dir), gearing)
  , DrawRect(color, style)
  , mDirection(dir)
{
  DisablePrompt(!valueIsEditable);
  mText = style.valueText;
  mHideCursorOnDrag = style.hideCursor;
  mShape = EVShape::Rectangle;
}

RCSlider::RCSlider(const IRECT& bounds, IActionFunction aF, const Color::HSLA& color, const IVStyle& style, bool valueIsEditable, DirectionType dir, double gearing)
  : ISliderControlBase(bounds, aF, DrawRect::ToEDirection(dir), gearing)
  , DrawRect(color, style)
  , mDirection(dir)
{
  DisablePrompt(!valueIsEditable);
  mText = style.valueText;
  mHideCursorOnDrag = style.hideCursor;
  mShape = EVShape::Rectangle;
};

void RCSlider::Draw(IGraphics& g)
{
  ControlState state = mMouseDown ? ControlState::kPressed : mMouseIsOver ? ControlState::kHovered : ControlState::kNormal;
  DrawTrack(g, mRECT, state, GetValue(), mDirection);
  DrawValue(g, mRECT, state, mDirection);
};

void RCSlider::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  if (mStyle.showValue && mValueBounds.Contains(x, y))
  {
    PromptUserInput(mValueBounds);
  }
  else
  {
    ISliderControlBase::OnMouseDown(x, y, mod);
  }
}

void RCSlider::OnMouseDblClick(float x, float y, const IMouseMod& mod)
{
#ifdef AAX_API
  PromptUserInput(mValueBounds);
#else
  SetValueToDefault(GetValIdxForPos(x, y));
#endif
}

void RCSlider::OnMouseUp(float x, float y, const IMouseMod& mod)
{
  ISliderControlBase::OnMouseUp(x, y, mod);
  SetDirty(true);
}

void RCSlider::OnMouseOver(float x, float y, const IMouseMod& mod)
{
  if (mStyle.showValue && !mDisablePrompt)
    mValueMouseOver = mValueBounds.Contains(x, y);

  ISliderControlBase::OnMouseOver(x, y, mod);
  SetDirty(false);
}

void RCSlider::OnResize()
{
  SetTargetRECT(MakeRects(mRECT));
  SetDirty(false);
}

bool RCSlider::IsHit(float x, float y) const
{
  if (!mDisablePrompt)
  {
    if (mValueBounds.Contains(x, y))
    {
      return true;
    }
  }

  return mWidgetBounds.Contains(x, y);
}

void RCSlider::SetDirty(bool push, int valIdx)
{
  ISliderControlBase::SetDirty(push);

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
