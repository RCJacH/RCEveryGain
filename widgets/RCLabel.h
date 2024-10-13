#pragma once

#include "IControl.h"
#include "widgets/RCStyle.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

class RCLabel : public ITextControl
{
public:
  RCLabel(const IRECT& bounds, const char* label, EDirection dir, const RCStyle& style = DEFAULT_RCSTYLE.WithDrawFrame(false), float gap = 0.f);
  void Draw(IGraphics& g) override;

protected:
  EDirection mDirection;
  RCStyle mStyle;
  float mGap;
};

RCLabel::RCLabel(const IRECT& bounds, const char* label, EDirection dir, const RCStyle& style, float gap)
  : ITextControl(bounds, label)
  , mDirection(dir)
  , mStyle(style)
  , mGap(gap)
{
  mText = style.valueText;
}

void RCLabel::Draw(IGraphics& g)
{
  auto colors = mStyle.GetColors();

  if (mStr.GetLength())
  {
    if (mStyle.drawFrame)
      g.DrawRect(colors.GetBorderColor(), mRECT, &mBlend, mStyle.frameThickness);

    const IText& text = mText.WithFGColor(colors.GetColor());
    const auto str = mStr.Get();
    const int length = mStr.GetLength();
    IRECT textBounds;
    IRECT remainingBounds = mRECT;
    IRECT charBounds;
    g.MeasureText(text, str, textBounds);
    switch (mDirection)
    {
    case EDirection::Horizontal:
      if (!mGap)
      {
        g.DrawText(text, str, remainingBounds, &mBlend);
        return;
      }

      remainingBounds.MidHPad((textBounds.W() + (length - 1) * mGap) * .5f);
      for (int i = 0; str[i] != '\0'; i++)
      {
        // Create a new const char* for each character
        const char singleChar[2] = {str[i], '\0'}; // Null-terminated string with 1 character
        const char* singleCharPtr = singleChar;
        g.MeasureText(text, singleCharPtr, charBounds);
        charBounds = remainingBounds.GetFromLeft(charBounds.W());
        g.DrawText(text, singleCharPtr, charBounds, &mBlend);
        remainingBounds.ReduceFromLeft(charBounds.W() + mGap);
      }
      break;
    case EDirection::Vertical:
      remainingBounds.MidVPad(((textBounds.H() + mGap) * length - mGap) * .5f);
      for (int i = 0; str[i] != '\0'; i++)
      {
        // Create a new const char* for each character
        const char singleChar[2] = {str[i], '\0'}; // Null-terminated string with 1 character
        const char* singleCharPtr = singleChar;
        g.MeasureText(text, singleCharPtr, charBounds);
        charBounds = remainingBounds.GetFromTop(charBounds.H());
        g.DrawText(text, singleCharPtr, charBounds, &mBlend);
        remainingBounds.ReduceFromTop(charBounds.H() + mGap);
      }
      break;
    }
  }
}

END_IGRAPHICS_NAMESPACE
END_IPLUG_NAMESPACE
