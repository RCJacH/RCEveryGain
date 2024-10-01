#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"
#include "Widgets/Color.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCButtonColors
{
public:
  RCButtonColors(IColor coverColor, IColor lightColor)
  {
    float h;
    float s;
    float l;
    float a;
    coverColor.GetHSLA(h, s, l, a);
    Color::HSLA coverColorHSLA = Color::HSLA(h, s, l, a);
    lightColor.GetHSLA(h, s, l, a);
    Color::HSLA lightColorHSL = Color::HSLA(h, s, l, a);
    };
  virtual ~RCButtonColors() {}

protected:
  Color::HSLA mActualCoverColorWhenOff;
  Color::HSLA mActualCoverColorWhenOn;
  Color::HSLA mActualLightColorWhenOff;
  Color::HSLA mActualLightColorWhenOn;
  Color::HSLA mSocketColorWhenOff;
  Color::HSLA mSocketColorWhenOn;
};

class RCButton : public IVectorBase
{
public:
  RCButton(const char* value = "", const IVStyle& style = DEFAULT_STYLE, bool valueInButton = true);

  virtual ~RCButton() {}

  virtual void Draw(IGraphics& g, const IRECT& bounds, bool active);
  virtual void DrawShadow(IGraphics& g, const std::array<float, 2> offset = {0.f, 0.f}, const IBlend* pBlend = 0);
  virtual void DrawSocket(IGraphics& g);
  virtual void DrawBulb(IGraphics& g);
  virtual void DrawBase(IGraphics& g);
  virtual void DrawBody(IGraphics& g);
  // virtual void DrawEmitter(IGraphics& g);
  // virtual void DrawHighlight(IGraphics& g);

  void SetBounds(const IRECT& bounds) { mRECT = bounds; };
  void SetActive(const bool active) { mActive = active; };

protected:
  IRECT mRECT = IRECT();
  bool mActive = false;
  float mBorderWidth;
  RCButtonColors mColors;
};

RCButton::RCButton(const char* value, const IVStyle& style, bool valueInButton)
  : IVectorBase(style, false, valueInButton)
  , mColors(RCButtonColors(mStyle.colorSpec.GetColor(EVColor::kFG), mStyle.colorSpec.GetColor(EVColor::kPR)))
{
}

void RCButton::Draw(IGraphics& g, const IRECT& bounds, bool active)
{
  DrawShadow(g, {-.08f, .06f});
  DrawBulb(g);
};

void RCButton::DrawShadow(IGraphics& g, const std::array<float, 2> offset, const IBlend* pBlend)
{
  // const float hOffset = mRECT.W() * offset[0];
  // const float vOffset = mRECT.H() * offset[1];
  // IRECT shadowBounds = mRECT.GetOffset(hOffset < 0.f ? hOffset : 0.f, vOffset < 0.f ? vOffset : 0.f, hOffset > 0.f ? -hOffset : 0.f, vOffset > 0.f ? -vOffset : 0.f);
  // std::initializer_list shadowGradStops = {IColorStop(shadowColor, .88f), IColorStop(shadowColor, 1.f)};
  // const IPattern pattern = IPattern::CreateRadialGradient(shadowBounds.MW(), shadowBounds.MH(), shadowBounds.W() * .5f, shadowGradStops);
  // g.PathClear();
  // g.PathEllipse(shadowBounds);
  // g.PathFill(pattern, IFillOptions(), pBlend);
}
void RCButton::DrawSocket(IGraphics& g) {}
void RCButton::DrawBulb(IGraphics& g)
{
  DrawBase(g);
  DrawBody(g);
}

void RCButton::DrawBase(IGraphics& g)
{

  // g.PathClear();
  // g.PathEllipse(mRECT);
  // g.PathFill(mRingColor.AsIColor());
  // g.PathClear();
  // g.PathEllipse(mRECT);
  // g.PathStroke(mBorderColor.AsIColor(), mBorderWidth);

  // if (mActive)
  // {
  //   std::initializer_list emitterGradStops = {
  //     IColorStop(COLOR_TRANSPARENT, .0f),
  //     IColorStop(mRingColor.AsIColor(), .4f),
  //   };
  //   IPattern pattern = IPattern::CreateLinearGradient(mRECT.L + mRECT.W(), mRECT.T, mRECT.L, mRECT.T + mRECT.H(), emitterGradStops);
  //   g.PathClear();
  //   g.PathEllipse(mRECT);
  //   g.PathStroke(pattern, mBorderWidth);
  // }

  // g.PathClear();
  // g.PathEllipse(mRECT);
  // g.PathStroke(mBaseColor.AsIColor(), mBorderWidth);
}

void RCButton::DrawBody(IGraphics& g)
{
  // std::initializer_list darkGradStops = {
  //   IColorStop(mShadeColor.AsIColor(), .0f),
  //   IColorStop(mBaseColor.AsIColor(), .1f),
  // };
  // IPattern darkGrad = IPattern::CreateRadialGradient(mRECT.L + mRECT.MW(), mRECT.T, mRECT.H(), darkGradStops);
  // std::initializer_list lightGradStops = {
  //   IColorStop(mShadeColor.scaled(0.f, .2f, .4f, -.25).AsIColor(), 0.f),
  //   IColorStop(mBaseColor.WithAlpha(.0f).AsIColor(), 1.f),
  // };
  // IPattern lightGrad = IPattern::CreateRadialGradient(mRECT.L + mRECT.W() * .3f, mRECT.T + mRECT.H() * .7f, mRECT.MW() * .5f, lightGradStops);

  // g.PathClear();
  // g.PathEllipse(mRECT.GetPadded(-mRECT.W() * .04f));
  // g.PathFill(darkGrad);
  // g.PathClear();
  // g.PathEllipse(mRECT);
  // g.PathFill(lightGrad);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
