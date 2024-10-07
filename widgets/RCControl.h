#include "IControl.h"
#include "IControls.h"
#include "IGraphics.h"
#include "IGraphicsStructs.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

struct MouseButtonStatus
{
  enum Status
  {
    Released,
    Pressing,
    Dragging,
    Clicked,
  };

  MouseButtonStatus() {};

  float press_x = -1.;
  float press_y = -1.;
  Status status = Status::Released;

  bool IsReleased() { return status == Status::Released; };
  bool IsPressing() { return status == Status::Pressing; };
  bool IsDragging() { return status == Status::Dragging; };
  bool IsClicked() { return status == Status::Clicked; };

  void Release() { status = Status::Released; };
  void SetStatus(float x, float y, bool isHovering, bool isOn)
  {
    switch (_CheckPressing(isHovering, isOn))
    {
    case 0:
      status = Status::Released;
      press_x = -1.;
      press_y = -1.;
      break;
    case 1:
      status = Status::Pressing;
      press_x = x;
      press_y = y;
      break;
    case 2:
      if (IsMouseMovedAfterPressing(x, y))
        status = Status::Released;
      else
        status = Status::Clicked;
      break;
    };
  };

  int _CheckPressing(bool isHovering, bool isOn)
  {
    switch (status)
    {
    case Status::Released:
      if (isHovering && isOn)
        return 1;
      break;
    case Status::Pressing:
    case Status::Dragging:
      if (!isOn)
        return 2;
    }
    return 0;
  };

  bool IsMouseMovedAfterPressing(float x, float y) { return press_x == x && press_y == y; };
};

struct MouseControl
{

  enum MouseClick
  {
    Released = 0,
    Pressing = 1,
    Dragging = 2,
    Clicked = 4,
  };

  MouseControl() {};

  float cur_x = -1.;
  float cur_y = -1.;
  float l_press_x = -1.;
  float l_press_y = -1.;
  float r_press_x = -1.;
  float r_press_y = -1.;
  bool isHovering = false;
  int lmb = MouseClick::Released;
  int rmb = MouseClick::Released;

  bool IsHovering() { return isHovering; };
  bool IsAnyPressing() { return IsLPressing() || IsRPressing(); };
  bool IsLPressing() { return lmb & MouseClick::Pressing; };
  bool IsRPressing() { return rmb & MouseClick::Pressing; };
  bool IsLDragging() { return lmb & MouseClick::Dragging; };
  bool IsRDragging() { return rmb & MouseClick::Dragging; };
  bool IsLClicked() { return lmb & MouseClick::Clicked; };
  bool IsRClicked() { return rmb & MouseClick::Clicked; };

  void Release() { lmb = rmb = MouseClick::Released; };
  void ReleaseL() { lmb = MouseClick::Released; };
  void ReleaseR() { rmb = MouseClick::Released; };
  void SetHovering(bool state = true)
  {
    isHovering = false;
    if (state && lmb == 0 && rmb == 0)
      isHovering = true;
  };

  void SetPressing(const IMouseMod& mod)
  {
    switch (_CheckPressing(lmb, mod.L))
    {
    case 0:
      lmb = MouseClick::Released;
      l_press_x = -1.;
      l_press_y = -1.;
      break;
    case 1:
      lmb = MouseClick::Pressing;
      l_press_x = cur_x;
      l_press_y = cur_y;
      break;
    case 2:
      if (_CheckClickPosition(l_press_x, l_press_y))
        lmb = MouseClick::Clicked;
      else
        lmb = MouseClick::Released;
      break;
    };
    switch (_CheckPressing(rmb, mod.R))
    {
    case 0:
      rmb = MouseClick::Released;
      r_press_x = -1.;
      r_press_y = -1.;
      break;
    case 1:
      rmb = MouseClick::Pressing;
      r_press_x = cur_x;
      r_press_y = cur_y;
      break;
    case 2:
      if (_CheckClickPosition(r_press_x, r_press_y))
        rmb = MouseClick::Clicked;
      else
        rmb = MouseClick::Released;
      break;
    };
  };

  int _CheckPressing(int mb, bool state)
  {
    switch (mb)
    {
    case MouseClick::Released:
      if (isHovering && state)
        return 1;
      break;
    case MouseClick::Pressing:
    case MouseClick::Dragging:
      if (!state)
        return 2;
    }
    return 0;
  };
  bool _CheckClickPosition(float x, float y) { return cur_x == x && cur_y == y; };
  void UpdatePosition(float x, float y)
  {
    cur_x = x;
    cur_y = y;
  };
};

class RCControl : public IControl
{
public:
  RCControl(const IRECT& bounds, int paramIdx)
    : IControl(bounds, paramIdx) {};
  RCControl(const IRECT& bounds, IActionFunction aF = nullptr)
    : IControl(bounds, aF) {};

  virtual ~RCControl() {};
  void OnMouseOver(float x, float y, const IMouseMod& mod) override;
  void OnMouseOut() override;
  void OnMouseDown(float x, float y, const IMouseMod& mod) override;
  void OnMouseUp(float x, float y, const IMouseMod& mod) override;
  void OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod) override;

  virtual void MouseOverAction(const IMouseMod& mod) {};
  virtual void MouseOutAction() {};
  virtual void MouseDownAction(const IMouseMod& mod) {};
  virtual void MouseUpAction(const IMouseMod& mod) {};
  virtual void MouseHoverAction(const IMouseMod& mod) {};
  virtual void MouseLPressAction(const IMouseMod& mod) {};
  virtual void MouseLClickAction(const IMouseMod& mod) {};
  virtual void MouseLReleaseAction() {};
  virtual void MouseRPressAction(const IMouseMod& mod) {};
  virtual void MouseRClickAction(const IMouseMod& mod) {};
  virtual void MouseRReleaseAction() {};
  virtual void MouseDragAction(const IMouseMod& mod) {};
  virtual void MouseLDragAction(float dX, float dY, const IMouseMod& mod) {};
  virtual void MouseRDragAction(float dX, float dY, const IMouseMod& mod) {};

protected:
  MouseControl mMouseControl = MouseControl();
};

void RCControl::OnMouseOver(float x, float y, const IMouseMod& mod)
{
  mMouseControl.UpdatePosition(x, y);
  bool prev = mMouseIsOver;
  mMouseIsOver = true;
  MouseOverAction(mod);
  if (prev == false)
  {
    mMouseControl.SetHovering(true);
    MouseHoverAction(mod);
    SetDirty(true, true);
  }
}

void RCControl::OnMouseOut()
{
  bool prev = mMouseIsOver;
  mMouseIsOver = false;
  if (prev == true)
  {
    mMouseControl.SetHovering(false);
    MouseOutAction();
    SetDirty(false);
  }
}

void RCControl::OnMouseDown(float x, float y, const IMouseMod& mod)
{
  mMouseControl.UpdatePosition(x, y);
  MouseDownAction(mod);
  mMouseControl.SetPressing(mod);
  if (mMouseControl.IsLPressing())
    MouseLPressAction(mod);
  if (mMouseControl.IsRPressing())
    MouseRPressAction(mod);
  SetDirty(true);
}

void RCControl::OnMouseUp(float x, float y, const IMouseMod& mod)
{
  mMouseControl.UpdatePosition(x, y);
  MouseUpAction(mod);
  mMouseControl.SetPressing(mod);
  if (mMouseControl.IsLClicked())
    MouseLClickAction(mod);
  if (mMouseControl.IsRClicked())
    MouseRClickAction(mod);
  SetDirty(false);
}

void RCControl::OnMouseDrag(float x, float y, float dX, float dY, const IMouseMod& mod)
{
  mMouseControl.UpdatePosition(x, y);
  MouseDragAction(mod);
  if (mMouseControl.IsLPressing())
    MouseLDragAction(dX, dY, mod);
  if (mMouseControl.IsRPressing())
    MouseRDragAction(dX, dY, mod);
  if (dX || dY)
  {
    SetDirty(true);
  }
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
