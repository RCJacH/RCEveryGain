#pragma once

#include "IControl.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

/** A base class for switch controls */
class RCSwitchControlBase : public IControl
{
public:
  RCSwitchControlBase(const IRECT& bounds, int paramIdx = kNoParameter, IActionFunction aF = nullptr, int numStates = 2);

  virtual ~RCSwitchControlBase() {}

  void OnInit() override;
  int GetSelectedIdx() const { return int(0.5 + GetValue() * (double)(mNumStates - 1)); }

  void SetStateDisabled(int stateIdx, bool disabled);
  void SetAllStatesDisabled(bool disabled);
  bool GetStateDisabled(int stateIdx) const;

protected:
  int mNumStates;
  WDL_TypedBuf<bool> mDisabledState;
};

RCSwitchControlBase::RCSwitchControlBase(const IRECT& bounds, int paramIdx, IActionFunction aF, int numStates)
  : IControl(bounds, paramIdx, aF)
  , mNumStates(numStates)
{
  mDisabledState.Resize(numStates);
  SetAllStatesDisabled(false);
  mDblAsSingleClick = true;
}

void RCSwitchControlBase::SetAllStatesDisabled(bool disabled)
{
  for (int i = 0; i < mNumStates; i++)
  {
    SetStateDisabled(i, disabled);
  }
  SetDirty(false);
}

void RCSwitchControlBase::SetStateDisabled(int stateIdx, bool disabled)
{
  if (stateIdx >= 0 && stateIdx < mNumStates && mDisabledState.GetSize())
    mDisabledState.Get()[stateIdx] = disabled;

  SetDirty(false);
}

bool RCSwitchControlBase::GetStateDisabled(int stateIdx) const
{
  if (stateIdx >= 0 && stateIdx < mNumStates && mDisabledState.GetSize())
    return mDisabledState.Get()[stateIdx];
  return false;
}

void RCSwitchControlBase::OnInit()
{
  if (GetParamIdx() > kNoParameter)
    mNumStates = (int)GetParam()->GetRange() + 1;

  assert(mNumStates > 1);
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
