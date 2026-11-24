#pragma once

#include <EpdFontFamily.h>

#include <functional>
#include <memory>

#include "CrossPointSettings.h"
#include "components/themes/BaseTheme.h"

class UITheme {
  // Static instance
  static UITheme instance;

 public:
  enum class TextVerticalAlignment { TOP, CENTER, BOTTOM };

  UITheme();
  static UITheme& getInstance() { return instance; }

  const ThemeMetrics& getMetrics() const;
  const BaseTheme& getTheme() const { return currentTheme ? *currentTheme : fallbackTheme; }
  // The drawable content area: full screen minus the board's viewable insets
  // (bezel / rounded-corner clearance from BoardConfig::ACTIVE.viewableInsets,
  // oriented to the current rotation). THE single place activities should get
  // their content bounds from so every screen clears the bezel identically.
  // Zero-inset on rectangular panels, so no visible change there.
  static Rect getContentArea(const GfxRenderer& renderer);
  Rect getScreenSafeArea(const GfxRenderer& renderer, bool hasFrontButtonHints = false,
                         bool hasSideButtonHints = false);
  // Header rect for GUI.drawHeader() on a screen laid out in getScreenSafeArea():
  // clear of the button hints but NOT inset for the bezel, since the header band
  // applies the board's viewable insets itself.
  Rect getHeaderRect(const GfxRenderer& renderer, bool hasFrontButtonHints = false, bool hasSideButtonHints = false);
  static void drawCenteredText(const GfxRenderer& renderer, Rect screen, int fontId, int y, const char* text,
                               bool black = true, EpdFontFamily::Style style = EpdFontFamily::REGULAR);
  // Wraps only overflowing text, then aligns the complete line block within bounds.
  static void drawCenteredWrappedText(const GfxRenderer& renderer, Rect bounds, int fontId, const char* text,
                                      int maxLines, bool black = true,
                                      EpdFontFamily::Style style = EpdFontFamily::REGULAR,
                                      TextVerticalAlignment verticalAlignment = TextVerticalAlignment::CENTER);
  void reload();
  static std::string getCoverThumbPath(std::string coverBmpPath, int coverHeight);
  static UIIcon getFileIcon(const std::string& filename);
  static int getStatusBarHeight();
  static int getProgressBarHeight();
  static int getReaderStatusBarHeight();
  // Portrait button-hint band depth at the bottom edge. In landscape the hints
  // form a side rail that is already part of the fui safe area (see
  // UiAppHost::railSafeDeviceContext), so landscape screens must not reserve a
  // bottom band for it too.
  static int16_t getHintBandBottomInset(const GfxRenderer& renderer);
  // Rail depth on the edge the hint rail occupies in landscape (left in
  // clockwise, right in counter-clockwise); 0 in portrait and on touch boards.
  static int16_t getHintRailLeftGutter(const GfxRenderer& renderer);
  static int16_t getHintRailRightGutter(const GfxRenderer& renderer);
  static bool isLandscape(const GfxRenderer& renderer);
  // Both rail gutters at once (both 0 in portrait), for screens that reserve
  // the whole rail. getHintRailLeftGutter/RightGutter stay for single-edge needs.
  struct HintRails {
    int16_t left = 0;
    int16_t right = 0;
  };
  static HintRails getHintRails(const GfxRenderer& renderer);

 private:
  BaseTheme fallbackTheme;
  const ThemeMetrics* currentMetrics = &BaseMetrics::values;
  std::unique_ptr<BaseTheme> currentTheme;
  mutable ThemeMetrics adjustedMetrics;
  mutable bool metricsValid = false;
  mutable bool metricsForTouch = false;
};

// Helper macro to access current theme
#define GUI UITheme::getInstance().getTheme()
