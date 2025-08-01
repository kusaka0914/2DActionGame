// ヘッダガード: このファイルが重複してインクルードされることを防ぐ
#pragma once

// SDL2のメインライブラリ: ウィンドウ作成、イベント処理、描画機能など
#include <SDL.h>
// SDL2の画像処理ライブラリ: PNG、JPEG等の画像ファイル読み込み
#include <SDL_image.h>
// SDL2のフォント描画ライブラリ: TTFフォントでのテキスト描画
#include <SDL_ttf.h>

// SDL_mixerが利用可能な場合のみインクルード
#ifdef SOUND_ENABLED
#include <SDL_mixer.h>
#endif
// C++標準ライブラリ: スマートポインタ（unique_ptr, shared_ptrなど）用
#include <memory>
// C++標準ライブラリ: 動的配列（std::vector）用
#include <vector>
// C++標準ライブラリ: 文字列処理
#include <string>
// C++標準ライブラリ: ファイル入出力
#include <fstream>

// 分離されたクラスファイルをインクルード
#include "ColorPalette.h"
#include "Item.h"
#include "Goal.h"
#include "Particle.h"
#include "Enemy.h"
#include "Boss.h"

// 衝突の種類を定義する列挙型
enum CollisionType {
    NO_COLLISION = 0,    // 衝突なし
    STOMP_ENEMY = 1,     // 敵を踏みつけ（上から）
    DAMAGED_BY_ENEMY = 2,// 敵からダメージ（横から）
    DASH_ATTACK_ENEMY = 3// ダッシュアタック（敵にダメージ）
};



// ステージデータ構造体
struct StageData {
    int stageNumber;                                    // ステージ番号
    std::string stageName;                              // ステージ名
    int mapData[19][100];                              // マップデータ（横スクロール対応）
    std::vector<std::pair<int, int>> enemyPositions;   // 敵の初期位置リスト
    std::vector<std::pair<std::pair<int, int>, ItemType>> itemPositions; // アイテム位置と種類
    GoalType goalType;                                  // ゴールの種類
    int goalX, goalY;                                   // ゴール位置
    int playerStartX, playerStartY;                     // プレイヤー開始位置
    int timeLimit;                                      // 制限時間（秒、0=無制限）
    std::string bgmFile;                                // BGMファイル名
    
    // コンストラクタ
    StageData() : stageNumber(1), stageName("Stage 1"), goalType(GOAL_FLAG), 
                  goalX(0), goalY(0), playerStartX(100), playerStartY(300), 
                  timeLimit(0), bgmFile("") {
        // マップを初期化（空で埋める）
        for (int y = 0; y < 19; y++) {
            for (int x = 0; x < 25; x++) {
                mapData[y][x] = 0;
            }
        }
    }
};






// ゲーム全体を管理するメインクラス
class Game {
public:  // 外部から呼び出せるメンバ（パブリック）
    // コンストラクタ: Gameオブジェクトが作成される時に呼ばれる
    Game();
    // デストラクタ: Gameオブジェクトが破棄される時に呼ばれる（メモリ解放など）
    ~Game();
    
    // ゲーム初期化関数: ウィンドウ作成、SDL初期化などを行う
    // title: ウィンドウのタイトル, x,y: ウィンドウ位置, width,height: ウィンドウサイズ, fullscreen: フルスクリーンかどうか
    bool Initialize(const char* title, int x, int y, int width, int height, bool fullscreen);
    // イベント処理関数: キーボード入力、マウス操作、ウィンドウ閉じるボタンなどを処理
    void HandleEvents();
    // ゲーム状態更新関数: プレイヤー位置、敵の動き、衝突判定などの計算を行う
    void Update();
    // 描画関数: 画面をクリアして、すべてのゲームオブジェクトを描画する
    void Render();
    // 終了処理関数: SDL関連のリソースを解放、メモリクリーンアップ
    void Clean();
    
    // ゲーム実行状態を確認する関数（const = この関数は内部データを変更しない）
    bool Running() const { return isRunning; }
    
    // 静的メンバ: すべてのGameインスタンスで共有される描画用レンダラー
    static SDL_Renderer* renderer;

private:  // クラス内部でのみアクセス可能なメンバ（プライベート）
    // ゲームループが継続中かどうかを示すフラグ（true=実行中, false=終了）
    bool isRunning;
    // SDL2のウィンドウオブジェクトへのポインタ
    SDL_Window* window;
    
    // === プレイヤー関連 ===
    // プレイヤーキャラクターのX, Y座標（画面上の位置）
    int playerX, playerY;
    // プレイヤーの水平移動速度（1フレームあたりのピクセル数）
    int playerSpeed;
    // プレイヤーを描画するための矩形データ（x, y, 幅, 高さを含む）
    SDL_Rect playerRect;
    
    // === プレイヤーパワーアップシステム ===
    // プレイヤーのパワーアップ状態（0=スモール, 1=ビッグ, 2=ファイア等）
    int playerPowerLevel;
    // ベースの移動速度
    int basePlayerSpeed;
    
    // === ホロウナイト風システム ===
    // ダッシュシステム
    bool canDash;                       // ダッシュ可能フラグ
    int dashCooldown;                   // ダッシュのクールダウン時間
    float dashSpeed;                    // ダッシュ時の移動速度
    int dashDuration;                   // ダッシュの持続時間
    int dashTimer;                      // ダッシュの残り時間
    bool isDashing;                     // ダッシュ中フラグ
    int lastDirection;                  // 最後の移動方向（1: 右, -1: 左）
    int dashCost;                       // ダッシュMP消費量

    // エアダッシュシステム
    int airDashCount;                // 現在の空中ダッシュ回数
    int maxAirDash;                  // 最大空中ダッシュ回数
    bool canAirDash;                 // 空中ダッシュが可能か
    float airDashSpeed;              // エアダッシュの速度
    int lastAirDashDirection;        // 最後のエアダッシュの方向
    
    // ダブルジャンプシステム
    int airJumpCount;                   // 現在の空中ジャンプ回数
    int maxAirJump;                     // 最大空中ジャンプ回数
    bool canAirJump;                    // 空中ジャンプ可能フラグ
    
    // 攻撃システム
    bool isAttacking;                // 攻撃中かどうか
    int attackCooldown;              // 攻撃のクールダウン
    int attackDuration;              // 攻撃の持続時間
    int attackTimer;                 // 攻撃タイマー
    SDL_Rect attackHitbox;           // 攻撃の当たり判定
    
    // 光線攻撃システム
    bool isChargingBeam;             // 光線チャージ中かどうか
    int beamChargeTime;              // 光線チャージ時間
    int maxBeamChargeTime;           // 最大チャージ時間
    bool isFiringBeam;               // 光線発射中かどうか
    int beamDuration;                // 光線持続時間
    int beamTimer;                   // 光線タイマー
    float beamDamage;                // 光線ダメージ
    int beamCost;                    // 光線MP消費量
    
    // ソウルシステム  
    int soulCount;                   // 現在の魂の数
    int maxSoul;                     // 最大魂数
    int playerHealth;                // プレイヤーのHP
    int maxHealth;                   // 最大HP
    int playerMP;                    // プレイヤーのMP
    int maxMP;                       // 最大MP
    
    // 移動システム拡張
    float playerVelX;                // X方向の速度（ダッシュ用）
    bool touchingWall;               // 壁に接触しているか
    bool canWallJump;                // ウォールジャンプが可能か
    int wallJumpCooldown;            // ウォールジャンプ後のクールダウン
    
    // 壁登りシステム
    bool isWallClimbing;             // 壁登り中フラグ
    int wallClimbStamina;            // 現在の壁登りスタミナ
    int maxWallClimbStamina;         // 最大壁登りスタミナ
    float wallClimbSpeed;            // 壁登り速度
    bool canWallClimb;               // 壁登り可能フラグ
    int wallClimbDirection;          // 壁登り方向（1: 右壁, -1: 左壁）
    int wallClimbTimer;              // 壁登り持続時間
    int wallSlideSpeed;              // 壁滑り速度
    
    // === エフェクトシステム ===
    // パーティクルシステム
    std::vector<Particle> particles; // パーティクル配列
    int particleLimit;               // パーティクル上限数
    
    // 画面シェイクシステム
    int screenShakeIntensity;        // シェイクの強度
    int screenShakeDuration;         // シェイクの持続時間
    float shakeOffsetX, shakeOffsetY; // シェイクのオフセット
    
    // === ビジュアルシステム ===
    // 背景グラデーション
    bool enableGradientBackground;   // グラデーション背景の有効化
    float gradientOffset;            // グラデーションのオフセット
    
    // プレイヤー光エフェクト
    float playerGlowIntensity;       // プレイヤーの光の強度
    float playerGlowTimer;           // 光のアニメーションタイマー
    
    // 環境効果
    float ambientDarkness;           // 環境の暗さ
    bool enableShadows;              // 影の有効化
    
    // === 物理システム（マリオ風ジャンプアクション用） ===
    // Y方向の速度（浮動小数点で精密な物理計算）
    float playerVelY;
    // 重力の強さ（毎フレーム速度に加算される）
    float gravity;
    // プレイヤーが地面に接触しているかのフラグ
    bool isOnGround;
    // ジャンプ中かどうかのフラグ
    bool isJumping;
    
    // === ゲーム状態システム ===
    // プレイヤーのスコア（敵を倒すと増加）
    int score;
    // プレイヤーのライフ（敵に当たると減少）
    int lives;
    // プレイヤーの初期位置（リスポーン用）
    int initialPlayerX, initialPlayerY;
    // ダメージを受けた後の無敵時間（フレーム数）
    int invincibilityTime;
    // 最大無敵時間（60フレーム = 1秒）
    static const int MAX_INVINCIBILITY_TIME = 60;
    
    // === タイマーシステム ===
    // ゲーム開始からの経過時間（秒）
    int gameTime;
    // フレームカウンター（60フレーム = 1秒）
    int frameCounter;
    
    // === UIシステム（画面上のテキスト表示） ===
    // フォントファイルのポインタ
    TTF_Font* font;
    // UIの描画エリア（画面上部）
    SDL_Rect uiArea;
    // UI背景色の透明度
    Uint8 uiBackgroundAlpha;
    
    // === サウンドシステム ===
#ifdef SOUND_ENABLED
    // 効果音データ
    Mix_Chunk* jumpSound;
    Mix_Chunk* coinSound;
    Mix_Chunk* powerUpSound;
    Mix_Chunk* enemyDefeatedSound;
    Mix_Chunk* damageSound;
    // BGM
    Mix_Music* backgroundMusic;
#endif
    // サウンドが有効かどうか
    bool soundEnabled;
    
    // === マップシステム（タイルベースのステージ） ===
    // マップの幅（タイル単位）- 横スクロール対応で大幅拡張
    static const int MAP_WIDTH = 100;  // 25 → 100 に拡張
    // マップの高さ（タイル単位）
    static const int MAP_HEIGHT = 19;
    // 1タイルのピクセルサイズ
    static const int TILE_SIZE = 32;
    
    // === カメラシステム ===
    float cameraX;                      // カメラのX座標
    float cameraY;                      // カメラのY座標
    static const int SCREEN_WIDTH = 800;   // 画面幅
    static const int SCREEN_HEIGHT = 600;  // 画面高さ
    float cameraFollowSpeed;            // カメラ追従速度
    int cameraDeadZone;                 // カメラのデッドゾーン（ピクセル）
    
    // === ゲーム状態管理システム ===
    enum GameState {
        STATE_TITLE,        // タイトル画面
        STATE_PLAYING,      // ゲームプレイ中
        STATE_PAUSED,       // ポーズ画面
        STATE_GAME_OVER,    // ゲームオーバー画面
        STATE_CREDITS       // クレジット画面
    };
    
    GameState currentGameState;     // 現在のゲーム状態
    int titleMenuSelection;         // タイトルメニューの選択項目
    int titleAnimationTimer;        // タイトルアニメーション用タイマー
    float titleGlowEffect;          // タイトルの光エフェクト
    bool showPressAnyKey;           // "Press Any Key"の点滅表示
    
    // マップデータ（2次元配列: 0=空、1=地面ブロック）
    int map[MAP_HEIGHT][MAP_WIDTH];
    
    // === ステージシステム ===
    // ステージデータの配列
    std::vector<StageData> stages;
    int currentStageIndex;  // 現在のステージ番号
    Goal* goal;             // ゴールオブジェクトへのポインタ
    bool stageCleared;      // ステージクリアフラグ
    bool isTransitioning;   // ステージ遷移中フラグ
    int remainingTime;      // ステージの制限時間
    bool allStagesCleared;  // 全ステージクリアフラグ
    
    // === ボス戦システム ===
    Boss* boss;                         // ボスオブジェクトへのポインタ
    std::vector<BossProjectile> bossProjectiles;  // ボスの弾丸配列
    bool isBossFight;                   // ボス戦中フラグ
    bool bossDefeated;                  // ボス撃破フラグ
    int bossStageIndex;                 // ボスステージのインデックス
    bool bossIntroComplete;             // ボス登場演出完了フラグ
    int bossIntroTimer;                 // ボス登場演出タイマー
    
    // === 敵キャラクターシステム ===
    // 敵キャラクターの配列（複数の敵を管理）
    std::vector<Enemy> enemies;
    
    // === アイテムシステム ===
    // アイテムの配列（複数のアイテムを管理）
    std::vector<Item> items;
    std::vector<EnemyProjectile> enemyProjectiles;  // 敵の弾丸リスト

    
    // === プライベートメソッド（内部処理用） ===
    // 衝突判定処理: プレイヤーと地面・プラットフォームの衝突をチェック（垂直方向）
    void CheckCollisions(int x, float& y);
    // 横方向の衝突判定: プレイヤーが横に移動する際のブロックとの衝突をチェック
    bool CheckHorizontalCollision(int x, float y);
    // マップ描画処理: タイルベースのステージを画面に描画
    void RenderMap();
    
    // === 新機能: プレイヤー・敵衝突判定システム ===
    // プレイヤーと敵の衝突判定（矩形同士の重なりをチェック）
    bool CheckPlayerEnemyCollision(const Enemy& enemy);
    // 衝突の種類を判定（踏みつけ or 横からの衝突）
    CollisionType GetCollisionType(const Enemy& enemy);
    // プレイヤー・敵衝突時の処理
    void HandlePlayerEnemyCollision(Enemy& enemy, CollisionType collisionType);
    // プレイヤーがダメージを受けた時の処理（ライフ減少、リスポーン等）
    void PlayerTakeDamage();
    // プレイヤーのリスポーン処理
    void RespawnPlayer();
    // スコア・ライフをコンソールに表示
    void DisplayGameStatus();
    
    // === アイテムシステムメソッド ===
    // アイテムシステムの初期化（アイテムの配置）
    void InitializeItems();
    // プレイヤーとアイテムの衝突判定
    bool CheckPlayerItemCollision(const Item& item);
    // アイテム取得時の処理
    void HandleItemCollection(Item& item);
    // アイテムの描画処理
    void RenderItems();
    // 特定のアイテム効果を適用
    void ApplyItemEffect(ItemType itemType);
    // プレイヤーのパワーアップ状態を更新
    void UpdatePlayerPowerState();
    
    // === ホロウナイト風システムメソッド ===
    // ダッシュシステム
    void HandleDash();                           // ダッシュ処理
    void UpdateDash();                           // ダッシュ状態の更新
    void StartDash(int direction);               // ダッシュ開始
    void EndDash();                              // ダッシュ終了
    
    // エアダッシュシステム
    void HandleAirDash();                        // エアダッシュ処理
    void StartAirDash(int direction);            // エアダッシュ開始
    void ResetAirDash();                         // エアダッシュ回数リセット
    bool CanPerformAirDash();                    // エアダッシュ可能判定
    
    // ダブルジャンプシステムメソッド
    void HandleAirJump();                // 空中ジャンプ処理
    void PerformAirJump();               // 空中ジャンプ実行
    void ResetAirJump();                 // 空中ジャンプリセット
    bool CanPerformAirJump();            // 空中ジャンプ可能チェック
    
    // 戦闘システム
    void HandleAttack();                         // 攻撃処理
    void UpdateAttack();                         // 攻撃状態の更新
    void StartAttack();                          // 攻撃開始
    void EndAttack();                            // 攻撃終了
    bool CheckAttackHit(const Enemy& enemy);     // 攻撃ヒット判定
    
    // 光線攻撃システム
    void HandleBeamAttack();                     // 光線攻撃処理
    void UpdateBeamAttack();                     // 光線攻撃状態の更新
    void StartBeamCharge();                      // 光線チャージ開始
    void FireBeam();                             // 光線発射
    void EndBeamAttack();                        // 光線攻撃終了
    bool CheckBeamHit(const Enemy& enemy);       // 光線ヒット判定
    void RenderBeam();                           // 光線描画
    
    // ウォールジャンプシステム
    void HandleWallJump();                       // ウォールジャンプ処理
    bool CheckWallCollision();                   // 壁との衝突判定
    void UpdateWallTouch();                      // 壁接触状態の更新
    
    // 壁登りシステム
    void StartWallClimb(int direction);          // 壁登り開始
    void UpdateWallClimb();                      // 壁登り更新
    void EndWallClimb();                         // 壁登り終了
    bool CanStartWallClimb();                    // 壁登り開始可能か判定
    void HandleWallClimbInput();                 // 壁登り中の入力処理
    
    // ソウルシステム
    void UpdateSoulSystem();                     // ソウルシステムの更新
    void CollectSoul(int amount);                // 魂を獲得
    void UseHeal();                              // 回復を使用
    void UseSoulBlast();                         // ソウル攻撃を使用
    
    // プレイヤー状態管理
    void UpdatePlayerMovement();                 // プレイヤー移動の統合更新
    void UpdatePlayerDirection();                // プレイヤーの向きを更新
    void ResetPlayerState();                     // プレイヤー状態をリセット
    void SafeMovePlayerX(int deltaX);             // 安全な水平移動（衝突判定付き）
    
    // === エフェクトシステムメソッド ===
    // パーティクルシステム
    void UpdateParticles();                      // パーティクルの更新
    void RenderParticles();                      // パーティクルの描画
    void SpawnParticle(float x, float y, float velX, float velY, ParticleType type, float life = 60.0f);
    void SpawnParticleBurst(float x, float y, ParticleType type, int count = 10); // パーティクル大量生成
    void CreateDashTrail();                      // ダッシュ軌跡生成
    void CreateAttackEffect();                   // 攻撃エフェクト生成
    void CreateEnemyDeathEffect(float x, float y); // 敵死亡エフェクト
    void CreateSoulCollectEffect(float x, float y); // 魂収集エフェクト
    
    // 画面シェイクシステム
    void UpdateScreenShake();                    // 画面シェイクの更新
    void StartScreenShake(int intensity, int duration); // 画面シェイク開始
    void ApplyScreenShake();                     // 画面シェイクの適用
    
    // === ビジュアルシステムメソッド ===
    // 背景描画
    void RenderGradientBackground();             // グラデーション背景の描画
    void UpdateVisualEffects();                 // ビジュアルエフェクトの更新
    
    // プレイヤー描画強化
    void RenderEnhancedPlayer();                 // 美化されたプレイヤー描画
    void RenderPlayerGlow();                     // プレイヤーの光エフェクト
    
    // 環境描画強化
    void RenderEnhancedTiles();                  // 美化されたタイル描画
    void RenderTileShadows();                    // タイルの影描画
    void RenderTile(int x, int y, bool hasTop, bool hasBottom, bool hasLeft, bool hasRight);
    
    // UI描画強化
    void RenderEnhancedUI();                     // 美化されたUI描画
    void RenderStylizedHealthBar();              // スタイル化されたHPバー
    void RenderStylizedSoulMeter();              // スタイル化された魂ゲージ
    
    // ユーティリティ
    void DrawGradientRect(SDL_Rect rect, SDL_Color topColor, SDL_Color bottomColor);
    void DrawGlowEffect(int x, int y, int radius, SDL_Color color, float intensity);
    void SetRenderColorWithAlpha(SDL_Color color, float alpha = 1.0f);
    
    // === ステージシステムメソッド ===
    // ステージシステムの初期化
    void InitializeStages();
    // 現在のステージを読み込み
    void LoadStage(int stageIndex);
    // 次のステージに進む
    void NextStage();
    // ステージをリセット（再スタート）
    void ResetCurrentStage();
    // プレイヤーとゴールの衝突判定
    bool CheckPlayerGoalCollision();
    // ステージクリア処理
    void HandleStageClear();
    // ゴールの描画処理
    void RenderGoal();
    // ステージデータの作成（個別ステージ）
    StageData CreateStage1();
    StageData CreateStage1Long();           // 横スクロール対応の長いステージ1
    StageData CreateStage2();
    StageData CreateStage3();
    StageData CreateBossStage();                 // ボスステージの作成
    // 制限時間の更新
    void UpdateTimeLimit();
    // ステージクリア条件のチェック
    bool CheckStageCleared();
    
    // === ボス戦システムメソッド ===
    void InitializeBoss();                       // ボス初期化
    void UpdateBoss();                           // ボス更新
    void RenderBoss();                           // ボス描画
    void StartBossFight();                       // ボス戦開始
    void EndBossFight();                         // ボス戦終了
    bool CheckPlayerBossCollision();             // プレイヤーとボスの衝突判定
    bool CheckAttackBossHit();                   // 攻撃がボスに当たったかチェック
    void UpdateBossProjectiles();                // ボス弾丸の更新
    void RenderBossProjectiles();                // ボス弾丸の描画
    bool CheckPlayerProjectileCollision(const BossProjectile& projectile);  // プレイヤーと弾丸の衝突
    void SpawnBossProjectile(float x, float y, float velX, float velY);      // ボス弾丸生成
    
    // === サウンドシステムメソッド（条件付きコンパイル） ===
#ifdef SOUND_ENABLED
    // サウンドシステムの初期化
    bool InitializeSound();
    // サウンドの読み込み
    bool LoadSounds();
    // サウンドリソースの解放
    void CleanupSound();
    // 効果音の再生
    void PlaySound(Mix_Chunk* sound);
    // BGMの再生
    void PlayMusic(Mix_Music* music, int loops = -1);
    // BGMの停止
    void StopMusic();
    // ボリューム設定
    void SetSoundVolume(int volume);  // 0-128
    void SetMusicVolume(int volume);  // 0-128
    // サウンドファイルが存在するかチェック
    bool CheckSoundFile(const std::string& filename);
#else
    // サウンド無効時のダミーメソッド
    bool InitializeSound() { return true; }
    void CleanupSound() {}
    void PlaySound(void* sound) {}
#endif
    
    // === 新機能: UIシステム ===
    // UIシステムの初期化（フォント読み込み等）
    bool InitializeUI();
    // UI描画処理（スコア、ライフ、タイマーを画面に表示）
    void RenderUI();
    // テキストを画面に描画するヘルパー関数
    void RenderText(const std::string& text, int x, int y, SDL_Color color);
    // ライフをハートアイコンで描画
    void RenderLives(int x, int y);
    // 時間をMM:SS形式で描画
    void RenderTime(int x, int y);
    // UIリソースの解放
    void CleanupUI();

    // === ゲームコントローラーシステム ===
    SDL_GameController* gameController;  // ゲームコントローラー
    bool controllerConnected;            // コントローラー接続状態
    
    // ボタン状態管理（入力遅延防止）
    bool prevControllerButtons[SDL_CONTROLLER_BUTTON_MAX];  // 前フレームのボタン状態
    
    // コントローラー関連メソッド
    void InitializeController();         // コントローラー初期化
    void HandleControllerInput();        // コントローラー入力処理
    void CleanupController();            // コントローラー終了処理
    bool GetControllerButton(SDL_GameControllerButton button);  // ボタン状態取得
    bool GetControllerButtonPressed(SDL_GameControllerButton button);  // ボタン新規押下判定
    float GetControllerAxis(SDL_GameControllerAxis axis);       // スティック軸取得
    void UpdateControllerButtonStates(); // ボタン状態更新
    
    // === ゲーム状態管理メソッド ===
    void HandleTitleInput();             // タイトル画面の入力処理
    void UpdateTitle();                  // タイトル画面の更新処理
    void RenderTitle();                  // タイトル画面の描画処理
    void ChangeGameState(GameState newState);  // ゲーム状態変更
    void StartNewGame();                 // 新しいゲーム開始
    void ShowCredits();                  // クレジット表示
    void UpdateGameplay();               // ゲームプレイの更新処理
    void RenderGameplay();               // ゲームプレイの描画処理
    
    // === カメラシステムメソッド ===
    void UpdateCamera();                 // カメラの更新処理
    void ClampCameraToWorld();           // カメラをワールド範囲内に制限
    int WorldToScreenX(int worldX);      // ワールド座標をスクリーン座標に変換（X軸）
    int WorldToScreenY(int worldY);      // ワールド座標をスクリーン座標に変換（Y軸）

    // === 敵システムメソッド ===
    // 敵システムの初期化
    void InitializeEnemies();
    // 敵の更新処理
    void UpdateEnemies();
    // 敵の弾丸更新処理
    void UpdateEnemyProjectiles();
    // 敵の弾丸描画処理
    void RenderEnemyProjectiles();
    // 敵の弾丸生成
    void SpawnEnemyProjectile(float x, float y, float velX, float velY, int damage = 1);
    // 敵とプレイヤーの衝突判定
    bool CheckEnemyCollision(const Enemy& enemy);
    // 敵の弾丸とプレイヤーの衝突判定
    void CheckEnemyProjectileCollisions();
}; // クラス定義の終了