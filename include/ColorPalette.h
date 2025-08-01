#pragma once

#include <SDL.h>

// カラーパレット定義（ホロウナイト風）
struct ColorPalette {
    // 背景色
    static const SDL_Color BACKGROUND_DARK;      // 深い青黒
    static const SDL_Color BACKGROUND_MID;       // 中間の青
    static const SDL_Color BACKGROUND_LIGHT;     // 薄い青
    
    // プレイヤー色
    static const SDL_Color PLAYER_PRIMARY;       // プレイヤーメイン色
    static const SDL_Color PLAYER_SECONDARY;     // プレイヤーアクセント色
    static const SDL_Color PLAYER_GLOW;          // プレイヤーの光
    
    // 環境色
    static const SDL_Color TILE_MAIN;            // タイルメイン色
    static const SDL_Color TILE_EDGE;            // タイルエッジ色
    static const SDL_Color TILE_SHADOW;          // タイル影色
    
    // UI色
    static const SDL_Color UI_PRIMARY;           // UIメイン色
    static const SDL_Color UI_SECONDARY;         // UIサブ色
    static const SDL_Color UI_ACCENT;            // UIアクセント色
    
    // エフェクト色
    static const SDL_Color SOUL_BLUE;            // 魂の青
    static const SDL_Color DAMAGE_RED;           // ダメージの赤
    static const SDL_Color HEALTH_GREEN;         // 体力の緑
}; 