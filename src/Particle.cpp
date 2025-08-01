#include "Particle.h"
#include <cstdlib>

// パーティクルのコンストラクタ
Particle::Particle(float x, float y, float velX, float velY, ParticleType type, float life)
    : x(x), y(y), velX(velX), velY(velY), type(type), life(life), maxLife(life), 
      active(true), size(4.0f), gravity(0.1f), friction(0.98f) {
    
    // パーティクルタイプに応じて色とプロパティを設定
    switch(type) {
        case PARTICLE_SPARK:
            color = ColorPalette::UI_ACCENT;  // 青いアクセント色
            gravity = 0.2f;
            break;
        case PARTICLE_SMOKE:
            color = ColorPalette::TILE_SHADOW;  // 暗いグレー
            color.a = 120;  // 半透明
            gravity = -0.05f;  // 上昇する
            friction = 0.95f;
            break;
        case PARTICLE_DASH_TRAIL:
            color = ColorPalette::PLAYER_GLOW;  // プレイヤーの光色
            gravity = 0.0f;  // 重力無し
            friction = 0.9f;
            size = 6.0f;
            break;
        case PARTICLE_SOUL:
            color = ColorPalette::SOUL_BLUE;  // 魂の青
            gravity = -0.1f;  // 少し上昇
            friction = 0.99f;
            size = 5.0f;
            break;
        case PARTICLE_EXPLOSION:
            color = ColorPalette::DAMAGE_RED;  // ダメージの赤
            gravity = 0.15f;
            friction = 0.96f;
            size = 3.0f + (rand() % 4);  // ランダムサイズ
            break;
    }
}

// パーティクルの更新
void Particle::Update() {
    if (!active) return;
    
    // 位置更新
    x += velX;
    y += velY;
    
    // 速度更新
    velY += gravity;
    velX *= friction;
    velY *= friction;
    
    // 生存時間減少
    life--;
    
    // アルファ値を生存時間に応じて変更
    float lifeRatio = life / maxLife;
    if (type == PARTICLE_DASH_TRAIL || type == PARTICLE_SOUL) {
        color.a = (Uint8)(180 * lifeRatio);
    } else {
        color.a = (Uint8)(255 * lifeRatio);
    }
    
    // サイズを徐々に小さくする（一部タイプ）
    if (type == PARTICLE_EXPLOSION) {
        size = size * 0.99f;
    }
    
    // 生存時間チェック
    if (life <= 0) {
        active = false;
    }
}

// パーティクルの描画
void Particle::Render(SDL_Renderer* renderer) {
    if (!active) return;
    
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    // パーティクルタイプに応じて描画方法を変更
    if (type == PARTICLE_DASH_TRAIL) {
        // ダッシュ軌跡は少し大きめの矩形
        SDL_Rect rect = {(int)(x - size/2), (int)(y - size/2), (int)size, (int)size};
        SDL_RenderFillRect(renderer, &rect);
    } else {
        // その他は小さな矩形or点
        for (int i = 0; i < (int)size; i++) {
            for (int j = 0; j < (int)size; j++) {
                SDL_RenderDrawPoint(renderer, (int)x + i - (int)size/2, (int)y + j - (int)size/2);
            }
        }
    }
}

// パーティクルが生存しているかチェック
bool Particle::IsAlive() {
    return active && life > 0;
} 