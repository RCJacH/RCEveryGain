#include "IControl.h"
#include "IControls.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class IBFittedTriggerButtonControl : public IControl, public IVectorBase
{
public:
  IBFittedTriggerButtonControl(const IRECT& bounds,
                               const IBitmap& offBitmap,
                               const IBitmap& onBitmap,
                               const char* label = "",
                               const IVStyle& style = DEFAULT_STYLE,
                               const std::array<float, 4> textOffsets = defaultTextOffsets,
                               IActionFunction aF = DefaultClickActionFunc);

  virtual ~IBFittedTriggerButtonControl(){};
  void Draw(IGraphics& g) override;
  void DrawWidget(IGraphics& g) override;
  void OnRescale() override
  {
    mOffBitmap = GetUI()->GetScaledBitmap(mOffBitmap);
    mOnBitmap = GetUI()->GetScaledBitmap(mOnBitmap);
  }
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
  void OnEndAnimation() override;
  void OnResize() override;

private:
  static constexpr std::array<float, 4> defaultTextOffsets = {0.f, 0.f, 0.f, 0.f};

protected:
  IBitmap mOffBitmap;
  IBitmap mOnBitmap;
  bool isFitted = false;
  bool mMouseDown = false;
  std::array<float, 4> mTextOffset;
};

IBFittedTriggerButtonControl::IBFittedTriggerButtonControl(
  const IRECT& bounds, const IBitmap& offBitmap, const IBitmap& onBitmap, const char* label, const IVStyle& style, const std::array<float, 4> textOffsets, IActionFunction aF)
  : IControl(bounds, kNoParameter, aF)
  , IVectorBase(style, true)
  , mOffBitmap(offBitmap)
  , mOnBitmap(onBitmap)
  , mTextOffset(textOffsets)
{
  mDblAsSingleClick = true;
  isFitted = true;
  AttachIControl(this, label);
}

void IBFittedTriggerButtonControl::Draw(IGraphics& g)
{
  DrawWidget(g);
  DrawLabel(g);
}

void IBFittedTriggerButtonControl::DrawWidget(IGraphics& g) { g.DrawFittedBitmap(GetValue() > 0.5 ? mOnBitmap : mOffBitmap, mRECT); }

void IBFittedTriggerButtonControl::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  if (mRECT.ContainsEdge(x, y) && !mMouseDown)
  {
    mMouseDown = true;
    SetValue(1.);
    SetDirty(false);
  }
}

void IBFittedTriggerButtonControl::OnMouseUp(float x, float y, const IMouseMod& mod)
{
  if (mMouseDown && mRECT.ContainsEdge(x, y))
  {
    SetDirty(true);
    OnEndAnimation();
  }
  mMouseDown = false;
}

void IBFittedTriggerButtonControl::OnEndAnimation()
{
  SetValue(0.);
  SetDirty(false);
  IControl::OnEndAnimation();
}

void IBFittedTriggerButtonControl::OnResize()
{
  SetTargetRECT(MakeRects(mRECT, false));
  SetDirty(false);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
