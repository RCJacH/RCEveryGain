#include "IControl.h"
#include "IControls.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class IBFaderSliderControl : public IBSliderControl
{
public:
  IBFaderSliderControl(
    const IRECT& bounds, const IBitmap& handleBitmap, const IBitmap& shadowBitmap, int paramIdx, EDirection dir, double gearing, const std::array<float, 2> shadowOffsets = defaultShadowOffsets);

  virtual ~IBFaderSliderControl() {};
  void Draw(IGraphics& g) override;
  void OnRescale() override
  {
    mBitmap = GetUI()->GetScaledBitmap(mBitmap);
    mShadowBitmap = GetUI()->GetScaledBitmap(mShadowBitmap);
  }

private:
  static constexpr std::array<float, 2> defaultShadowOffsets = {0.f, 0.f};

protected:
  IBitmap mShadowBitmap;
  std::array<float, 2> mShadowOffsets;
};

IBFaderSliderControl::IBFaderSliderControl(
  const IRECT& bounds, const IBitmap& handleBitmap, const IBitmap& shadowBitmap, int paramIdx, EDirection dir, double gearing, const std::array<float, 2> shadowOffsets)
  : IBSliderControl(bounds, handleBitmap, IBitmap(), paramIdx, dir, gearing)
  , mShadowBitmap(shadowBitmap)
  , mShadowOffsets(shadowOffsets)
{
}

void IBFaderSliderControl::Draw(IGraphics& g)
{
  if (mShadowBitmap.IsValid())
  {
    float h = mShadowOffsets[0];
    float v = mShadowOffsets[1];
    IRECT shadowBounds = mRECT.GetCentredInside(IRECT(0, 0, mShadowBitmap));
    float loff = (h < 0.f) * h;
    float toff = (v < 0.f) * v;
    float roff = (h > 0.f) * -h;
    float boff = (v > 0.f) * -v;
    shadowBounds = shadowBounds.GetOffset(loff, toff, roff, boff);
    g.DrawBitmap(mShadowBitmap, shadowBounds, 0, 0, &mBlend);
  }


  g.DrawFittedBitmap(mBitmap, GetHandleBounds(), &mBlend);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
