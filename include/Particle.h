#pragma once

#include <SDL.h>

#include "ColorPalette.h"

// パーティクルの種類を定義する列挙型
enum ParticleType {
    PARTICLE_SPARK = 0,        // 火花
    PARTICLE_SMOKE = 1,        // 煙
    PARTICLE_DASH_TRAIL = 2,   // ダッシュ軌跡
    PARTICLE_SOUL = 3,         // 魂
    PARTICLE_EXPLOSION = 4     // 爆発
};

// パーティクルクラス: ゲーム内のエフェクト用パーティクル
class Particle {
public:
    float x, y;              // パーティクルの位置
    float velX, velY;        // パーティクルの速度
    SDL_Color color;         // パーティクルの色
    ParticleType type;       // パーティクルの種類
    float life;              // 残り生存時間
    float maxLife;           // 最大生存時間
    bool active;             // パーティクルがアクティブかどうか
    float size;              // パーティクルのサイズ
    float gravity;           // 重力の影響
    float friction;          // 摩擦（速度減衰）
    
    // コンストラクタ
    Particle(float x, float y, float velX, float velY, ParticleType type, float life = 60.0f);
    
    // 更新処理
    void Update();
    
    // 描画処理
    void Render(SDL_Renderer* renderer);
    
    // 生存判定
    bool IsAlive();
}; 