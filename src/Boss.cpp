#include "Boss.h"
#include <iostream>
#include <cstdlib>

// ボスのコンストラクタ
Boss::Boss(int x, int y) : x(x), y(y), velX(0), velY(0), active(true), 
                          health(100), maxHealth(100), attackTimer(0), moveTimer(0), 
                          phase(0), isAttacking(false), isStunned(false), stunTimer(0), 
                          animationTimer(0), currentAttack(CHARGE_ATTACK) {
    rect = {x, y, 80, 100};  // 大きなボス
}

// ボスの更新処理
void Boss::Update() {
    if (!active) return;
    
    animationTimer += 0.1f;
    
    // スタン状態の処理
    if (isStunned) {
        stunTimer--;
        if (stunTimer <= 0) {
            isStunned = false;
        }
        return;
    }
    
    // タイマー更新
    attackTimer--;
    moveTimer--;
    
    // フェーズ管理（体力によってフェーズ変更）
    if (health < maxHealth * 0.7f && phase == 0) {
        phase = 1;  // フェーズ2: より攻撃的
        std::cout << "🔥 ボスがフェーズ2に移行！" << std::endl;
    } else if (health < maxHealth * 0.3f && phase == 1) {
        phase = 2;  // フェーズ3: 最も攻撃的
        std::cout << "💀 ボスが最終フェーズに移行！" << std::endl;
    }
    
    // 移動パターン
    if (moveTimer <= 0) {
        // プレイヤー方向に向かって移動
        float targetX = 400;  // プレイヤーの位置（仮）
        if (x < targetX) {
            velX = 1.0f + phase * 0.5f;
        } else {
            velX = -1.0f - phase * 0.5f;
        }
        moveTimer = 60 + (rand() % 120);  // 1-3秒
    }
    
    // 攻撃パターン
    if (attackTimer <= 0 && !isAttacking) {
        int attackType = rand() % 4;
        StartAttack(static_cast<AttackPattern>(attackType));
    }
    
    // 位置更新
    x += (int)velX;
    y += (int)velY;
    
    // 画面境界チェック
    if (x < 50) { x = 50; velX = 0; }
    if (x > 700) { x = 700; velX = 0; }
    
    // 矩形更新
    rect.x = x;
    rect.y = y;
}

// ボスの攻撃開始
void Boss::StartAttack(AttackPattern pattern) {
    if (isAttacking) return;
    
    currentAttack = pattern;
    isAttacking = true;
    attackTimer = 120 + (rand() % 180);  // 2-5秒後に次の攻撃
    
    switch (pattern) {
        case CHARGE_ATTACK:
            std::cout << "⚡ ボスが突進攻撃！" << std::endl;
            velX *= 3.0f;  // 高速移動
            break;
        case PROJECTILE_ATTACK:
            std::cout << "🔥 ボスが弾幕攻撃！" << std::endl;
            break;
        case SLAM_ATTACK:
            std::cout << "💥 ボスが地面攻撃！" << std::endl;
            break;
        case TELEPORT_ATTACK:
            std::cout << "👻 ボスがテレポート攻撃！" << std::endl;
            x = 200 + (rand() % 400);  // ランダム位置にテレポート
            break;
    }
}

// ボスのダメージ処理
void Boss::TakeDamage(int damage) {
    if (isStunned) return;
    
    health -= damage;
    if (health <= 0) {
        health = 0;
        active = false;
        std::cout << "💀 ボス撃破！" << std::endl;
    } else {
        // 一時的にスタン
        isStunned = true;
        stunTimer = 30;  // 0.5秒
        std::cout << "💥 ボスにダメージ！ 残りHP: " << health << std::endl;
    }
}

// ボスが倒されたかチェック
bool Boss::IsDefeated() {
    return health <= 0 || !active;
}

// BossProjectileのコンストラクタ
BossProjectile::BossProjectile(float x, float y, float velX, float velY)
    : x(x), y(y), velX(velX), velY(velY), active(true), lifeTime(300.0f) {
    rect = {(int)x, (int)y, 8, 8};
}

// BossProjectileの更新
void BossProjectile::Update() {
    if (!active) return;
    
    // 位置更新
    x += velX;
    y += velY;
    
    // 矩形更新
    rect.x = (int)x;
    rect.y = (int)y;
    
    // 生存時間減少
    lifeTime--;
    if (lifeTime <= 0) {
        active = false;
    }
    
    // 画面外チェック
    if (x < -50 || x > 850 || y < -50 || y > 650) {
        active = false;
    }
} 