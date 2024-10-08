#include "IControl.h"
#include "IControls.h"
#include "widgets/RCControl.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A base class for slider/fader controls, to handle mouse action and Sender. */
class RCSliderControl : public RCControl
{
public:
  RCSliderControl(const IRECT& bounds, int paramIdx = kNoParameter, EDirection dir = EDirection::Vertical, double gearing = DEFAULT_GEARING, float handleSize = 0.f);
  RCSliderControl(const IRECT& bounds, IActionFunction aF = nullptr, EDirection dir = EDirection::Vertical, double gearing = DEFAULT_GEARING, float handleSize = 0.f);

  virtual ~RCSliderControl() {}

  void OnResize() override;
  void OnMouseDblClick(float x, float y, const IMouseMod& mod) override;
  // void MouseOverAction(const IMouseMod& mod) override;
  // void MouseOutAction() override;
  // void MouseDownAction(const IMouseMod& mod) override;
  void MouseUpAction(const IMouseMod& mod) override;
  // void MouseHoverAction(const IMouseMod& mod) override;
  void MouseLPressAction(const IMouseMod& mod) override;
  // void MouseLClickAction(const IMouseMod& mod) override;
  // void MouseLReleaseAction() override;
  void MouseRPressAction(const IMouseMod& mod) override;
  void MouseRClickAction(const IMouseMod& mod) override;
  // void MouseRReleaseAction() override;
  // void MouseDragAction(const IMouseMod& mod) override;
  void MouseLDragAction(float dX, float dY, const IMouseMod& mod) override;
  // void MouseRDragAction(float dX, float dY, const IMouseMod& mod) override;
  void OnMouseWheel(float x, float y, const IMouseMod& mod, float d) override;

  void SetGearing(double gearing) { mGearing = gearing; }
  bool IsFineControl(const IMouseMod& mod, bool wheel) const;

protected:
  bool mHideCursorOnDrag = true;
  EDirection mDirection;
  IRECT mTrackBounds;
  float mHandleSize;
  double mGearing;
  double mMouseLDownValue;
  double mMouseRDownValue;
};

RCSliderControl::RCSliderControl(const IRECT& bounds, int paramIdx, EDirection dir, double gearing, float handleSize)
  : RCControl(bounds, paramIdx)
  , mDirection(dir)
  , mHandleSize(handleSize)
  , mGearing(gearing)
{
}

RCSliderControl::RCSliderControl(const IRECT& bounds, IActionFunction aF, EDirection dir, double gearing, float handleSize)
  : RCControl(bounds, aF)
  , mDirection(dir)
  , mHandleSize(handleSize)
  , mGearing(gearing)
{
}

void RCSliderControl::OnResize()
{
  SetTargetRECT(mRECT);
  mTrackBounds = mRECT;
  SetDirty(false);
}

void RCSliderControl::MouseUpAction(const IMouseMod& mod)
{
  if (mHideCursorOnDrag)
    GetUI()->HideMouseCursor(false);
}

void RCSliderControl::MouseLPressAction(const IMouseMod& mod) { mMouseLDownValue = GetValue(); }

void RCSliderControl::MouseRPressAction(const IMouseMod& mod) { mMouseRDownValue = GetValue(); }

void RCSliderControl::MouseRClickAction(const IMouseMod& mod)
{
  if (!mMouseControl.IsLPressing())
    SetValueToDefault(GetValIdxForPos(mMouseControl.cur_x, mMouseControl.cur_y));
  mMouseControl.ReleaseL();
}

void RCSliderControl::OnMouseDblClick(float x, float y, const IMouseMod& mod) { PromptUserInput(GetValIdxForPos(x, y)); }

void RCSliderControl::MouseLDragAction(float dX, float dY, const IMouseMod& mod)
{
  const IParam* pParam = GetParam();
  const float x = mMouseControl.cur_x;
  const float y = mMouseControl.cur_y;

  double gearing = IsFineControl(mod, false) ? mGearing * 10.0 : mGearing;

  if (mDirection == EDirection::Vertical)
    mMouseControl.lmb.d_pos += static_cast<double>(dY / static_cast<double>(mTrackBounds.H()) / gearing);
  else
    mMouseControl.lmb.d_pos += static_cast<double>(dX / static_cast<double>(mTrackBounds.W()) / gearing);

  double v = Clip(mMouseControl.lmb.d_pos + mMouseLDownValue, 0., 1.);

  if (pParam && pParam->GetStepped() && pParam->GetStep() > 0)
    v = pParam->ConstrainNormalized(v);

  SetValue(v);
}

void RCSliderControl::OnMouseWheel(float x, float y, const IMouseMod& mod, float d)
{
  const double gearing = IsFineControl(mod, true) ? 0.001 : 0.01;
  double newValue = 0.0;
  const double oldValue = GetValue();
  const IParam* pParam = GetParam();

  if (pParam && pParam->GetStepped() && pParam->GetStep() > 0)
  {
    if (d != 0.f)
    {
      const double step = pParam->GetStep();
      double v = pParam->FromNormalized(oldValue);
      v += d > 0 ? step : -step;
      newValue = pParam->ToNormalized(v);
    }
    else
    {
      newValue = oldValue;
    }
  }
  else
  {
    newValue = oldValue + gearing * d;
  }

  SetValue(newValue);
  SetDirty();
}

bool RCSliderControl::IsFineControl(const IMouseMod& mod, bool wheel) const
{
#ifdef PROTOOLS
  #ifdef OS_WIN
  return mod.C;
  #else
  return wheel ? mod.C : mod.R;
  #endif
#else
  return (mod.C || mod.S);
#endif
}


END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
