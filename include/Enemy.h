#pragma once

#include <SDL.h>
#include <cmath>

// 敵の種類を定義する列挙型
enum EnemyType {
    ENEMY_GOOMBA = 0,    // 基本的な歩行敵（クリボー風）
    ENEMY_SHOOTER = 1,   // 射撃敵
    ENEMY_JUMPER = 2,    // ジャンプ敵
    ENEMY_CHASER = 3,    // 追跡敵
    ENEMY_FLYING = 4     // 飛行敵
};

// 敵の状態を定義する列挙型
enum EnemyState {
    ENEMY_PATROL = 0,    // 巡回状態
    ENEMY_ALERT = 1,     // 警戒状態（プレイヤー発見）
    ENEMY_ATTACK = 2,    // 攻撃状態
    ENEMY_STUNNED = 3    // スタン状態
};

// 敵の弾丸クラス
class EnemyProjectile {
public:
    float x, y;              // 弾丸の位置
    float velX, velY;        // 弾丸の速度
    SDL_Rect rect;           // 衝突判定用の矩形
    bool active;             // 弾丸がアクティブかどうか
    float lifeTime;          // 弾丸の生存時間
    int damage;              // 弾丸のダメージ
    
    // コンストラクタ
    EnemyProjectile(float x, float y, float velX, float velY, int damage = 1);
    
    // 更新処理
    void Update();
    
    // プレイヤーとの衝突判定
    bool CheckCollisionWithPlayer(const SDL_Rect& playerRect);
};

// 敵キャラクタークラス: プレイヤーに敵対するNPC
class Enemy {
public:
    int x, y;                    // 敵の位置座標
    float velX, velY;            // 敵の速度
    int speed;                   // 敵の移動速度
    int direction;               // 敵の向き（-1=左、1=右）
    bool active;                 // 敵がアクティブかどうか
    SDL_Rect rect;               // 衝突判定用の矩形
    
    // 敵のタイプと状態
    EnemyType type;              // 敵の種類
    EnemyState state;            // 敵の現在の状態
    int health;                  // 敵の体力
    int maxHealth;               // 敵の最大体力
    
    // AI関連
    float detectionRange;        // プレイヤー検出範囲
    float attackRange;           // 攻撃範囲
    int playerX, playerY;        // プレイヤーの位置（記録用）
    bool playerDetected;         // プレイヤーが検出されたかどうか
    
    // 攻撃関連
    int attackCooldown;          // 攻撃のクールダウン
    int attackTimer;             // 攻撃タイマー
    int attackDamage;            // 攻撃ダメージ
    
    // 移動パターン関連
    int patrolDistance;          // 巡回距離
    int originalX;               // 初期X座標（巡回の基準点）
    int jumpCooldown;            // ジャンプのクールダウン
    bool isOnGround;             // 地面にいるかどうか
    
    // 状態管理
    int stateTimer;              // 状態継続時間
    int stunTimer;               // スタン時間
    float animationTimer;        // アニメーション用タイマー
    
    // コンストラクタ: x,y位置と敵種類を指定して初期化
    Enemy(int x, int y, EnemyType type);
    
    // 敵の状態を1フレーム分更新
    void Update(int playerX, int playerY, int map[19][100]);
    
    // AI更新処理
    void UpdateAI(int playerX, int playerY);
    
    // 移動更新処理
    void UpdateMovement(int map[19][100]);
    
    // 攻撃更新処理
    void UpdateAttack();
    
    // プレイヤー検出処理
    void CheckPlayerDetection(int playerX, int playerY);
    
    // 攻撃実行
    void PerformAttack();
    
    // ダメージを受ける
    void TakeDamage(int damage);
    
    // プレイヤーまでの距離を計算
    float DistanceToPlayer(int playerX, int playerY);
    
    // プレイヤーが見えるかチェック
    bool CanSeePlayer(int playerX, int playerY, int map[19][100]);
}; 