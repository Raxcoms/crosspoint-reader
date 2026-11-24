#pragma once
// Helpers for header status chrome that the fui header cannot place itself.
#include <FreeInkUICore.h>
#include <components/UITheme.h>
#include <components/bars/battery-indicator.h>
#include <components/controls/header.h>

namespace freeink {
namespace ui {

// Withhold the battery/clock chrome in landscape (the hint rail sits over the
// edge corners where the fui header pins them) so the caller can redraw it
// centered on the rail-safe band. Returns what was withheld (empty in
// portrait), mutating the props the header will render with.
inline HeaderStatusProps withholdCenteredStatus(const GfxRenderer& renderer, HeaderProps& header) {
  if (!UITheme::isLandscape(renderer)) return HeaderStatusProps{};
  const auto withheld = header.status;
  header.status.showBattery = false;
  header.status.clockText = nullptr;
  return withheld;
}

// The centered status group sits on the rail-safe band, so it centers on the
// screen's usable area, not the full-bleed band the header draws. Pass the band
// the header actually drew (rail-safe) so placement stays consistent.
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
  const int16_t clockW =
      status.clockText ? target.measureText(status.battery.text.font, status.clockText, status.battery.text).width : 0;
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

// The landscape hint rail is a full-height strip on the screen edge. This
// divider runs along the rail content edge from the header bottom rule to the
// screen bottom, meeting the rule at the corner without crossing it. It is
// drawn on the content side so the rail fill (painted later) never covers it.
inline void drawHeaderRailDivider(const GfxRenderer& renderer, const Rect& band, const UITheme::HintRails& rail) {
  if (rail.left == 0 && rail.right == 0) return;
  constexpr int dividerWidth = 3;
  const int dividerY = band.y + band.height;
  const int dividerHeight = renderer.getScreenHeight() - dividerY;
  if (dividerHeight <= 0) return;
  const int dividerX = rail.left > 0 ? band.x : band.x + band.width - dividerWidth;
  renderer.fillRect(dividerX, dividerY, dividerWidth, dividerHeight, true);
}

}  // namespace ui
}  // namespace freeink
