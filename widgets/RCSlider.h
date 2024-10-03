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
           DrawRect::DirectionType dir = DrawRect::DirectionType::Horizontal,
           double gearing = DEFAULT_GEARING);

  RCSlider(const IRECT& bounds,
           IActionFunction aF,
           const Color::HSLA& color,
           const IVStyle& style = DEFAULT_STYLE,
           bool valueIsEditable = false,
           DrawRect::DirectionType dir = DrawRect::DirectionType::Horizontal,
           double gearing = DEFAULT_GEARING);

  virtual ~RCSlider() {}

  virtual void Draw(IGraphics& g);

  void SetActive(const bool active) { mActive = active; };

protected:
  IRECT mRECT = IRECT();
  DrawRect::DirectionType mDirection;
  bool mActive = false;
  DrawRect::ControlState mState = DrawRect::ControlState::kNormal;
};

RCSlider::RCSlider(const IRECT& bounds, int paramIdx, const Color::HSLA& color, const IVStyle& style, bool valueIsEditable = false, DrawRect::DirectionType dir, double gearing)
  : ISliderControlBase(bounds, paramIdx, DrawRect::ToEDirection(dir), gearing)
  , DrawRect(color, style)
  , mDirection(dir)
{
  DisablePrompt(!valueIsEditable);
  mText = style.valueText;
  mHideCursorOnDrag = style.hideCursor;
  mShape = EVShape::Rectangle;
}

RCSlider::RCSlider(const IRECT& bounds, IActionFunction aF, const Color::HSLA& color, const IVStyle& style, bool valueIsEditable, DrawRect::DirectionType dir, double gearing)
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
  DrawTrack(g, mRECT, mState, GetValue(), mDirection);
  DrawValue(g, mRECT, mState, mDirection);
};

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
