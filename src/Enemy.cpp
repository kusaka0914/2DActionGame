#include "Enemy.h"
#include <iostream>
#include <cmath>

// === 敵の弾丸クラスの実装 ===
EnemyProjectile::EnemyProjectile(float x, float y, float velX, float velY, int damage) 
    : x(x), y(y), velX(velX), velY(velY), active(true), lifeTime(300.0f), damage(damage) {
    rect = {(int)x, (int)y, 6, 6};  // 小さな弾丸
}

void EnemyProjectile::Update() {
    if (!active) return;
    
    // 位置を更新
    x += velX;
    y += velY;
    
    // 矩形の位置を同期
    rect.x = (int)x;
    rect.y = (int)y;
    
    // 生存時間を減少
    lifeTime--;
    if (lifeTime <= 0) {
        active = false;
    }
}

bool EnemyProjectile::CheckCollisionWithPlayer(const SDL_Rect& playerRect) {
    if (!active) return false;
    return SDL_HasIntersection(&rect, &playerRect);
}

// === 敵キャラクタークラスのメソッド実装 ===
Enemy::Enemy(int x, int y, EnemyType type) 
    : x(x), y(y), velX(0), velY(0), speed(1), direction(-1), active(true),
      type(type), state(ENEMY_PATROL), health(1), maxHealth(1),
      detectionRange(150.0f), attackRange(100.0f), playerX(0), playerY(0), playerDetected(false),
      attackCooldown(0), attackTimer(0), attackDamage(1),
      patrolDistance(100), originalX(x), jumpCooldown(0), isOnGround(true),
      stateTimer(0), stunTimer(0), animationTimer(0.0f) {
    
    rect = {x, y, 30, 30};
    
    // 敵タイプに応じて初期設定
    switch (type) {
        case ENEMY_GOOMBA:
            speed = 1;
            health = maxHealth = 1;
            detectionRange = 80.0f;
            attackRange = 30.0f;
            break;
        case ENEMY_SHOOTER:
            speed = 0;  // 射撃敵は移動しない
            health = maxHealth = 2;
            detectionRange = 200.0f;
            attackRange = 180.0f;
            break;
        case ENEMY_JUMPER:
            speed = 2;
            health = maxHealth = 2;
            detectionRange = 120.0f;
            attackRange = 50.0f;
            break;
        case ENEMY_CHASER:
            speed = 3;
            health = maxHealth = 3;
            detectionRange = 250.0f;
            attackRange = 40.0f;
            break;
        case ENEMY_FLYING:
            speed = 2;
            health = maxHealth = 2;
            detectionRange = 180.0f;
            attackRange = 80.0f;
            velY = -1;  // 初期的に上下移動
            break;
    }
}

void Enemy::Update(int playerX, int playerY, int map[19][100]) {
    if (!active) return;
    
    // プレイヤー位置を保存
    this->playerX = playerX;
    this->playerY = playerY;
    
    // アニメーションタイマー更新
    animationTimer += 0.1f;
    
    // スタン状態の処理
    if (stunTimer > 0) {
        stunTimer--;
        state = ENEMY_STUNNED;
        return;
    }
    
    // AI更新
    UpdateAI(playerX, playerY);
    
    // 移動更新
    UpdateMovement(map);
    
    // 攻撃更新
    UpdateAttack();
    
    // 矩形更新
    rect.x = x;
    rect.y = y;
}

// 敵のAI更新処理
void Enemy::UpdateAI(int playerX, int playerY) {
    // プレイヤー検出
    CheckPlayerDetection(playerX, playerY);
    
    // 状態タイマー更新
    stateTimer++;
    
    // 状態に応じたAI
    switch (state) {
        case ENEMY_PATROL:
            // 巡回状態：通常の移動パターン
            if (playerDetected && DistanceToPlayer(playerX, playerY) < detectionRange) {
                state = ENEMY_ALERT;
                stateTimer = 0;
            }
            break;
            
        case ENEMY_ALERT:
            // 警戒状態：プレイヤーに向かって移動
            if (DistanceToPlayer(playerX, playerY) < attackRange) {
                state = ENEMY_ATTACK;
                stateTimer = 0;
            } else if (DistanceToPlayer(playerX, playerY) > detectionRange * 1.5f) {
                state = ENEMY_PATROL;
                playerDetected = false;
                stateTimer = 0;
            } else {
                // プレイヤーに向かって移動
                if (playerX < x) {
                    direction = -1;
                } else {
                    direction = 1;
                }
            }
            break;
            
        case ENEMY_ATTACK:
            // 攻撃状態：攻撃を実行
            if (DistanceToPlayer(playerX, playerY) > attackRange * 1.2f) {
                state = ENEMY_ALERT;
                stateTimer = 0;
            }
            break;
            
        case ENEMY_STUNNED:
            // スタン状態：何もしない
            break;
    }
}

// 敵の移動更新処理
void Enemy::UpdateMovement(int map[19][100]) {
    // 敵タイプに応じた移動パターン
    switch (type) {
        case ENEMY_GOOMBA:
            // 基本的な歩行敵：水平移動のみ
            if (state != ENEMY_STUNNED) {
                velX = speed * direction;
            } else {
                velX = 0;
            }
            break;
            
        case ENEMY_SHOOTER:
            // 射撃敵：移動しない
            velX = 0;
            break;
            
        case ENEMY_JUMPER:
            // ジャンプ敵：ジャンプで移動
            if (state == ENEMY_ALERT || state == ENEMY_ATTACK) {
                if (jumpCooldown <= 0 && isOnGround) {
                    velY = -8;  // ジャンプ
                    jumpCooldown = 60;  // 1秒のクールダウン
                }
                velX = speed * direction * 0.5f;  // 空中でも少し移動
            } else {
                velX = speed * direction * 0.3f;  // 巡回時は遅め
            }
            break;
            
        case ENEMY_CHASER:
            // 追跡敵：プレイヤーに向かって高速移動
            if (state == ENEMY_ALERT || state == ENEMY_ATTACK) {
                velX = speed * direction * 1.5f;  // 高速移動
            } else {
                velX = speed * direction * 0.5f;  // 巡回時は普通
            }
            break;
            
        case ENEMY_FLYING:
            // 飛行敵：上下左右に移動
            if (state == ENEMY_ALERT || state == ENEMY_ATTACK) {
                velX = speed * direction;
                // プレイヤーのY座標に向かって移動
                if (playerY < y - 10) {
                    velY = -1;
                } else if (playerY > y + 10) {
                    velY = 1;
                } else {
                    velY = 0;
                }
            } else {
                velX = speed * direction * 0.5f;
                // 上下に波のような動き
                velY = sin(animationTimer * 0.1f) * 2;
            }
            break;
    }
    
    // 重力適用（飛行敵以外）
    if (type != ENEMY_FLYING && !isOnGround) {
        velY += 0.5f;  // 重力
    }
    
    // ジャンプクールダウン更新
    if (jumpCooldown > 0) {
        jumpCooldown--;
    }
    
    // 位置更新
    x += (int)velX;
    y += (int)velY;
    
    // 簡単な地面判定（詳細な実装は後で）
    int tileY = (y + rect.h) / 32;
    if (tileY < 19 && tileY >= 0) {
        int tileX = (x + rect.w / 2) / 32;
        if (tileX < 100 && tileX >= 0) {
            if (map[tileY][tileX] == 1) {
                y = tileY * 32 - rect.h;
                velY = 0;
                isOnGround = true;
            } else {
                isOnGround = false;
            }
        }
    }
    
    // 画面境界チェック
    if (x < 0) {
        x = 0;
        direction = 1;
    } else if (x > 100 * 32 - rect.w) {
        x = 100 * 32 - rect.w;
        direction = -1;
    }
}

// 敵の攻撃更新処理
void Enemy::UpdateAttack() {
    // 攻撃クールダウン更新
    if (attackCooldown > 0) {
        attackCooldown--;
    }
    
    // 攻撃状態で攻撃可能な場合
    if (state == ENEMY_ATTACK && attackCooldown <= 0) {
        PerformAttack();
    }
}

// 敵のプレイヤー検出処理
void Enemy::CheckPlayerDetection(int playerX, int playerY) {
    float distance = DistanceToPlayer(playerX, playerY);
    
    if (distance < detectionRange && CanSeePlayer(playerX, playerY, nullptr)) {
        playerDetected = true;
    }
}

// 敵の攻撃実行
void Enemy::PerformAttack() {
    attackCooldown = 120;  // 2秒のクールダウン
    
    // 敵タイプに応じた攻撃パターン
    switch (type) {
        case ENEMY_GOOMBA:
            // 近接攻撃（ダメージは衝突判定で処理）
            break;
            
        case ENEMY_SHOOTER:
            // 射撃攻撃：プレイヤーに向けて弾丸発射
            {
                float dx = playerX - x;
                float dy = playerY - y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance > 0) {
                    float speed = 4.0f;
                    float velX = (dx / distance) * speed;
                    float velY = (dy / distance) * speed;
                    
                    // 弾丸生成は後でGameクラスから呼び出す
                    // SpawnEnemyProjectile(x + rect.w/2, y + rect.h/2, velX, velY);
                }
            }
            break;
            
        case ENEMY_JUMPER:
            // ジャンプ攻撃
            if (isOnGround) {
                velY = -10;  // 高いジャンプ
                velX = direction * speed * 2;  // 横方向にも高速移動
            }
            break;
            
        case ENEMY_CHASER:
            // 突進攻撃
            velX = direction * speed * 3;  // 非常に高速な突進
            break;
            
        case ENEMY_FLYING:
            // 急降下攻撃
            velY = 6;  // 下向きに高速移動
            break;
    }
}

// 敵がダメージを受ける
void Enemy::TakeDamage(int damage) {
    health -= damage;
    stunTimer = 30;  // 0.5秒スタン
    
    if (health <= 0) {
        active = false;
    }
}

// プレイヤーまでの距離を計算
float Enemy::DistanceToPlayer(int playerX, int playerY) {
    float dx = playerX - x;
    float dy = playerY - y;
    return sqrt(dx * dx + dy * dy);
}

// プレイヤーが見えるかチェック（簡易版）
bool Enemy::CanSeePlayer(int playerX, int playerY, int map[19][100]) {
    // 簡単な実装：距離のみチェック
    return DistanceToPlayer(playerX, playerY) < detectionRange;
} 