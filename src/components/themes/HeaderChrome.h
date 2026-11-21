#pragma once
// Helpers for header status chrome that the fui header cannot place itself.
#include <FreeInkUICore.h>
#include <components/bars/battery-indicator.h>
#include <components/controls/header.h>

namespace freeink {
namespace ui {

// In landscape the button-hint rail sits over the edge corners where the fui
// header pins the battery and clock. BaseTheme draws those as one group
// centered on the status strip instead; pass the filled status props that were
// withheld from the header. Placement mirrors the header component's own
// measurements so chrome placement stays consistent across screens.
template <size_t MaxInteractions>
void drawCenteredHeaderStatus(Frame<MaxInteractions>& frame, const Rect& band, const HeaderStatusProps& status) {
  if (!status.showBattery && !status.clockText) return;
  DrawTarget& target = frame.target();
  int16_t batteryW = 0;
  if (status.showBattery) {
    batteryW = static_cast<int16_t>(status.battery.glyphWidth + 2);
    if (status.battery.label) {
      batteryW = static_cast<int16_t>(
          batteryW + status.battery.gap +
          target.measureText(status.battery.text.font, status.battery.label, status.battery.text).width);
    }
  }
  const int16_t clockW = status.clockText
                             ? target.measureText(status.battery.text.font, status.clockText, status.battery.text).width
                             : 0;
  const int16_t groupGap = batteryW > 0 && clockW > 0 ? status.battery.gap : 0;
  const int16_t groupX = static_cast<int16_t>(band.x + (band.width - (batteryW + clockW + groupGap)) / 2);
  if (batteryW > 0) {
    batteryIndicator(frame, Rect{groupX, band.y, batteryW, status.stripHeight}, status.battery);
  }
  if (clockW > 0) {
    const int16_t clockX = static_cast<int16_t>(groupX + batteryW + groupGap);
    const int16_t lineH = target.lineHeight(status.battery.text.font);
    const Rect ink = target.inkBounds(status.battery.text.font, status.clockText, status.battery.text);
    const int16_t clockY = static_cast<int16_t>(band.y + (status.stripHeight - ink.height) / 2 - ink.y);
    target.text(Rect{clockX, clockY, clockW, lineH}, status.clockText, status.battery.text);
  }
}

}  // namespace ui
}  // namespace freeink
