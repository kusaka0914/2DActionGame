#include "ColorPalette.h"

// カラーパレット定数の実装
// 背景色（ホロウナイト風のダークトーン）
const SDL_Color ColorPalette::BACKGROUND_DARK = {15, 20, 35, 255};    // 深い青黒
const SDL_Color ColorPalette::BACKGROUND_MID = {25, 35, 55, 255};     // 中間の青
const SDL_Color ColorPalette::BACKGROUND_LIGHT = {40, 50, 70, 255};   // 薄い青

// プレイヤー色（神秘的な白と青）
const SDL_Color ColorPalette::PLAYER_PRIMARY = {220, 230, 240, 255};  // 淡い白
const SDL_Color ColorPalette::PLAYER_SECONDARY = {150, 180, 220, 255}; // 淡い青
const SDL_Color ColorPalette::PLAYER_GLOW = {180, 200, 255, 100};     // 青い光

// 環境色（石や岩の色調）
const SDL_Color ColorPalette::TILE_MAIN = {60, 70, 85, 255};          // メインの灰青色
const SDL_Color ColorPalette::TILE_EDGE = {80, 90, 110, 255};         // エッジの明るい色
const SDL_Color ColorPalette::TILE_SHADOW = {30, 35, 45, 255};        // 影の暗い色

// UI色（視認性の良い色）
const SDL_Color ColorPalette::UI_PRIMARY = {200, 210, 230, 255};      // メインUI色
const SDL_Color ColorPalette::UI_SECONDARY = {120, 140, 180, 255};    // サブUI色
const SDL_Color ColorPalette::UI_ACCENT = {100, 200, 255, 255};       // アクセント色

// エフェクト色
const SDL_Color ColorPalette::SOUL_BLUE = {120, 180, 255, 255};       // 魂の青
const SDL_Color ColorPalette::DAMAGE_RED = {255, 80, 80, 255};        // ダメージの赤
const SDL_Color ColorPalette::HEALTH_GREEN = {80, 255, 120, 255};     // 体力の緑 