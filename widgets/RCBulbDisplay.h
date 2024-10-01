#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCBulbDisplay : public IVectorBase
{
public:
  RCBulbDisplay(const IRECT& bounds, const bool active, const char* label = "", const char* value = "", const IVStyle& style = DEFAULT_STYLE, bool valueInButton = true);

  virtual ~RCBulbDisplay() {}

  virtual void Draw(IGraphics& g);
  virtual void DrawShadow(IGraphics& g, const std::array<float, 2> offset = {0.f, 0.f}, const IBlend* pBlend = 0);
  virtual void DrawBulb(IGraphics& g);
  virtual void DrawBase(IGraphics& g);
  virtual void DrawBody(IGraphics& g);
  // virtual void DrawEmitter(IGraphics& g);
  // virtual void DrawHighlight(IGraphics& g);

  void SetBounds(const IRECT& bounds) { mRECT = bounds; };
  void SetActive(const bool active) { mActive = active; };

private:
  IRECT mRECT = IRECT();
  bool mActive = false;
  float mBorderWidth;
  Color::HSLA mBaseColor;
  Color::HSLA mShadowColor;
  Color::HSLA mRingColor;
  Color::HSLA mShadeColor;
  Color::HSLA mHighlightColor;
  Color::HSLA mBorderColor;
};

RCBulbDisplay::RCBulbDisplay(const IRECT& bounds, const bool active, const char* label, const char* value, const IVStyle& style, bool valueInButton)
  : IVectorBase(style, false, valueInButton)
  , mRECT(bounds)
  , mActive(active)
  , mBorderWidth(bounds.W() * .06f)
{
  mRECT.Pad(-mBorderWidth);
  float H;
  float S;
  float L;
  float A;
  mStyle.colorSpec.GetColor(active ? EVColor::kPR : EVColor::kFG).GetHSLA(H, S, L, A);
  mBaseColor = Color::HSLA(int(H * 360), S, L, A);
  mRingColor = mBaseColor.scaled(-35.f, .9f);
  mShadeColor = mBaseColor.scaled(0.f, -.1f, -.6f);
  mHighlightColor = mBaseColor.scaled(0.f, -.5f, .9f);
  mShadowColor = mRingColor.scaled(0.f, -.2f, mActive ? -.3f : -.8f, -.4f);
  mBorderColor = mBaseColor.scaled(0.f, -.6f, -.8f);
}

void RCBulbDisplay::Draw(IGraphics& g)
{
  DrawShadow(g, {-.08f, .06f});
  DrawBulb(g);
};

void RCBulbDisplay::DrawShadow(IGraphics& g, const std::array<float, 2> offset, const IBlend* pBlend)
{
  const float hOffset = mRECT.W() * offset[0];
  const float vOffset = mRECT.H() * offset[1];
  IColor shadowColor = mShadowColor.AsIColor();
  IRECT shadowBounds = mRECT.GetOffset(hOffset < 0.f ? hOffset : 0.f, vOffset < 0.f ? vOffset : 0.f, hOffset > 0.f ? -hOffset : 0.f, vOffset > 0.f ? -vOffset : 0.f);
  std::initializer_list shadowGradStops = {IColorStop(shadowColor, .88f), IColorStop(shadowColor, 1.f)};
  const IPattern pattern = IPattern::CreateRadialGradient(shadowBounds.MW(), shadowBounds.MH(), shadowBounds.W() * .5f, shadowGradStops);
  g.PathClear();
  g.PathEllipse(shadowBounds);
  g.PathFill(pattern, IFillOptions(), pBlend);
}

void RCBulbDisplay::DrawBulb(IGraphics& g)
{
  DrawBase(g);
  DrawBody(g);
}

void RCBulbDisplay::DrawBase(IGraphics& g)
{

  g.PathClear();
  g.PathEllipse(mRECT);
  g.PathFill(mRingColor.AsIColor());
  g.PathClear();
  g.PathEllipse(mRECT);
  g.PathStroke(mBorderColor.AsIColor(), mBorderWidth);

  if (mActive)
  {
    std::initializer_list emitterGradStops = {
      IColorStop(COLOR_TRANSPARENT, .0f),
      IColorStop(mRingColor.AsIColor(), .4f),
    };
    IPattern pattern = IPattern::CreateLinearGradient(mRECT.L + mRECT.W(), mRECT.T, mRECT.L, mRECT.T + mRECT.H(), emitterGradStops);
    g.PathClear();
    g.PathEllipse(mRECT);
    g.PathStroke(pattern, mBorderWidth);
  }

  g.PathClear();
  g.PathEllipse(mRECT);
  g.PathStroke(mBaseColor.AsIColor(), mBorderWidth);
}

void RCBulbDisplay::DrawBody(IGraphics& g)
{
  std::initializer_list darkGradStops = {
    IColorStop(mShadeColor.AsIColor(), .0f),
    IColorStop(mBaseColor.AsIColor(), .1f),
  };
  IPattern darkGrad = IPattern::CreateRadialGradient(mRECT.L + mRECT.MW(), mRECT.T, mRECT.H(), darkGradStops);
  // std::initializer_list lightGradStops = {
  //   IColorStop(mShadeColor.scaled(0.f, .2f, .4f, -.25).AsIColor(), 0.f),
  //   IColorStop(mBaseColor.WithAlpha(.0f).AsIColor(), 1.f),
  // };
  // IPattern lightGrad = IPattern::CreateRadialGradient(mRECT.L + mRECT.W() * .3f, mRECT.T + mRECT.H() * .7f, mRECT.MW() * .5f, lightGradStops);

  g.PathClear();
  g.PathEllipse(mRECT.GetPadded(-mRECT.W() * .04f));
  g.PathFill(darkGrad);
  // g.PathClear();
  // g.PathEllipse(mRECT);
  // g.PathFill(lightGrad);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
