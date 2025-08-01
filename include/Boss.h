#ifndef BOSS_H
#define BOSS_H

#include <SDL.h>

// ボスクラス: ホロウナイト風の強力な敵
class Boss {
public:
    int x, y;                   // ボスの位置座標
    float velX, velY;           // ボスの速度
    SDL_Rect rect;              // 描画・衝突判定用の矩形
    bool active;                // ボスが有効かどうか
    int health;                 // ボスのHP
    int maxHealth;              // ボスの最大HP
    int attackTimer;            // 攻撃タイマー
    int moveTimer;              // 移動パターンタイマー
    int phase;                  // ボスの行動フェーズ
    bool isAttacking;           // 攻撃中フラグ
    bool isStunned;             // スタン状態
    int stunTimer;              // スタン時間
    float animationTimer;       // アニメーションタイマー
    
    // ボスの攻撃パターン
    enum AttackPattern {
        CHARGE_ATTACK = 0,      // 突進攻撃
        PROJECTILE_ATTACK = 1,  // 弾幕攻撃
        SLAM_ATTACK = 2,        // 地面叩きつけ
        TELEPORT_ATTACK = 3     // テレポート攻撃
    };
    
    AttackPattern currentAttack;
    
    Boss(int x, int y);
    void Update();
    void StartAttack(AttackPattern pattern);
    void TakeDamage(int damage);
    bool IsDefeated();
};

// ボスの弾丸クラス
class BossProjectile {
public:
    float x, y;                 // 弾丸の位置
    float velX, velY;           // 弾丸の速度
    SDL_Rect rect;              // 衝突判定用の矩形
    bool active;                // 弾丸が有効かどうか
    float lifeTime;             // 弾丸の生存時間
    
    BossProjectile(float x, float y, float velX, float velY);
    void Update();
};

#endif // BOSS_H 