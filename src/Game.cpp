// Gameクラスのヘッダファイルをインクルード（クラス定義を読み込み）
#include "Game.h"
// C++標準ライブラリ: コンソール出力（std::cout, std::endl）用
#include <iostream>
// C++標準ライブラリ: 数学関数（sin, cosなど）用
#include <cmath>
// C++標準ライブラリ: アルゴリズム（std::remove_ifを使用するため）
#include <algorithm>



// 静的メンバ変数の初期化: すべてのGameインスタンスで共有されるレンダラー
SDL_Renderer* Game::renderer = nullptr;

// === Gameクラスの敵システム管理メソッド ===


// === Gameクラスの敵システム管理メソッド ===

// 敵システムの更新
void Game::UpdateEnemies() {
    for (auto& enemy : enemies) {
        if (enemy.active) {
            enemy.Update(playerX, playerY, map);
            
            // 射撃敵の弾丸生成チェック
            if (enemy.type == ENEMY_SHOOTER && enemy.state == ENEMY_ATTACK && enemy.attackCooldown == 119) {
                float dx = playerX - enemy.x;
                float dy = playerY - enemy.y;
                float distance = sqrt(dx * dx + dy * dy);
                if (distance > 0) {
                    float speed = 4.0f;
                    float velX = (dx / distance) * speed;
                    float velY = (dy / distance) * speed;
                    
                    SpawnEnemyProjectile(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, velX, velY, 1);
                    
                    // 射撃エフェクト
                    SpawnParticleBurst(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, PARTICLE_SPARK, 3);
                    StartScreenShake(2, 5);
                }
            }
        }
    }
}

// 敵の弾丸更新処理
void Game::UpdateEnemyProjectiles() {
    for (auto it = enemyProjectiles.begin(); it != enemyProjectiles.end();) {
        EnemyProjectile& projectile = *it;
        
        if (!projectile.active) {
            it = enemyProjectiles.erase(it);
            continue;
        }
        
        projectile.Update();
        
        // プレイヤーとの衝突判定
        if (projectile.CheckCollisionWithPlayer(playerRect)) {
            // プレイヤーにダメージ
            if (invincibilityTime <= 0) {
                PlayerTakeDamage();
            }
            
            // パーティクル効果
            SpawnParticleBurst(projectile.x, projectile.y, PARTICLE_EXPLOSION, 5);
            
            it = enemyProjectiles.erase(it);
            continue;
        }
        
        // 壁との衝突判定
        int tileX = (int)(projectile.x / TILE_SIZE);
        int tileY = (int)(projectile.y / TILE_SIZE);
        if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
            if (map[tileY][tileX] == 1) {
                // 壁にヒット
                SpawnParticleBurst(projectile.x, projectile.y, PARTICLE_SPARK, 3);
                it = enemyProjectiles.erase(it);
                continue;
            }
        }
        
        ++it;
    }
}

// 敵の弾丸生成
void Game::SpawnEnemyProjectile(float x, float y, float velX, float velY, int damage) {
    enemyProjectiles.emplace_back(x, y, velX, velY, damage);
}

// 敵の弾丸とプレイヤーの衝突判定
void Game::CheckEnemyProjectileCollisions() {
    // UpdateEnemyProjectilesで処理済み
}

// 敵の弾丸描画処理
void Game::RenderEnemyProjectiles() {
    for (const auto& projectile : enemyProjectiles) {
        if (!projectile.active) continue;
        
        // カメラオフセットを適用
        int screenX = WorldToScreenX((int)projectile.x);
        int screenY = WorldToScreenY((int)projectile.y);
        
        // 画面外カリング
        if (screenX < -20 || screenX > SCREEN_WIDTH + 20 || 
            screenY < -20 || screenY > SCREEN_HEIGHT + 20) {
            continue;
        }
        
        // 敵の弾丸（赤い光）
        SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
        SDL_Rect projectileRect = {screenX, screenY, 6, 6};
        SDL_RenderFillRect(renderer, &projectileRect);
        
        // 弾丸の光エフェクト
        SDL_SetRenderDrawColor(renderer, 255, 150, 150, 128);
        SDL_Rect glowRect = {screenX - 2, screenY - 2, 10, 10};
        SDL_RenderDrawRect(renderer, &glowRect);
        
        // 弾丸の軌跡
        SDL_SetRenderDrawColor(renderer, 255, 200, 200, 64);
        SDL_Rect trailRect = {screenX - 1, screenY - 1, 8, 8};
        SDL_RenderDrawRect(renderer, &trailRect);
    }
}



// コンストラクタ: Gameオブジェクト作成時に呼ばれる初期化処理
// メンバ初期化リストを使用して各メンバ変数を初期値で設定
Game::Game() : isRunning(false), window(nullptr), 
               playerX(100), playerY(300), playerSpeed(5),
                               playerPowerLevel(0), basePlayerSpeed(5),
               playerVelY(0), gravity(0.8f), isOnGround(false), isJumping(false),
               score(0), lives(3), initialPlayerX(100), initialPlayerY(300), invincibilityTime(0),
               gameTime(0), frameCounter(0), font(nullptr), uiBackgroundAlpha(180),
               currentStageIndex(0), goal(nullptr), stageCleared(false), isTransitioning(false),
               remainingTime(0), allStagesCleared(false),
               // ホロウナイト風システムの初期化
               canDash(true), dashCooldown(0), dashSpeed(12.0f), dashDuration(15), dashTimer(0),
               isDashing(false), lastDirection(1), isAttacking(false), attackCooldown(0),
               attackDuration(20), attackTimer(0), soulCount(0), maxSoul(99),
               playerHealth(3), maxHealth(3), playerVelX(0.0f), touchingWall(false), canWallJump(false), wallJumpCooldown(0),
               // 壁登りシステムの初期化
               isWallClimbing(false), wallClimbStamina(100), maxWallClimbStamina(100), wallClimbSpeed(-3.0f),
               canWallClimb(false), wallClimbDirection(0), wallClimbTimer(0), wallSlideSpeed(2),
               // エアダッシュシステムの初期化
               airDashCount(0), maxAirDash(2), canAirDash(true), airDashSpeed(10.0f), lastAirDashDirection(1),
               // ダブルジャンプシステムの初期化
               airJumpCount(0), maxAirJump(1), canAirJump(true),
               // 光線攻撃システムの初期化
               isChargingBeam(false), beamChargeTime(0), maxBeamChargeTime(60),
               isFiringBeam(false), beamDuration(30), beamTimer(0), beamDamage(3.0f), beamCost(5),
               // ダッシュMP消費設定
               dashCost(2),
               // ボス戦システムの初期化
               boss(nullptr), isBossFight(false), bossDefeated(false), bossStageIndex(-1),
               bossIntroComplete(false), bossIntroTimer(0),
               // エフェクトシステムの初期化
               particleLimit(500), screenShakeIntensity(0), screenShakeDuration(0),
               shakeOffsetX(0.0f), shakeOffsetY(0.0f),
               // ビジュアルシステムの初期化
               enableGradientBackground(true), gradientOffset(0.0f),
               playerGlowIntensity(0.8f), playerGlowTimer(0.0f),
               ambientDarkness(0.2f), enableShadows(true),
#ifdef SOUND_ENABLED
               jumpSound(nullptr), coinSound(nullptr), powerUpSound(nullptr), 
               enemyDefeatedSound(nullptr), damageSound(nullptr), backgroundMusic(nullptr), 
#endif
               soundEnabled(true),
                               // ゲームコントローラーシステムの初期化
                gameController(nullptr), controllerConnected(false),
                // ゲーム状態管理システムの初期化
                currentGameState(STATE_TITLE), titleMenuSelection(0), titleAnimationTimer(0),
                titleGlowEffect(0.0f), showPressAnyKey(true),
                // カメラシステムの初期化
                cameraX(0.0f), cameraY(0.0f), cameraFollowSpeed(0.1f), cameraDeadZone(100) {
    
    // コントローラーボタン状態の初期化
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        prevControllerButtons[i] = false;
    }
    
    // === マップの初期化（マリオ風ステージ作成） ===
    // 全体を空（0）で初期化
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = 0;  // 空
        }
    }
    
    // 最下段を地面にする（マリオの基本地面）
    for (int x = 0; x < MAP_WIDTH; x++) {
        map[MAP_HEIGHT - 1][x] = 1;  // 地面
        map[MAP_HEIGHT - 2][x] = 1;  // 地面（厚み）
    }
    
    // プラットフォームを追加（マリオ風の浮島）
    // 左側の小さなプラットフォーム
    map[14][5] = 1;
    map[14][6] = 1;
    map[14][7] = 1;
    
    // 中央の中くらいのプラットフォーム
    map[12][10] = 1;
    map[12][11] = 1;
    map[12][12] = 1;
    map[12][13] = 1;
    
    // 右側の高いプラットフォーム
    map[10][18] = 1;
    map[10][19] = 1;
    map[10][20] = 1;
    
    // 階段状のプラットフォーム
    map[16][15] = 1;
    map[15][16] = 1;
    map[14][17] = 1;
    
    // === 敵キャラクターの初期配置 ===
    // 多様な敵タイプで初期配置
    Enemy goomba1(400, (MAP_HEIGHT - 3) * TILE_SIZE, ENEMY_GOOMBA);
    enemies.push_back(goomba1);
    
    Enemy shooter1(320, 11 * TILE_SIZE, ENEMY_SHOOTER);
    enemies.push_back(shooter1);
    
    // === UIエリアの初期化 ===
    // UI描画エリアを画面上部に設定（高さ50ピクセル）
    uiArea.x = 0;
    uiArea.y = 0;
    uiArea.w = 800;  // 画面幅と同じ
    uiArea.h = 50;   // UI用の高さ
    
    // 初期ゲーム状態を表示
    std::cout << "📊 初期状態 - スコア: " << score << " | ❤️ ライフ: " << lives << std::endl;
    
    // === ステージシステムの初期化 ===
    InitializeStages();
    
    // === アイテムシステムの初期化 ===
    InitializeItems();
}

// デストラクタ: Gameオブジェクト破棄時に呼ばれる終了処理
// Clean関数を呼び出してリソースの解放を確実に実行
Game::~Game() {
    Clean();
}

// ゲーム初期化関数: SDL2の初期化、ウィンドウ・レンダラーの作成を行う
// title: ウィンドウタイトル, x,y: ウィンドウ位置, width,height: ウィンドウサイズ, fullscreen: フルスクリーンフラグ
// 戻り値: 初期化成功時true、失敗時false
bool Game::Initialize(const char* title, int x, int y, int width, int height, bool fullscreen) {
    // ウィンドウフラグの初期化（通常ウィンドウの場合は0）
    int flags = 0;
    // フルスクリーンが指定された場合はフラグを設定
    if (fullscreen) {
        flags = SDL_WINDOW_FULLSCREEN;
    }
    
    // SDL2ライブラリ全体を初期化（ビデオ、オーディオ、イベント、タイマーなど）
    if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
        // 初期化成功時の処理
        std::cout << "SDL初期化成功" << std::endl;
        
        // ゲームウィンドウを作成
        // title: タイトルバーに表示される文字列
        // x, y: ウィンドウの画面上での位置
        // width, height: ウィンドウのサイズ（ピクセル）
        // flags: ウィンドウの表示モード（フルスクリーンなど）
        window = SDL_CreateWindow(title, x, y, width, height, flags);
        if (window) {
            std::cout << "ウィンドウ作成成功" << std::endl;
        }
        
        // レンダラーを作成（実際の描画処理を担当）
        // window: 描画対象のウィンドウ
        // -1: 使用するグラフィックドライバー（-1で自動選択）
        // SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC: ハードウェア加速 + VSync有効
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (renderer) {
            // レンダラーのデフォルト描画色を白色（R=255, G=255, B=255, A=255）に設定
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "レンダラー作成成功" << std::endl;
        }
        
        // 初期化が完了したのでゲーム実行フラグをtrueに設定
        isRunning = true;
        
        // UIシステムを初期化
        if (!InitializeUI()) {
            std::cout << "UI初期化に失敗しました" << std::endl;
            isRunning = false;
        }
        
        // ゲームコントローラーを初期化
        InitializeController();
        
        // サウンドシステムを初期化
#ifdef SOUND_ENABLED
        if (!InitializeSound()) {
            std::cout << "サウンド初期化に失敗しました（ゲームは続行されます）" << std::endl;
            soundEnabled = false;
        }
#else
        std::cout << "サウンドシステムは無効です（SDL_mixerが見つかりません）" << std::endl;
        soundEnabled = false;
#endif
    } else {
        // SDL初期化失敗時の処理
        isRunning = false;
    }
    
    // プレイヤーキャラクターの描画用矩形を初期化
    playerRect.x = playerX;        // X座標（横位置）
    playerRect.y = playerY;        // Y座標（縦位置）
    playerRect.w = 30;             // 幅（ピクセル）- マリオサイズに調整
    playerRect.h = 30;             // 高さ（ピクセル）- マリオサイズに調整
    
    // 初期化結果を返す（成功時true、失敗時false）
    return isRunning;
}

// イベント処理関数: キーボード入力、マウス操作、ウィンドウイベントを処理
void Game::HandleEvents() {
    // SDL_Event構造体: キーボード、マウス、ウィンドウイベントの情報を格納
    SDL_Event event;
    
    // イベントキューからすべてのイベントを処理（遅延防止）
    while (SDL_PollEvent(&event)) {
        // イベントの種類に応じて処理を分岐
        switch (event.type) {
            case SDL_QUIT:  // ウィンドウの×ボタンが押された場合
                isRunning = false;  // ゲームループを終了
                break;
            case SDL_CONTROLLERDEVICEADDED:  // コントローラー接続
                std::cout << "🎮 コントローラーが接続されました" << std::endl;
                if (!controllerConnected) {
                    InitializeController();
                }
                break;
            case SDL_CONTROLLERDEVICEREMOVED:  // コントローラー切断
                std::cout << "🎮 コントローラーが切断されました" << std::endl;
                CleanupController();
                break;
            default:  // その他のイベントは無視
                break;
        }
    }
    
    // ゲーム状態に応じた入力処理
    switch (currentGameState) {
        case STATE_TITLE:
            HandleTitleInput();
            break;
        case STATE_PLAYING:
            // 入力処理（コントローラー優先）
            if (controllerConnected && gameController) {
                // ゲームコントローラー入力処理のみ
                HandleControllerInput();
            } else {
                // リアルタイムキーボード入力処理（コントローラー未接続時のみ）
                const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
                
                // === 水平移動（マリオ風の左右移動）+ 横方向衝突判定 ===
                // 左移動: 左矢印キー または Aキー が押されている場合
                if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
                    int newPlayerX = playerX - playerSpeed;
                    // 左方向の衝突判定
                    if (!CheckHorizontalCollision(newPlayerX, playerY)) {
                        playerX = newPlayerX;  // 衝突しない場合のみ移動
                    }
                }
                // 右移動: 右矢印キー または Dキー が押されている場合
                if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
                    int newPlayerX = playerX + playerSpeed;
                    // 右方向の衝突判定
                    if (!CheckHorizontalCollision(newPlayerX, playerY)) {
                        playerX = newPlayerX;  // 衝突しない場合のみ移動
                    }
                }
                
                // === ジャンプ（通常ジャンプ + ダブルジャンプ + ウォールジャンプ） ===
                // ジャンプ: スペースキー または Wキー が押された場合
                if (currentKeyStates[SDL_SCANCODE_SPACE] || currentKeyStates[SDL_SCANCODE_W]) {
                    if (isOnGround) {
                        // 通常ジャンプ（地面から）
                        playerVelY = -15.0f;  // 上向きの初速度（負の値が上方向）
                        isOnGround = false;   // 地面から離れる
                        isJumping = true;     // ジャンプ状態に設定
                        
#ifdef SOUND_ENABLED
                        // ジャンプ音を再生
                        PlaySound(jumpSound);
#endif
                    } else if (touchingWall && !isOnGround && canWallJump) {
                        // ウォールジャンプ（最優先）
                        HandleWallJump();
                    } else if (CanPerformAirJump()) {
                        // 空中ジャンプ（ダブルジャンプ）
                        HandleAirJump();
                    }
                }
                
                // === ホロウナイト風入力処理 ===
                
                // 攻撃: Zキー
                if (currentKeyStates[SDL_SCANCODE_Z] && !isAttacking && attackCooldown <= 0) {
                    HandleAttack();
                }
                
                // 回復: Cキー（ソウルを使用）
                if (currentKeyStates[SDL_SCANCODE_C] && soulCount >= 33 && playerHealth < maxHealth) {
                    UseHeal();
                }
                
                // 壁登り中の入力処理
                if (isWallClimbing) {
                    HandleWallClimbInput();
                }
                
                // === 画面境界チェック（X軸のみ、Y軸は物理システムで管理） ===
                if (playerX < 0) playerX = 0;  // 左端を超えた場合、左端に固定
                if (playerX > (MAP_WIDTH * TILE_SIZE) - playerRect.w) {
                    playerX = (MAP_WIDTH * TILE_SIZE) - playerRect.w;  // 右端制限
                }
                
                // === ステージ操作キー ===
                // Nキー: 次のステージ（ステージクリア後のみ）
                if (currentKeyStates[SDL_SCANCODE_N] && stageCleared && !isTransitioning) {
                    if (!allStagesCleared) {
                        NextStage();
                        isTransitioning = true;  // 連続入力防止
                    }
                }
                
                // Rキー: 現在のステージをリスタート
                if (currentKeyStates[SDL_SCANCODE_R] && !isTransitioning) {
                    ResetCurrentStage();
                    isTransitioning = true;  // 連続入力防止
                }
                
                // キーが離されたら遷移フラグをリセット
                if (!currentKeyStates[SDL_SCANCODE_N] && !currentKeyStates[SDL_SCANCODE_R]) {
                    isTransitioning = false;
                }
            }
            
            // プレイヤーの向きを更新（コントローラー・キーボード共通）
            UpdatePlayerDirection();
            break;
        case STATE_PAUSED:
        case STATE_GAME_OVER:
        case STATE_CREDITS:
            // 各状態の入力処理は後で実装
            break;
    }
    
    // コントローラーボタン状態を更新（次フレーム用）
    UpdateControllerButtonStates();
}

// ゲーム状態更新関数: 1フレームごとのゲームロジック処理
void Game::Update() {
    // === 共通タイマーの更新 ===
    frameCounter++;
    if (frameCounter >= 60) {  // 60フレーム = 1秒
        gameTime++;
        frameCounter = 0;
    }
    
    // ゲーム状態に応じた更新処理
    switch (currentGameState) {
        case STATE_TITLE:
            UpdateTitle();
            break;
        case STATE_PLAYING:
            UpdateGameplay();
            break;
        case STATE_PAUSED:
        case STATE_GAME_OVER:
        case STATE_CREDITS:
            // 各状態の更新処理は後で実装
            break;
    }
}

// ゲームプレイ中の更新処理
void Game::UpdateGameplay() {
    
    // === プレイヤーの物理計算（重力とジャンプ） ===
    // 地面に接触していない場合は重力を適用（壁登り中は除く）
    if (!isOnGround && !isWallClimbing) {
        playerVelY += gravity;  // 重力による下向きの加速
    }
    
    // Y位置の仮計算（衝突判定前）
    float newPlayerY = playerY + playerVelY;
    
    // 衝突判定を実行（地面・プラットフォームとの接触チェック）
    CheckCollisions(playerX, newPlayerY);
    
    // プレイヤー位置を更新（衝突判定で調整された位置）
    playerY = newPlayerY;
    
    // 描画用矩形を更新
    playerRect.x = playerX;
    playerRect.y = (int)playerY;  // floatからintにキャスト
    
    // 地面着地時のスタミナ回復
    if (isOnGround && wallClimbStamina < maxWallClimbStamina) {
        wallClimbStamina += 2;  // 毎フレーム2ずつ回復
        if (wallClimbStamina > maxWallClimbStamina) {
            wallClimbStamina = maxWallClimbStamina;
        }
    }
    
    // === ホロウナイト風システムの更新 ===
    // ダッシュシステムの更新
    UpdateDash();
    
    // 攻撃システムの更新
    UpdateAttack();
    
    // 光線攻撃システムの更新
    UpdateBeamAttack();
    
    // 光線攻撃の入力処理（毎フレーム実行）
    HandleBeamAttack();
    
    // ダッシュの入力処理（毎フレーム実行）
    HandleDash();
    
    // デバッグ: MP獲得量を確認
    static int lastSoulCount = 0;
    if (soulCount != lastSoulCount) {
        std::cout << "🔍 MP変化: " << lastSoulCount << " → " << soulCount << " (差分: " << (soulCount - lastSoulCount) << ")" << std::endl;
        lastSoulCount = soulCount;
    }
    
    // 壁接触の更新
    UpdateWallTouch();
    
    // 壁登りシステムの更新
    UpdateWallClimb();
    
    // ソウルシステムの更新
    UpdateSoulSystem();
    
    // プレイヤー移動の統合更新
    UpdatePlayerMovement();
    
    // === エフェクトシステムの更新 ===
    UpdateParticles();
    UpdateScreenShake();
    CreateDashTrail();  // ダッシュ軌跡の生成
    CreateAttackEffect(); // 攻撃エフェクトの生成
    
    // === ビジュアルシステムの更新 ===
    UpdateVisualEffects();
    
    // === ボス戦システムの更新 ===
    UpdateBoss();
    
    // === 敵キャラクターの更新 ===
    UpdateEnemies();
    
    // === 敵の弾丸更新 ===
    UpdateEnemyProjectiles();
    
    // === 無敵時間の更新 ===
    if (invincibilityTime > 0) {
        invincibilityTime--;  // 無敵時間を1フレーム減らす
    }
    
    // === プレイヤー・敵衝突判定 ===
    // 無敵時間中でない場合のみ衝突判定を実行
    if (invincibilityTime <= 0) {
        for (auto& enemy : enemies) {
            if (enemy.active && CheckPlayerEnemyCollision(enemy)) {
                // 衝突が発生した場合、衝突の種類を判定
                CollisionType collisionType = GetCollisionType(enemy);
                HandlePlayerEnemyCollision(enemy, collisionType);
                break;  // 1フレームに1つの敵との衝突のみ処理
            }
        }
    }

    // === アイテムシステムの更新 ===
    for (auto& item : items) {
        if (item.active && !item.collected) {
            item.Update();
            if (CheckPlayerItemCollision(item)) {
                HandleItemCollection(item);
            }
        }
    }

    // プレイヤーのパワーアップ状態を更新
    UpdatePlayerPowerState();
    
    // === ステージシステムの更新 ===
    // ゴールの更新
    if (goal && goal->active) {
        goal->Update();
    }
    
    // 制限時間の更新
    UpdateTimeLimit();
    
    // ステージクリア条件のチェック
    if (!stageCleared && CheckStageCleared()) {
        HandleStageClear();
    }
    
    // プレイヤーとゴールの衝突判定
    if (!stageCleared && goal && goal->active && CheckPlayerGoalCollision()) {
        HandleStageClear();
    }
    
    // === カメラシステムの更新 ===
    UpdateCamera();
}

// 衝突判定処理: プレイヤーと地面・プラットフォームの衝突をチェック（全方向対応）
void Game::CheckCollisions(int x, float& y) {
    // === 上向きの衝突判定（ジャンプ時に頭がブロックにぶつかる） ===
    if (playerVelY < 0) {  // 上向きに移動している場合
        // プレイヤーの頭の部分のタイル座標を計算
        int headTileX = (x + playerRect.w / 2) / TILE_SIZE;
        int headTileY = (int)y / TILE_SIZE;
        
        // マップ範囲内かチェック
        if (headTileX >= 0 && headTileX < MAP_WIDTH && headTileY >= 0 && headTileY < MAP_HEIGHT) {
            // ブロックに衝突した場合
            if (map[headTileY][headTileX] == 1) {
                // プレイヤーの頭をブロックの下に配置
                y = (headTileY + 1) * TILE_SIZE;
                playerVelY = 0;  // 上向きの速度を停止（マリオ風の頭ぶつけ）
                std::cout << "💥 ブロックに頭をぶつけた！" << std::endl;
                return;
            }
        }
    }
    
    // === 下向きの衝突判定（落下時に地面やプラットフォームに着地） ===
    if (playerVelY >= 0) {  // 下向きに移動している、または静止している場合
        // プレイヤーの足元のタイル座標を計算
        int footTileX = (x + playerRect.w / 2) / TILE_SIZE;
        int footTileY = ((int)y + playerRect.h) / TILE_SIZE;
        
        // マップ範囲内かチェック
        if (footTileX >= 0 && footTileX < MAP_WIDTH && footTileY >= 0 && footTileY < MAP_HEIGHT) {
            // 地面タイル（1）に衝突した場合
            if (map[footTileY][footTileX] == 1) {
                // プレイヤーを地面の上に正確に配置
                y = footTileY * TILE_SIZE - playerRect.h;
                playerVelY = 0;       // 落下速度をリセット
                isOnGround = true;    // 地面接触フラグをON
                isJumping = false;    // ジャンプ終了
                
                // エアダッシュ回数をリセット
                ResetAirDash();
                // ダブルジャンプ回数をリセット
                ResetAirJump();
                return;
            }
        }
    }
    
    // 地面に接触していない（空中にいる）
    isOnGround = false;
    
    // 画面下端から落下した場合の処理（ゲームオーバーやリスポーン）
    if (y > MAP_HEIGHT * TILE_SIZE) {
        // 画面上部にリスポーン
        playerX = 100;
        y = 100;
        playerVelY = 0;
    }
}

// 描画処理関数: ゲーム状態に応じた描画
void Game::Render() {
    // 画面をクリア
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 黒でクリア
    SDL_RenderClear(renderer);
    
    // ゲーム状態に応じた描画処理
    switch (currentGameState) {
        case STATE_TITLE:
            RenderTitle();
            break;
        case STATE_PLAYING:
            RenderGameplay();
            break;
        case STATE_PAUSED:
        case STATE_GAME_OVER:
        case STATE_CREDITS:
            // 各状態の描画処理は後で実装
            break;
    }
    
    // 画面に描画内容を表示（ダブルバッファリング）
    SDL_RenderPresent(renderer);
}

// マップ描画処理: タイルベースのステージを画面に描画
void Game::RenderMap() {
    SDL_Rect tileRect;
    tileRect.w = TILE_SIZE;  // タイルの幅
    tileRect.h = TILE_SIZE;  // タイルの高さ
    
    // マップ全体をスキャンして地面タイルを描画
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (map[y][x] == 1) {  // 地面タイルの場合
                // タイルの描画位置を計算
                tileRect.x = x * TILE_SIZE;
                tileRect.y = y * TILE_SIZE;
                
                // 茶色の地面ブロック（マリオ風）を描画
                SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255);  // 茶色
                SDL_RenderFillRect(renderer, &tileRect);
                
                // ブロックの境界線を黒で描画（見やすさのため）
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // 黒色
                SDL_RenderDrawRect(renderer, &tileRect);
            }
        }
    }
}

// 終了処理関数: SDL2関連のリソースを解放してメモリリークを防ぐ
void Game::Clean() {
    // ゴールオブジェクトを解放
    if (goal) {
        delete goal;
        goal = nullptr;
    }
    
    // サウンドリソースを解放
    CleanupSound();
    
    // UIリソースを解放
    CleanupUI();
    
    // ゲームコントローラーを解放
    CleanupController();
    
    // レンダラーが作成されている場合は破棄
    if (renderer) {
        SDL_DestroyRenderer(renderer);  // レンダラーのメモリを解放
        renderer = nullptr;             // ポインタを無効化（ダングリングポインタ防止）
    }
    // ウィンドウが作成されている場合は破棄
    if (window) {
        SDL_DestroyWindow(window);      // ウィンドウのメモリを解放
        window = nullptr;               // ポインタを無効化
    }
    // SDL2ライブラリ全体を終了（すべてのSDL2リソースを解放）
    SDL_Quit();
    // 終了メッセージをコンソールに出力
    std::cout << "マリオ風ゲーム終了" << std::endl;
    
    // ゴールオブジェクトの削除
    delete goal;
    
    // ボスオブジェクトの削除
    delete boss;
    
    // === サウンドシステムのクリーンアップ ===
}

// === 新機能: プレイヤー・敵衝突判定システムの実装 ===

// プレイヤーと敵の衝突判定（矩形同士の重なりをチェック）
bool Game::CheckPlayerEnemyCollision(const Enemy& enemy) {
    // SDL_HasIntersectionを使用して矩形の重なりを判定
    return SDL_HasIntersection(&playerRect, &enemy.rect);
}

// 衝突の種類を判定（ダッシュアタック > 踏みつけ > 横からの衝突）
CollisionType Game::GetCollisionType(const Enemy& enemy) {
    // プレイヤーの中心座標を計算
    int playerCenterX = playerRect.x + playerRect.w / 2;
    int playerCenterY = playerRect.y + playerRect.h / 2;
    
    // 敵の中心座標を計算
    int enemyCenterX = enemy.rect.x + enemy.rect.w / 2;
    int enemyCenterY = enemy.rect.y + enemy.rect.h / 2;
    
    // ダッシュ中（地上ダッシュまたはエアダッシュ）の場合は最優先でダッシュアタック
    if (isDashing || (airDashCount > 0 && !isOnGround)) {
        return DASH_ATTACK_ENEMY;  // ダッシュアタック（敵にダメージ）
    }
    // プレイヤーが敵よりも上にいる場合（踏みつけ）
    // かつ、プレイヤーが落下中または下向きの速度を持っている場合
    else if (playerCenterY < enemyCenterY && playerVelY > 0) {
        return STOMP_ENEMY;  // 踏みつけ攻撃
    } else {
        return DAMAGED_BY_ENEMY;  // 横からの衝突（ダメージ）
    }
}

// プレイヤー・敵衝突時の処理
void Game::HandlePlayerEnemyCollision(Enemy& enemy, CollisionType collisionType) {
    switch (collisionType) {
        case STOMP_ENEMY:
            // 敵を倒す処理
            enemy.active = false;  // 敵を無効化
            score += 100;          // スコア加算
            playerVelY = -10.0f;   // 小さなジャンプ（マリオ風の踏みつけバウンス）
            
            // 通常攻撃でMP2増加
            CollectSoul(2);
            
            // 敵死亡エフェクトを生成
            CreateEnemyDeathEffect(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2);
            
#ifdef SOUND_ENABLED
            // 敵撃破音を再生
            PlaySound(enemyDefeatedSound);
#endif
            
            // 成功メッセージとスコア表示
            std::cout << "🍄 敵を倒した！ +100点 +2MP" << std::endl;
            DisplayGameStatus();
            break;
            
        case DASH_ATTACK_ENEMY:
            // ダッシュアタックで敵を倒す処理
            enemy.active = false;  // 敵を無効化
            score += 200;          // 高スコア加算（踏みつけの2倍）
            // ダッシュは継続（バウンスしない）
            
            // ダッシュ攻撃でMP1増加
            CollectSoul(1);
            
            // ダッシュアタック専用エフェクト
            CreateEnemyDeathEffect(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2);
            SpawnParticleBurst(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, PARTICLE_EXPLOSION, 15);
            StartScreenShake(8, 15);  // 強い衝撃
            
            // プレイヤー周りにも攻撃エフェクト
            SpawnParticleBurst(playerX + playerRect.w/2, playerY + playerRect.h/2, PARTICLE_SPARK, 8);
            
            // プレイヤーの光エフェクトを一時的に強化
            playerGlowIntensity = 1.5f;  // 攻撃時の光強化
            
#ifdef SOUND_ENABLED
            // 敵撃破音を再生
            PlaySound(enemyDefeatedSound);
#endif
            
            // 成功メッセージとスコア表示
            std::cout << "💥 ダッシュアタック！ +200点 +1MP ⚡" << std::endl;
            DisplayGameStatus();
            break;
            
        case DAMAGED_BY_ENEMY:
            // プレイヤーがダメージを受ける処理
            PlayerTakeDamage();
            break;
            
        default:
            // 何もしない
            break;
    }
}

// プレイヤーがダメージを受けた時の処理（ライフ減少、リスポーン等）
void Game::PlayerTakeDamage() {
    // 無敵時間中はダメージを受けない
    if (invincibilityTime > 0) return;
    
    // ホロウナイト風のHP管理
    if (playerHealth > 1) {
        playerHealth--;
        invincibilityTime = MAX_INVINCIBILITY_TIME;  // 無敵時間を設定
        std::cout << "💔 ダメージ！ HP: " << playerHealth << "/" << maxHealth << std::endl;
        
#ifdef SOUND_ENABLED
        // ダメージ音を再生
        PlaySound(damageSound);
#endif
        return;  // HP残りがあるので続行
    }
    
    // パワーアップ状態の場合はまずパワーダウン
    if (playerPowerLevel > 0) {
        playerPowerLevel--;
        
        // プレイヤーサイズを元に戻す
        if (playerPowerLevel == 0) {
            playerRect.w = 30;  // 元のサイズ
            playerRect.h = 30;
        } else if (playerPowerLevel == 1) {
            playerRect.w = 32;  // 中間サイズ
            playerRect.h = 32;
        }
        
        invincibilityTime = MAX_INVINCIBILITY_TIME;  // 無敵時間を設定
        std::cout << "⬇️ パワーダウン！ レベル: " << playerPowerLevel << std::endl;
        
#ifdef SOUND_ENABLED
        // ダメージ音を再生
        PlaySound(damageSound);
#endif
        return;  // ライフは減らさない
    }
    
    // 最後のHP失失時にライフ減少
    lives--;  // ライフを1減らす
    invincibilityTime = MAX_INVINCIBILITY_TIME;  // 無敵時間を設定
    
#ifdef SOUND_ENABLED
    // ダメージ音を再生
    PlaySound(damageSound);
#endif
    
    std::cout << "💀 死亡！ ライフ: " << lives << std::endl;
}

// プレイヤーのリスポーン処理
void Game::RespawnPlayer() {
    // プレイヤーを初期位置に戻す
    playerX = initialPlayerX;
    playerY = initialPlayerY;
    playerVelY = 0;  // 落下速度をリセット
    isOnGround = false;
    isJumping = false;
    
    // パワーアップ状態をリセット
    playerPowerLevel = 0;
    playerSpeed = basePlayerSpeed;
    playerRect.w = 30;  // 元のサイズに戻す
    playerRect.h = 30;
    
    // ホロウナイト風システムのリセット
    ResetPlayerState();
    
    // 描画用矩形も更新
    playerRect.x = playerX;
    playerRect.y = playerY;
    
    std::cout << "🔄 プレイヤーがリスポーンしました" << std::endl;
}

// スコア・ライフをコンソールに表示
void Game::DisplayGameStatus() {
    std::cout << "📊 スコア: " << score << " | ❤️ ライフ: " << lives << std::endl;
}

// 横方向の衝突判定: プレイヤーが横に移動する際のブロックとの衝突をチェック
bool Game::CheckHorizontalCollision(int x, float y) {
    // プレイヤーの左端と右端のタイル座標を計算
    int leftTileX = x / TILE_SIZE;
    int rightTileX = (x + playerRect.w - 1) / TILE_SIZE;
    
    // プレイヤーの上端と下端のタイル座標を計算
    int topTileY = (int)y / TILE_SIZE;
    int bottomTileY = ((int)y + playerRect.h - 1) / TILE_SIZE;
    
    // プレイヤーの範囲内のすべてのタイルをチェック
    for (int tileY = topTileY; tileY <= bottomTileY; tileY++) {
        for (int tileX = leftTileX; tileX <= rightTileX; tileX++) {
            // マップ範囲内かチェック
            if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
                // ブロック（1）と衝突する場合
                if (map[tileY][tileX] == 1) {
                    return true;  // 衝突あり
                }
            }
        }
    }
    
    return false;  // 衝突なし
}

// === UIシステムの実装 ===

// UIシステムの初期化（フォント読み込み等）
bool Game::InitializeUI() {
    // SDL_ttfライブラリを初期化
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init エラー: " << TTF_GetError() << std::endl;
        return false;
    }
    
    // システムフォントを読み込み（フォントファイルがない場合のフォールバック）
    // macOS、Linux、Windowsのフォントパスを順番に試す
    const char* fontPaths[] = {
        "/System/Library/Fonts/Supplemental/Arial.ttf",     // macOS Arial
        "/System/Library/Fonts/Apple Symbols.ttf",          // macOS Apple Symbols
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf", // Linux
        "C:\\Windows\\Fonts\\arial.ttf",                    // Windows
        nullptr
    };
    
    // 利用可能なフォントを探す
    for (int i = 0; fontPaths[i] != nullptr; i++) {
        font = TTF_OpenFont(fontPaths[i], 20);  // フォントサイズ20
        if (font) {
            std::cout << "フォント読み込み成功: " << fontPaths[i] << std::endl;
            break;
        }
    }
    
    // フォントが見つからない場合の処理
    if (!font) {
        std::cout << "警告: フォントファイルが見つかりません。UIテキストが表示されません。" << std::endl;
        std::cout << "TTF_OpenFont エラー: " << TTF_GetError() << std::endl;
        // フォントがなくてもゲームは続行可能
    }
    
    return true;  // フォントがなくても成功とする
}

// UI描画処理（スコア、ライフ、タイマーを画面に表示）
void Game::RenderUI() {
    // UI背景を半透明の黒で描画
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, uiBackgroundAlpha);  // 半透明の黒
    SDL_RenderFillRect(renderer, &uiArea);
    
    // フォントが利用可能な場合のみテキストを描画
    if (font) {
        // 白色でテキストを描画
        SDL_Color white = {255, 255, 255, 255};
        
        // スコア表示（左上）
        std::string scoreText = "Score: " + std::to_string(score);
        RenderText(scoreText, 10, 15, white);
        
        // ライフ表示（中央左）
        RenderLives(250, 15);
        
        // 時間表示（右上）
        RenderTime(650, 15);
        
        // ステージ情報表示（中央上）
        if (currentStageIndex < (int)stages.size()) {
            std::string stageText = stages[currentStageIndex].stageName;
            SDL_Color cyan = {0, 255, 255, 255};
            RenderText(stageText, 350, 15, cyan);
        }
        
        // パワーレベル表示（左下）
        if (playerPowerLevel > 0) {
            std::string powerText = "Power Lv." + std::to_string(playerPowerLevel);
            SDL_Color green = {0, 255, 0, 255};
            RenderText(powerText, 10, uiArea.h + 5, green);
        }
        

        
        // 制限時間表示（残り時間がある場合）
        if (remainingTime > 0) {
            std::string timeText = "Time: " + std::to_string(remainingTime);
            SDL_Color red = {255, 100, 100, 255};
            RenderText(timeText, 550, uiArea.h + 5, red);
        }
        
        // === ホロウナイト風UI表示 ===
        // HP表示（ハート）
        SDL_Color heartColor = {255, 100, 100, 255};  // 赤色
        std::string hpText = "HP: ";
        for (int i = 0; i < maxHealth; i++) {
            if (i < playerHealth) {
                hpText += "♥ ";  // 満タンのハート
            } else {
                hpText += "♡ ";  // 空のハート
            }
        }
        RenderText(hpText, 10, uiArea.h + 25, heartColor);
        
        // 魂表示
        if (soulCount > 0) {
            std::string soulText = "Soul: " + std::to_string(soulCount) + "/" + std::to_string(maxSoul);
            SDL_Color soulColor = {150, 200, 255, 255};  // 青色
            RenderText(soulText, 200, uiArea.h + 25, soulColor);
        }
        
        // ダッシュクールダウン表示
        if (dashCooldown > 0) {
            float cooldownPercent = (float)dashCooldown / 45.0f;
            std::string dashText = "Dash: " + std::to_string((int)(cooldownPercent * 100)) + "%";
            SDL_Color dashColor = {255, 200, 100, 255};  // オレンジ色
            RenderText(dashText, 350, uiArea.h + 25, dashColor);
        } else if (canDash) {
            SDL_Color readyColor = {100, 255, 100, 255};  // 緑色
            RenderText("Dash: Ready", 350, uiArea.h + 25, readyColor);
        }
        
        // エアダッシュ残り回数表示
        if (!isOnGround) {
            int remainingAirDash = maxAirDash - airDashCount;
            std::string airDashText = "Air Dash: " + std::to_string(remainingAirDash) + "/" + std::to_string(maxAirDash);
            SDL_Color airDashColor = {100, 200, 255, 255};  // 青色
            if (remainingAirDash > 0) {
                RenderText(airDashText, 500, uiArea.h + 25, airDashColor);
            } else {
                SDL_Color emptyColor = {150, 150, 150, 255};  // グレー
                RenderText(airDashText, 500, uiArea.h + 25, emptyColor);
            }
        }
        
        // 操作説明（常時表示）
        SDL_Color helpColor = {180, 180, 180, 255};  // グレー
        RenderText("X/Shift:Dash Z:Attack C:Heal", 10, uiArea.h + 45, helpColor);
        
        // ステージクリア表示
        if (stageCleared) {
            std::string clearText = allStagesCleared ? "ALL STAGES CLEAR!" : "STAGE CLEAR!";
            SDL_Color yellow = {255, 255, 0, 255};
            RenderText(clearText, 300, 250, yellow);
            
            // 操作説明
            if (!allStagesCleared) {
                SDL_Color white = {255, 255, 255, 255};
                RenderText("Press N for Next Stage", 280, 280, white);
            }
            SDL_Color gray = {200, 200, 200, 255};
            RenderText("Press R to Restart", 310, 310, gray);
        }
    }
    
    // ブレンドモードを元に戻す
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// テキストを画面に描画するヘルパー関数
void Game::RenderText(const std::string& text, int x, int y, SDL_Color color) {
    if (!font) return;  // フォントがない場合は何もしない
    
    // テキストサーフェスを作成
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        return;  // テキスト作成失敗
    }
    
    // サーフェスからテクスチャを作成
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_FreeSurface(textSurface);
        return;  // テクスチャ作成失敗
    }
    
    // テキストの描画位置とサイズを設定
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;
    destRect.w = textSurface->w;
    destRect.h = textSurface->h;
    
    // テキストを描画
    SDL_RenderCopy(renderer, textTexture, nullptr, &destRect);
    
    // リソースを解放
    SDL_DestroyTexture(textTexture);
    SDL_FreeSurface(textSurface);
}

// ライフをハートアイコンで描画
void Game::RenderLives(int x, int y) {
    SDL_Color white = {255, 255, 255, 255}; // 白色
    SDL_Color red = {255, 100, 100, 255};   // 薄い赤色
    
    if (!font) return;  // フォントがない場合は何もしない
    
    // ライフを数字とシンプルな記号で表示
    std::string livesText = "Lives: " + std::to_string(lives);
    RenderText(livesText, x, y, white);
    
    // ライフの残り数に応じて追加の視覚的表現
    for (int i = 0; i < lives; i++) {
        // シンプルな記号を使用（文字化けしない）
        RenderText("*", x + 80 + (i * 15), y, red);  // アスタリスクをハート代わりに
    }
}

// 時間をMM:SS形式で描画
void Game::RenderTime(int x, int y) {
    if (!font) return;
    
    // 時間を分:秒形式に変換
    int minutes = gameTime / 60;
    int seconds = gameTime % 60;
    
    // MM:SS形式の文字列を作成
    std::string timeText = std::string("Time: ") + 
                          (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":" +
                          (seconds < 10 ? "0" : "") + std::to_string(seconds);
    
    SDL_Color yellow = {255, 255, 100, 255};  // 薄い黄色
    RenderText(timeText, x, y, yellow);
}

// UIリソースの解放
void Game::CleanupUI() {
    // フォントを解放
    if (font) {
        TTF_CloseFont(font);
        font = nullptr;
    }
    
    // SDL_ttfライブラリを終了
    TTF_Quit();
}

// === アイテムシステムの実装 ===

// アイテムシステムの初期化（アイテムの配置）
void Game::InitializeItems() {
    // 既存のアイテムをクリア
    items.clear();
    
    // === コインの配置（プラットフォーム上に配置） ===
    // 左側のプラットフォーム上にコイン
    items.push_back(Item(5 * TILE_SIZE + 10, 13 * TILE_SIZE - 25, COIN));
    items.push_back(Item(6 * TILE_SIZE + 10, 13 * TILE_SIZE - 25, COIN));
    items.push_back(Item(7 * TILE_SIZE + 10, 13 * TILE_SIZE - 25, COIN));
    
    // 中央のプラットフォーム上にコインとパワーアップ
    items.push_back(Item(10 * TILE_SIZE + 10, 11 * TILE_SIZE - 25, COIN));
    items.push_back(Item(11 * TILE_SIZE + 10, 11 * TILE_SIZE - 25, POWER_MUSHROOM));
    items.push_back(Item(12 * TILE_SIZE + 10, 11 * TILE_SIZE - 25, COIN));
    items.push_back(Item(13 * TILE_SIZE + 10, 11 * TILE_SIZE - 25, COIN));
    
    // 右側の高いプラットフォーム上にレアアイテム
    items.push_back(Item(18 * TILE_SIZE + 10, 9 * TILE_SIZE - 25, LIFE_UP));
    items.push_back(Item(19 * TILE_SIZE + 10, 9 * TILE_SIZE - 25, COIN));
    items.push_back(Item(20 * TILE_SIZE + 10, 9 * TILE_SIZE - 25, COIN));
    
    // 地面上にいくつかのコイン
    items.push_back(Item(8 * TILE_SIZE + 10, (MAP_HEIGHT - 3) * TILE_SIZE - 25, COIN));
    items.push_back(Item(15 * TILE_SIZE + 10, (MAP_HEIGHT - 3) * TILE_SIZE - 25, COIN));
    items.push_back(Item(22 * TILE_SIZE + 10, (MAP_HEIGHT - 3) * TILE_SIZE - 25, COIN));
    
    std::cout << "🎁 アイテムシステム初期化完了 - アイテム数: " << items.size() << std::endl;
}

// プレイヤーとアイテムの衝突判定
bool Game::CheckPlayerItemCollision(const Item& item) {
    if (!item.active || item.collected) {
        return false;
    }
    
    // SDL_HasIntersectionを使用して矩形の重なりを判定
    return SDL_HasIntersection(&playerRect, &item.rect);
}

// アイテム取得時の処理
void Game::HandleItemCollection(Item& item) {
    if (!item.active || item.collected) {
        return;
    }
    
    // アイテムを取得済みに設定
    item.collected = true;
    item.active = false;
    
    // アイテム収集エフェクト（種類に応じて異なるエフェクト）
    if (item.type == COIN) {
        CreateSoulCollectEffect(item.x, item.y);  // コインは魂エフェクト
    } else {
        SpawnParticleBurst(item.x, item.y, PARTICLE_SPARK, 12);  // その他は火花エフェクト
        StartScreenShake(2, 8);  // 軽い画面シェイク
    }
    
    // アイテムの効果を適用
    ApplyItemEffect(item.type);
    
    // 取得メッセージ表示
    switch (item.type) {
        case COIN:
            std::cout << "🪙 コイン取得！ +100点" << std::endl;
#ifdef SOUND_ENABLED
            PlaySound(coinSound);
#endif
            break;
        case POWER_MUSHROOM:
            std::cout << "🍄 パワーアップキノコ取得！" << std::endl;
#ifdef SOUND_ENABLED
            PlaySound(powerUpSound);
#endif
            break;
        case LIFE_UP:
            std::cout << "🔺 1UPキノコ取得！ +1ライフ" << std::endl;
#ifdef SOUND_ENABLED
            PlaySound(powerUpSound);
#endif
            break;
    }
    
    DisplayGameStatus();
}

// アイテムの描画処理
void Game::RenderItems() {
    for (const auto& item : items) {
        if (item.active && !item.collected) {
            // アイテム種類に応じて色を変更
            switch (item.type) {
                case COIN:
                    // 金色（黄色）
                    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
                    break;
                case POWER_MUSHROOM:
                    // 赤色（キノコ）
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    break;
                case LIFE_UP:
                    // 緑色（1UPキノコ）
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    break;
            }
            
            // アイテムを描画
            SDL_RenderFillRect(renderer, &item.rect);
            
            // アイテムの境界線を黒で描画（見やすさのため）
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &item.rect);
        }
    }
}

// 特定のアイテム効果を適用
void Game::ApplyItemEffect(ItemType itemType) {
    switch (itemType) {
        case COIN:
            score += 100;  // コインは100点
            break;
            
        case POWER_MUSHROOM:
            if (playerPowerLevel < 2) {  // 最大レベル2まで
                playerPowerLevel++;
                
                // プレイヤーサイズを大きくする
                if (playerPowerLevel == 1) {
                    playerRect.w = 32;  // 少し大きく
                    playerRect.h = 32;
                } else if (playerPowerLevel == 2) {
                    playerRect.w = 36;  // さらに大きく
                    playerRect.h = 36;
                }
                
                score += 1000;  // パワーアップボーナス
            }
            break;
            
        case LIFE_UP:
            lives++;
            score += 500;  // ライフアップボーナス
            break;
    }
}

// プレイヤーのパワーアップ状態を更新
void Game::UpdatePlayerPowerState() {
    // パワーアップ状態の管理（今後の拡張用）
}

// === サウンドシステムの実装 ===

#ifdef SOUND_ENABLED
// サウンドシステムの初期化
bool Game::InitializeSound() {
    // SDL_mixerを初期化
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cout << "SDL_mixer初期化エラー: " << Mix_GetError() << std::endl;
        return false;
    }
    
    std::cout << "🔊 SDL_mixer初期化成功" << std::endl;
    
    // サウンドファイルを読み込み
    if (!LoadSounds()) {
        std::cout << "サウンドファイル読み込みエラー（一部のサウンドが利用できません）" << std::endl;
        // サウンドファイルがなくてもゲームは続行
    }
    
    // 音量を設定（0-128の範囲）
    SetSoundVolume(64);  // 効果音は中音量
    SetMusicVolume(32);  // BGMは低音量
    
    return true;
}

// サウンドの読み込み
bool Game::LoadSounds() {
    bool allLoaded = true;
    
    // サウンドファイルのパス
    std::string soundDir = "assets/sounds/";
    
    // 効果音ファイルが存在する場合のみ読み込み
    if (CheckSoundFile(soundDir + "jump.wav")) {
        jumpSound = Mix_LoadWAV((soundDir + "jump.wav").c_str());
        if (!jumpSound) {
            std::cout << "ジャンプ音読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        }
    }
    
    if (CheckSoundFile(soundDir + "coin.wav")) {
        coinSound = Mix_LoadWAV((soundDir + "coin.wav").c_str());
        if (!coinSound) {
            std::cout << "コイン音読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        }
    }
    
    if (CheckSoundFile(soundDir + "powerup.wav")) {
        powerUpSound = Mix_LoadWAV((soundDir + "powerup.wav").c_str());
        if (!powerUpSound) {
            std::cout << "パワーアップ音読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        }
    }
    
    if (CheckSoundFile(soundDir + "enemy_defeat.wav")) {
        enemyDefeatedSound = Mix_LoadWAV((soundDir + "enemy_defeat.wav").c_str());
        if (!enemyDefeatedSound) {
            std::cout << "敵撃破音読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        }
    }
    
    if (CheckSoundFile(soundDir + "damage.wav")) {
        damageSound = Mix_LoadWAV((soundDir + "damage.wav").c_str());
        if (!damageSound) {
            std::cout << "ダメージ音読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        }
    }
    
    // BGMファイルが存在する場合のみ読み込み
    if (CheckSoundFile(soundDir + "bgm.ogg") || CheckSoundFile(soundDir + "bgm.mp3")) {
        std::string bgmFile = CheckSoundFile(soundDir + "bgm.ogg") ? 
                              soundDir + "bgm.ogg" : soundDir + "bgm.mp3";
        backgroundMusic = Mix_LoadMUS(bgmFile.c_str());
        if (!backgroundMusic) {
            std::cout << "BGM読み込みエラー: " << Mix_GetError() << std::endl;
            allLoaded = false;
        } else {
            // BGMを開始
            PlayMusic(backgroundMusic);
        }
    }
    
    if (allLoaded) {
        std::cout << "🎵 全サウンドファイル読み込み完了" << std::endl;
    } else {
        std::cout << "⚠️ 一部のサウンドファイルが見つかりません" << std::endl;
        std::cout << "サウンドファイルを assets/sounds/ ディレクトリに配置してください:" << std::endl;
        std::cout << "- jump.wav, coin.wav, powerup.wav, enemy_defeat.wav, damage.wav" << std::endl;
        std::cout << "- bgm.ogg または bgm.mp3" << std::endl;
    }
    
    return allLoaded;
}

// サウンドリソースの解放
void Game::CleanupSound() {
    // 効果音を解放
    if (jumpSound) {
        Mix_FreeChunk(jumpSound);
        jumpSound = nullptr;
    }
    if (coinSound) {
        Mix_FreeChunk(coinSound);
        coinSound = nullptr;
    }
    if (powerUpSound) {
        Mix_FreeChunk(powerUpSound);
        powerUpSound = nullptr;
    }
    if (enemyDefeatedSound) {
        Mix_FreeChunk(enemyDefeatedSound);
        enemyDefeatedSound = nullptr;
    }
    if (damageSound) {
        Mix_FreeChunk(damageSound);
        damageSound = nullptr;
    }
    
    // BGMを解放
    if (backgroundMusic) {
        Mix_FreeMusic(backgroundMusic);
        backgroundMusic = nullptr;
    }
    
    // SDL_mixerを終了
    Mix_CloseAudio();
    std::cout << "🔇 サウンドシステム終了" << std::endl;
}

// === ゲームコントローラーシステムの実装 ===

// ゲームコントローラーの初期化
void Game::InitializeController() {
    // SDL のゲームコントローラーサブシステムを初期化
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        std::cout << "⚠️ ゲームコントローラー初期化に失敗: " << SDL_GetError() << std::endl;
        controllerConnected = false;
        return;
    }
    
    // 接続されているコントローラーを検索
    int numJoysticks = SDL_NumJoysticks();
    std::cout << "🎮 検出されたコントローラー数: " << numJoysticks << std::endl;
    
    if (numJoysticks > 0) {
        // 最初のコントローラーを開く
        for (int i = 0; i < numJoysticks; i++) {
            if (SDL_IsGameController(i)) {
                gameController = SDL_GameControllerOpen(i);
                if (gameController) {
                    controllerConnected = true;
                    const char* name = SDL_GameControllerName(gameController);
                    std::cout << "🎮 コントローラー接続成功: " << (name ? name : "不明") << std::endl;
                    std::cout << "🕹️ コントローラー操作:" << std::endl;
                    std::cout << "   左スティック: 移動" << std::endl;
                    std::cout << "   Aボタン: ジャンプ/ウォールジャンプ" << std::endl;
                    std::cout << "   Xボタン: ダッシュ/エアダッシュ" << std::endl;
                    std::cout << "   Yボタン: 攻撃" << std::endl;
                    std::cout << "   Bボタン: 回復" << std::endl;
                    std::cout << "   Startボタン: 次のステージ（クリア後）" << std::endl;
                    std::cout << "   Selectボタン: リスタート" << std::endl;
                    break;
                }
            }
        }
    }
    
    if (!controllerConnected) {
        std::cout << "🎮 コントローラーが見つかりません。キーボード操作を使用してください。" << std::endl;
    }
}

// ゲームコントローラー入力処理
void Game::HandleControllerInput() {
    if (!controllerConnected || !gameController) return;
    
    // 移動処理（左スティック） - 連続入力対応
    float leftX = GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);
    if (leftX < -0.3f) {  // 左移動
        int newPlayerX = playerX - playerSpeed;
        if (!CheckHorizontalCollision(newPlayerX, playerY)) {
            playerX = newPlayerX;
        }
    } else if (leftX > 0.3f) {  // 右移動
        int newPlayerX = playerX + playerSpeed;
        if (!CheckHorizontalCollision(newPlayerX, playerY)) {
            playerX = newPlayerX;
        }
    }
    
    // ジャンプ（通常ジャンプ + ダブルジャンプ + ウォールジャンプ）（Aボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_A)) {
        if (isOnGround) {
            // 通常ジャンプ（地面から）
            playerVelY = -15.0f;
            isOnGround = false;
            isJumping = true;
            
#ifdef SOUND_ENABLED
            PlaySound(jumpSound);
#endif
        } else if (touchingWall && !isOnGround && canWallJump) {
            // ウォールジャンプ（最優先）
            HandleWallJump();
        } else if (CanPerformAirJump()) {
            // 空中ジャンプ（ダブルジャンプ）
            HandleAirJump();
        }
    }
    
    // ダッシュ（Xボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_X)) {
        if (canDash && dashCooldown <= 0) {
            if (isOnGround) {
                StartDash(lastDirection);  // 地上ダッシュ
            } else if (CanPerformAirDash()) {
                HandleAirDash();  // エアダッシュ
            }
        }
    }
    
    // 攻撃（Yボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_Y) && !isAttacking && attackCooldown <= 0) {
        StartAttack();
    }
    
    // 回復（Bボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_B) && soulCount >= 33 && playerHealth < maxHealth) {
        UseHeal();
    }
    
    // 次のステージ（Startボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_START) && stageCleared && !isTransitioning) {
        isTransitioning = true;
        NextStage();
    }
    
    // リスタート（Selectボタン） - 新規押下のみ
    if (GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_BACK) && !isTransitioning) {
        isTransitioning = true;
        ResetCurrentStage();
        isTransitioning = false;
    }
}

// コントローラーボタン状態取得
bool Game::GetControllerButton(SDL_GameControllerButton button) {
    if (!controllerConnected || !gameController) return false;
    return SDL_GameControllerGetButton(gameController, button) != 0;
}

// コントローラーボタン新規押下判定（入力遅延防止）
bool Game::GetControllerButtonPressed(SDL_GameControllerButton button) {
    if (!controllerConnected || !gameController) return false;
    
    bool currentState = SDL_GameControllerGetButton(gameController, button) != 0;
    bool wasPressed = currentState && !prevControllerButtons[button];
    
    return wasPressed;
}

// コントローラーボタン状態更新
void Game::UpdateControllerButtonStates() {
    if (!controllerConnected || !gameController) return;
    
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        prevControllerButtons[i] = SDL_GameControllerGetButton(gameController, (SDL_GameControllerButton)i) != 0;
    }
}

// コントローラー軸状態取得
float Game::GetControllerAxis(SDL_GameControllerAxis axis) {
    if (!controllerConnected || !gameController) return 0.0f;
    
    Sint16 value = SDL_GameControllerGetAxis(gameController, axis);
    // -32768 ～ 32767 の値を -1.0f ～ 1.0f に正規化
    return value / 32767.0f;
}

// ゲームコントローラーの終了処理
void Game::CleanupController() {
    if (gameController) {
        SDL_GameControllerClose(gameController);
        gameController = nullptr;
        std::cout << "🎮 ゲームコントローラー終了" << std::endl;
    }
    controllerConnected = false;
    
    // ボタン状態をリセット
    for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
        prevControllerButtons[i] = false;
    }
}

// 効果音の再生
void Game::PlaySound(Mix_Chunk* sound) {
    if (soundEnabled && sound) {
        Mix_PlayChannel(-1, sound, 0);
    }
}

// === ゲーム状態管理システムの実装 ===

// タイトル画面の入力処理
void Game::HandleTitleInput() {
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // メニュー選択（上下キー）
    static bool upPressed = false, downPressed = false;
    
    if ((currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) && !upPressed) {
        titleMenuSelection = (titleMenuSelection - 1 + 3) % 3;  // 3つのメニュー項目
        upPressed = true;
    } else if (!currentKeyStates[SDL_SCANCODE_UP] && !currentKeyStates[SDL_SCANCODE_W]) {
        upPressed = false;
    }
    
    if ((currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) && !downPressed) {
        titleMenuSelection = (titleMenuSelection + 1) % 3;
        downPressed = true;
    } else if (!currentKeyStates[SDL_SCANCODE_DOWN] && !currentKeyStates[SDL_SCANCODE_S]) {
        downPressed = false;
    }
    
    // コントローラー入力（メニュー選択）
    if (controllerConnected && gameController) {
        float leftY = GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);
        static bool stickUpPressed = false, stickDownPressed = false;
        
        if (leftY < -0.5f && !stickUpPressed) {
            titleMenuSelection = (titleMenuSelection - 1 + 3) % 3;
            stickUpPressed = true;
        } else if (leftY >= -0.3f) {
            stickUpPressed = false;
        }
        
        if (leftY > 0.5f && !stickDownPressed) {
            titleMenuSelection = (titleMenuSelection + 1) % 3;
            stickDownPressed = true;
        } else if (leftY <= 0.3f) {
            stickDownPressed = false;
        }
    }
    
    // 決定キー（Enter, Space, コントローラーAボタン）
    bool enterPressed = currentKeyStates[SDL_SCANCODE_RETURN] || 
                       currentKeyStates[SDL_SCANCODE_SPACE] ||
                       (controllerConnected && GetControllerButtonPressed(SDL_CONTROLLER_BUTTON_A));
    
    // 任意キーでゲーム開始（最初の状態）
    bool anyKeyPressed = false;
    for (int i = 0; i < 512; i++) {
        if (currentKeyStates[i]) {
            anyKeyPressed = true;
            break;
        }
    }
    
    if (controllerConnected && gameController) {
        for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; i++) {
            if (GetControllerButton((SDL_GameControllerButton)i)) {
                anyKeyPressed = true;
                break;
            }
        }
    }
    
    if (showPressAnyKey && anyKeyPressed) {
        showPressAnyKey = false;
        return;
    }
    
    if (!showPressAnyKey && enterPressed) {
        switch (titleMenuSelection) {
            case 0:  // Start Game
                StartNewGame();
                break;
            case 1:  // Credits
                ShowCredits();
                break;
            case 2:  // Exit
                isRunning = false;
                break;
        }
    }
}

// タイトル画面の更新処理
void Game::UpdateTitle() {
    titleAnimationTimer++;
    
    // タイトルの光エフェクトアニメーション
    titleGlowEffect = 0.5f + 0.3f * sin(titleAnimationTimer * 0.05f);
    
    // "Press Any Key"の点滅
    if (showPressAnyKey && (titleAnimationTimer / 30) % 2 == 0) {
        // 点滅エフェクト
    }
}

// タイトル画面の描画処理
void Game::RenderTitle() {
    // === ダークな背景グラデーション ===
    DrawGradientRect({0, 0, 800, 600}, 
                     ColorPalette::BACKGROUND_DARK, 
                     {10, 5, 15, 255});
    
    // === タイトルロゴ（テキスト）===
    SDL_Color titleColor = ColorPalette::UI_PRIMARY;
    titleColor.r = (Uint8)(titleColor.r * titleGlowEffect);
    titleColor.g = (Uint8)(titleColor.g * titleGlowEffect);
    titleColor.b = (Uint8)(titleColor.b * titleGlowEffect);
    
    // タイトルの光エフェクト
    DrawGlowEffect(400, 150, 60, ColorPalette::SOUL_BLUE, titleGlowEffect);
    
    if (font) {
        RenderText("HOLLOW KNIGHT STYLE", 250, 120, titleColor);
        RenderText("2D ACTION GAME", 280, 160, ColorPalette::UI_SECONDARY);
    }
    
    if (showPressAnyKey) {
        // === "Press Any Key" 表示 ===
        if ((titleAnimationTimer / 30) % 2 == 0) {
            if (font) {
                RenderText("Press Any Key to Continue", 250, 400, ColorPalette::UI_ACCENT);
            }
        }
    } else {
        // === メニュー項目 ===
        const char* menuItems[] = {"Start Game", "Credits", "Exit"};
        for (int i = 0; i < 3; i++) {
            SDL_Color menuColor = (i == titleMenuSelection) ? 
                                 ColorPalette::UI_ACCENT : ColorPalette::UI_SECONDARY;
            
            // 選択中の項目に光エフェクト
            if (i == titleMenuSelection) {
                DrawGlowEffect(400, 300 + i * 50, 30, ColorPalette::UI_ACCENT, 0.7f);
            }
            
            if (font) {
                RenderText(menuItems[i], 350, 280 + i * 50, menuColor);
            }
        }
        
        // === 操作説明 ===
        if (font) {
            RenderText("Arrow Keys: Select  Enter/Space: Confirm", 200, 500, ColorPalette::UI_SECONDARY);
            if (controllerConnected) {
                RenderText("Controller: Left Stick + A Button", 220, 530, ColorPalette::UI_SECONDARY);
            }
        }
    }
    
    // === 装飾的なパーティクル ===
    // 背景に少し光る粒子を追加
    for (int i = 0; i < 20; i++) {
        int x = (titleAnimationTimer * 2 + i * 40) % 900 - 50;
        int y = 100 + (i * 30) % 400;
        float alpha = 0.3f + 0.2f * sin((titleAnimationTimer + i * 10) * 0.1f);
        
        SetRenderColorWithAlpha(ColorPalette::SOUL_BLUE, alpha);
        SDL_Rect particle = {x, y, 3, 3};
        SDL_RenderFillRect(renderer, &particle);
    }
}

// ゲーム状態変更
void Game::ChangeGameState(GameState newState) {
    currentGameState = newState;
    
    switch (newState) {
        case STATE_TITLE:
            std::cout << "🏠 タイトル画面に戻りました" << std::endl;
            break;
        case STATE_PLAYING:
            std::cout << "🎮 ゲーム開始！" << std::endl;
            break;
        case STATE_PAUSED:
            std::cout << "⏸️ ゲームを一時停止しました" << std::endl;
            break;
        case STATE_GAME_OVER:
            std::cout << "💀 ゲームオーバー" << std::endl;
            break;
        case STATE_CREDITS:
            std::cout << "🎬 クレジット表示" << std::endl;
            break;
    }
}

// 新しいゲーム開始
void Game::StartNewGame() {
    // プレイヤー状態をリセット
    ResetPlayerState();
    
    // スコアとライフをリセット
    score = 0;
    lives = 3;
    
    // 最初のステージをロード
    currentStageIndex = 0;
    LoadStage(0);
    
    // ゲーム状態をプレイ中に変更
    ChangeGameState(STATE_PLAYING);
}

// クレジット表示
void Game::ShowCredits() {
    std::cout << "🎬 クレジット:" << std::endl;
    std::cout << "   Game: Hollow Knight Style 2D Action" << std::endl;
    std::cout << "   Engine: SDL2" << std::endl;
    std::cout << "   Programming: C++" << std::endl;
    std::cout << "   Inspired by: Team Cherry's Hollow Knight" << std::endl;
    
    // タイトルに戻る
    ChangeGameState(STATE_TITLE);
}

// ゲームプレイ中の描画処理
void Game::RenderGameplay() {
    // === 美化された背景描画（カメラ固定）===
    RenderGradientBackground();
    
    // === 美化されたタイル描画（カメラオフセット適用）===
    RenderEnhancedTiles();
    
    // === 美化されたプレイヤー描画（カメラオフセット適用）===
    RenderEnhancedPlayer();
    
    // === 敵キャラクターの描画（カメラオフセット適用）===
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            // カメラオフセットを適用した描画位置を計算
            int screenX = WorldToScreenX(enemy.x);
            int screenY = WorldToScreenY(enemy.y);
            
            // 画面外にいる場合は描画しない
            if (screenX + enemy.rect.w < 0 || screenX > SCREEN_WIDTH || 
                screenY + enemy.rect.h < 0 || screenY > SCREEN_HEIGHT) {
                continue;
            }
            
            SDL_Rect enemyScreenRect = {screenX, screenY, enemy.rect.w, enemy.rect.h};
            
            // 敵も少し美化
            SetRenderColorWithAlpha(ColorPalette::DAMAGE_RED, 0.9f);
            SDL_RenderFillRect(renderer, &enemyScreenRect);
            
            // 敵の縁取り
            SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 0.7f);
            SDL_RenderDrawRect(renderer, &enemyScreenRect);
        }
    }
    
    // === アイテムの描画（美化版・カメラオフセット適用）===
    for (const auto& item : items) {
        if (item.active && !item.collected) {
            // カメラオフセットを適用した描画位置を計算
            int screenX = WorldToScreenX(item.x);
            int screenY = WorldToScreenY(item.y);
            
            // 画面外にいる場合は描画しない
            if (screenX + item.rect.w < 0 || screenX > SCREEN_WIDTH || 
                screenY + item.rect.h < 0 || screenY > SCREEN_HEIGHT) {
                continue;
            }
            
            SDL_Rect itemScreenRect = {screenX, screenY, item.rect.w, item.rect.h};
            
            // アイテム種類に応じて美化された色を設定
            switch (item.type) {
                case COIN:
                    SetRenderColorWithAlpha(ColorPalette::SOUL_BLUE, 1.0f);
                    break;
                case POWER_MUSHROOM:
                    SetRenderColorWithAlpha(ColorPalette::DAMAGE_RED, 1.0f);
                    break;
                case LIFE_UP:
                    SetRenderColorWithAlpha(ColorPalette::HEALTH_GREEN, 1.0f);
                    break;
            }
            
            // アイテム本体を描画
            SDL_RenderFillRect(renderer, &itemScreenRect);
            
            // アイテムの光エフェクト（カメラオフセット適用）
            int centerX = screenX + item.rect.w / 2;
            int centerY = screenY + item.rect.h / 2;
            DrawGlowEffect(centerX, centerY, 12, ColorPalette::UI_ACCENT, 0.5f);
            
            // アイテムの境界線
            SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
            SDL_RenderDrawRect(renderer, &itemScreenRect);
        }
    }
    
    // === ゴールの描画（美化版・カメラオフセット適用）===
    if (goal && goal->active) {
        // カメラオフセットを適用した描画位置を計算
        int screenX = WorldToScreenX(goal->x);
        int screenY = WorldToScreenY(goal->y);
        
        // 画面外にいる場合は描画しない
        if (screenX + goal->rect.w < 0 || screenX > SCREEN_WIDTH || 
            screenY + goal->rect.h < 0 || screenY > SCREEN_HEIGHT) {
            // ゴールは重要なので、画面外でも処理は継続
        } else {
            SDL_Rect goalScreenRect = {screenX, screenY, goal->rect.w, goal->rect.h};
            
            // ゴール種類に応じて美化された色を設定
            switch (goal->type) {
                case GOAL_FLAG:
                    SetRenderColorWithAlpha(ColorPalette::SOUL_BLUE, 1.0f);
                    break;
                case GOAL_DOOR:
                    SetRenderColorWithAlpha(ColorPalette::UI_ACCENT, 1.0f);
                    break;
                case GOAL_COLLECT_ALL:
                    SetRenderColorWithAlpha(ColorPalette::HEALTH_GREEN, 1.0f);
                    break;
            }
            
            // ゴールを描画
            SDL_RenderFillRect(renderer, &goalScreenRect);
            
            // ゴールの光エフェクト（カメラオフセット適用）
            int centerX = screenX + goal->rect.w / 2;
            int centerY = screenY + goal->rect.h / 2;
            DrawGlowEffect(centerX, centerY, 20, ColorPalette::UI_ACCENT, 0.8f);
            
            // ゴールの境界線
            SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
            SDL_RenderDrawRect(renderer, &goalScreenRect);
        }
    }
    
    // === ボス戦システムの描画 ===
    RenderBoss();
    RenderBossProjectiles();
    
    // === 敵の弾丸描画 ===
    RenderEnemyProjectiles();
    
    // === エフェクトシステムの描画 ===
    RenderParticles();
    
    // === 光線描画 ===
    RenderBeam();
    
    // === 美化されたUI描画 ===
    RenderEnhancedUI();
}

// === カメラシステムの実装 ===

// カメラの更新処理
void Game::UpdateCamera() {
    // プレイヤーの中心位置を取得
    int playerCenterX = playerX + playerRect.w / 2;
    int playerCenterY = playerY + playerRect.h / 2;
    
    // カメラの目標位置を計算（プレイヤーを画面中央に）
    float targetCameraX = playerCenterX - SCREEN_WIDTH / 2;
    float targetCameraY = playerCenterY - SCREEN_HEIGHT / 2;
    
    // デッドゾーンを考慮した滑らかなカメラ追従
    float deltaX = targetCameraX - cameraX;
    float deltaY = targetCameraY - cameraY;
    
    // デッドゾーン外の場合のみカメラを移動
    if (abs(deltaX) > cameraDeadZone) {
        cameraX += deltaX * cameraFollowSpeed;
    }
    if (abs(deltaY) > cameraDeadZone) {
        cameraY += deltaY * cameraFollowSpeed;
    }
    
    // カメラをワールド範囲内に制限
    ClampCameraToWorld();
}

// カメラをワールド範囲内に制限
void Game::ClampCameraToWorld() {
    // X軸の制限
    float minCameraX = 0;
    float maxCameraX = (MAP_WIDTH * TILE_SIZE) - SCREEN_WIDTH;
    
    if (cameraX < minCameraX) cameraX = minCameraX;
    if (cameraX > maxCameraX) cameraX = maxCameraX;
    
    // Y軸の制限
    float minCameraY = 0;
    float maxCameraY = (MAP_HEIGHT * TILE_SIZE) - SCREEN_HEIGHT;
    
    if (cameraY < minCameraY) cameraY = minCameraY;
    if (cameraY > maxCameraY) cameraY = maxCameraY;
}

// ワールド座標をスクリーン座標に変換（X軸）
int Game::WorldToScreenX(int worldX) {
    return worldX - (int)cameraX;
}

// ワールド座標をスクリーン座標に変換（Y軸）
int Game::WorldToScreenY(int worldY) {
    return worldY - (int)cameraY;
}

// BGMの再生
void Game::PlayMusic(Mix_Music* music, int loops) {
    if (soundEnabled && music) {
        Mix_PlayMusic(music, loops);  // -1 = 無限ループ
    }
}

// BGMの停止
void Game::StopMusic() {
    Mix_HaltMusic();
}

// ボリューム設定
void Game::SetSoundVolume(int volume) {
    Mix_Volume(-1, volume);  // 全チャンネルの音量設定
}

void Game::SetMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}

// サウンドファイルが存在するかチェック
bool Game::CheckSoundFile(const std::string& filename) {
    std::ifstream file(filename);
    return file.good();
}
#endif  // SOUND_ENABLED



// === ステージシステムの実装 ===

// ステージシステムの初期化
void Game::InitializeStages() {
    stages.clear();
    
    // ステージ1: 基本的なプラットフォームアクション（横スクロール対応）
    stages.push_back(CreateStage1Long());
    
    // ステージ2: より複雑な地形
    stages.push_back(CreateStage2());
    
    // ステージ3: 最終ステージ
    stages.push_back(CreateStage3());
    
    // ボスステージ: 強大なボスとの戦い
    stages.push_back(CreateBossStage());
    
    // 最初のステージを読み込み
    LoadStage(0);
}

// 現在のステージを読み込み
void Game::LoadStage(int stageIndex) {
    if (stageIndex < 0 || stageIndex >= (int)stages.size()) {
        std::cout << "❌ 無効なステージインデックス: " << stageIndex << std::endl;
        return;
    }
    
    currentStageIndex = stageIndex;
    const StageData& stage = stages[stageIndex];
    
    // マップデータをコピー
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = stage.mapData[y][x];
        }
    }
    
    // プレイヤー位置を設定
    playerX = stage.playerStartX;
    playerY = stage.playerStartY;
    initialPlayerX = stage.playerStartX;
    initialPlayerY = stage.playerStartY;
    
    // 既存の敵とアイテムをクリア
    enemies.clear();
    items.clear();
    
    // 敵を配置（新しいシステムで多様な敵タイプ）
    for (size_t i = 0; i < stage.enemyPositions.size(); i++) {
        const auto& enemyPos = stage.enemyPositions[i];
        // 敵タイプをランダムまたは位置に基づいて決定
        EnemyType enemyType;
        if (i % 5 == 0) {
            enemyType = ENEMY_SHOOTER;  // 5体に1体は射撃敵
        } else if (i % 4 == 0) {
            enemyType = ENEMY_JUMPER;   // 4体に1体はジャンプ敵
        } else if (i % 3 == 0) {
            enemyType = ENEMY_CHASER;   // 3体に1体は追跡敵
        } else if (i % 6 == 0) {
            enemyType = ENEMY_FLYING;   // 6体に1体は飛行敵
        } else {
            enemyType = ENEMY_GOOMBA;   // その他は基本敵
        }
        
        Enemy enemy(enemyPos.first, enemyPos.second, enemyType);
        enemies.push_back(enemy);
    }
    
    // アイテムを配置
    for (const auto& itemData : stage.itemPositions) {
        const auto& pos = itemData.first;
        ItemType itemType = itemData.second;
        items.push_back(Item(pos.first, pos.second, itemType));
    }
    
    // ゴールを設定
    if (goal) {
        delete goal;
    }
    goal = new Goal(stage.goalX, stage.goalY, stage.goalType);
    
    // ステージ状態をリセット
    stageCleared = false;
    isTransitioning = false;
    remainingTime = stage.timeLimit;
    
    // プレイヤー状態をリセット
    playerVelY = 0;
    isOnGround = false;
    isJumping = false;
    playerRect.x = playerX;
    playerRect.y = playerY;
    
    // ボス戦チェック（ステージ4がボスステージ）
    if (stageIndex == 3) {  // ボスステージ（0から数えて3番目）
        bossStageIndex = stageIndex;
        StartBossFight();
    } else {
        // 通常ステージの場合はボス戦フラグをリセット
        isBossFight = false;
        bossDefeated = false;
    }
    
    std::cout << "🚀 " << stage.stageName << " を読み込みました" << std::endl;
}

// 次のステージに進む
void Game::NextStage() {
    if (currentStageIndex < (int)stages.size() - 1) {
        LoadStage(currentStageIndex + 1);
    } else {
        allStagesCleared = true;
        std::cout << "🎉 全ステージクリア！" << std::endl;
    }
}

// ステージをリセット（再スタート）
void Game::ResetCurrentStage() {
    LoadStage(currentStageIndex);
}

// プレイヤーとゴールの衝突判定
bool Game::CheckPlayerGoalCollision() {
    if (!goal || !goal->active) {
        return false;
    }
    
    return SDL_HasIntersection(&playerRect, &goal->rect);
}

// ステージクリア処理
void Game::HandleStageClear() {
    stageCleared = true;
    goal->active = false;
    
    // スコアボーナス
    score += 1000;
    if (remainingTime > 0) {
        score += remainingTime * 10;  // 残り時間ボーナス
    }
    
    std::cout << "🎯 ステージクリア！ ボーナス: " << (1000 + (remainingTime * 10)) << "点" << std::endl;
    DisplayGameStatus();
    
    // 即座に次のステージへ進む
    NextStage();
}

// ゴールの描画処理
void Game::RenderGoal() {
    if (!goal || !goal->active) {
        return;
    }
    
    // ゴール種類に応じて色を変更
    switch (goal->type) {
        case GOAL_FLAG:
            // 黄色（フラッグポール）
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            break;
        case GOAL_DOOR:
            // 紫色（ドア）
            SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255);
            break;
        case GOAL_COLLECT_ALL:
            // オレンジ色（収集ゴール）
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            break;
    }
    
    // ゴールを描画
    SDL_RenderFillRect(renderer, &goal->rect);
    
    // ゴールの境界線を黒で描画
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &goal->rect);
}

// ステージデータの作成（ステージ1）
StageData Game::CreateStage1() {
    StageData stage;
    stage.stageNumber = 1;
    stage.stageName = "Stage 1 - Tutorial";
    stage.goalType = GOAL_FLAG;
    stage.goalX = 22 * TILE_SIZE;
    stage.goalY = (MAP_HEIGHT - 3) * TILE_SIZE - 64;
    stage.playerStartX = 100;
    stage.playerStartY = 300;
    stage.timeLimit = 0;  // 無制限
    
    // 基本的な地面を作成
    for (int x = 0; x < MAP_WIDTH; x++) {
        stage.mapData[MAP_HEIGHT - 1][x] = 1;  // 地面
        stage.mapData[MAP_HEIGHT - 2][x] = 1;  // 地面（厚み）
    }
    
    // プラットフォームを追加
    stage.mapData[14][5] = 1;
    stage.mapData[14][6] = 1;
    stage.mapData[14][7] = 1;
    
    stage.mapData[12][10] = 1;
    stage.mapData[12][11] = 1;
    stage.mapData[12][12] = 1;
    stage.mapData[12][13] = 1;
    
    // 敵の配置
    stage.enemyPositions.push_back({400, (MAP_HEIGHT - 3) * TILE_SIZE});
    
    // アイテムの配置
    stage.itemPositions.push_back({{5 * TILE_SIZE + 10, 13 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{6 * TILE_SIZE + 10, 13 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{11 * TILE_SIZE + 10, 11 * TILE_SIZE - 25}, POWER_MUSHROOM});
    
    return stage;
}

// 横スクロール対応の長いステージ1の作成
StageData Game::CreateStage1Long() {
    StageData stage;
    stage.stageNumber = 1;
    stage.stageName = "Stage 1 - Long Journey";
    stage.goalType = GOAL_FLAG;
    stage.goalX = 98 * TILE_SIZE;  // 台座の右端（ブロックのない場所）
    stage.goalY = (MAP_HEIGHT - 6) * TILE_SIZE - 32;  // 台座の上に配置
    stage.playerStartX = 100;
    stage.playerStartY = 300;
    stage.timeLimit = 0;  // 無制限
    
    // まず全体を空にする
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            stage.mapData[y][x] = 0;
        }
    }
    
    // === 基本的な地面を作成 ===
    for (int x = 0; x < MAP_WIDTH; x++) {
        stage.mapData[MAP_HEIGHT - 1][x] = 1;  // 地面
        stage.mapData[MAP_HEIGHT - 2][x] = 1;  // 地面（厚み）
    }
    
    // === セクション1: 開始エリア（X: 0-20）===
    // 最初のプラットフォーム
    for (int x = 5; x < 8; x++) {
        stage.mapData[14][x] = 1;
    }
    for (int x = 10; x < 15; x++) {
        stage.mapData[12][x] = 1;
    }
    
    // === セクション2: ジャンプチャレンジ（X: 20-40）===
    // 連続プラットフォーム
    for (int x = 22; x < 25; x++) {
        stage.mapData[13][x] = 1;
    }
    for (int x = 27; x < 30; x++) {
        stage.mapData[11][x] = 1;
    }
    for (int x = 32; x < 35; x++) {
        stage.mapData[9][x] = 1;
    }
    for (int x = 37; x < 40; x++) {
        stage.mapData[11][x] = 1;
    }
    
    // === セクション3: 壁ジャンプエリア（X: 40-60）===
    // 左側の高い壁
    for (int y = 10; y < MAP_HEIGHT - 2; y++) {
        stage.mapData[y][42] = 1;
    }
    // 右側の高い壁
    for (int y = 8; y < MAP_HEIGHT - 2; y++) {
        stage.mapData[y][58] = 1;
    }
    // 中間のプラットフォーム
    for (int x = 45; x < 48; x++) {
        stage.mapData[14][x] = 1;
    }
    for (int x = 52; x < 55; x++) {
        stage.mapData[10][x] = 1;
    }
    
    // === セクション4: 複雑な地形（X: 60-80）===
    // 階段状の地形
    for (int i = 0; i < 8; i++) {
        for (int y = MAP_HEIGHT - 3 - i; y < MAP_HEIGHT - 1; y++) {
            stage.mapData[y][62 + i] = 1;
        }
    }
    
    // 上部プラットフォーム
    for (int x = 72; x < 78; x++) {
        stage.mapData[8][x] = 1;
    }
    
    // === セクション5: ゴールエリア（X: 80-100）===
    // ゴール前の最終チャレンジ
    for (int x = 82; x < 85; x++) {
        stage.mapData[12][x] = 1;
    }
    for (int x = 88; x < 91; x++) {
        stage.mapData[10][x] = 1;
    }
    
    // ゴール台座
    for (int x = 94; x < 98; x++) {
        for (int y = MAP_HEIGHT - 5; y < MAP_HEIGHT - 1; y++) {
            stage.mapData[y][x] = 1;
        }
    }
    
    // === 敵の配置（全体に分散）===
    stage.enemyPositions.push_back({6 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({25 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({45 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({70 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({85 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    
    // === アイテムの配置（探索を促すために隠された場所に）===
    // セクション1のアイテム
    stage.itemPositions.push_back({{6 * TILE_SIZE, 13 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{12 * TILE_SIZE, 11 * TILE_SIZE - 25}, COIN});
    
    // セクション2のアイテム
    stage.itemPositions.push_back({{23 * TILE_SIZE, 12 * TILE_SIZE - 25}, POWER_MUSHROOM});
    stage.itemPositions.push_back({{33 * TILE_SIZE, 8 * TILE_SIZE - 25}, COIN});
    
    // セクション3のアイテム
    stage.itemPositions.push_back({{46 * TILE_SIZE, 13 * TILE_SIZE - 25}, LIFE_UP});
    stage.itemPositions.push_back({{53 * TILE_SIZE, 9 * TILE_SIZE - 25}, COIN});
    
    // セクション4のアイテム
    stage.itemPositions.push_back({{75 * TILE_SIZE, 7 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{67 * TILE_SIZE, (MAP_HEIGHT - 8) * TILE_SIZE - 25}, COIN});
    
    // セクション5のアイテム
    stage.itemPositions.push_back({{89 * TILE_SIZE, 9 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{95 * TILE_SIZE, (MAP_HEIGHT - 4) * TILE_SIZE - 25}, POWER_MUSHROOM});
    
    return stage;
}

// ステージデータの作成（ステージ2）- 横スクロール対応
StageData Game::CreateStage2() {
    StageData stage;
    stage.stageNumber = 2;
    stage.stageName = "Stage 2 - Underground Cave";
    stage.goalType = GOAL_FLAG;
    stage.goalX = 94 * TILE_SIZE;  // 台座の右端（ブロックのない場所）
    stage.goalY = (MAP_HEIGHT - 11) * TILE_SIZE - 32;  // 台座の上に配置
    stage.playerStartX = 100;
    stage.playerStartY = 300;
    stage.timeLimit = 240;  // 4分（長いステージなので延長）
    
    // まず全体を空にする
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            stage.mapData[y][x] = 0;
        }
    }
    
    // === 基本地面 ===
    for (int x = 0; x < MAP_WIDTH; x++) {
        stage.mapData[MAP_HEIGHT - 1][x] = 1;
        stage.mapData[MAP_HEIGHT - 2][x] = 1;
    }
    
    // === セクション1: 洞窟入口（X: 0-25）===
    // 洞窟の天井
    for (int x = 15; x < MAP_WIDTH; x++) {
        stage.mapData[5][x] = 1;
    }
    
    // 入口の鍾乳石
    for (int x = 8; x < 12; x++) {
        stage.mapData[14][x] = 1;
    }
    for (int x = 18; x < 22; x++) {
        stage.mapData[12][x] = 1;
    }
    
    // === セクション2: 狭い通路（X: 25-45）===
    // 上下から迫る壁
    for (int x = 25; x < 45; x++) {
        stage.mapData[6][x] = 1;  // 天井
        if ((x - 25) % 8 < 4) {
            stage.mapData[15][x] = 1;  // 床から突き出る障害物
        }
    }
    
    // === セクション3: 地下湖エリア（X: 45-65）===
    // 水面（床を少し上に）
    for (int x = 45; x < 65; x++) {
        stage.mapData[MAP_HEIGHT - 4][x] = 1;  // 水面の代わりに床
        stage.mapData[MAP_HEIGHT - 3][x] = 0;  // 元の床を削除
    }
    
    // 飛び石プラットフォーム
    for (int i = 0; i < 4; i++) {
        int x = 48 + i * 5;
        stage.mapData[MAP_HEIGHT - 6][x] = 1;
        stage.mapData[MAP_HEIGHT - 6][x + 1] = 1;
    }
    
    // === セクション4: クリスタル洞窟（X: 65-85）===
    // 複雑な上下構造
    for (int x = 65; x < 85; x++) {
        if ((x - 65) % 6 < 3) {
            stage.mapData[8][x] = 1;   // 上層プラットフォーム
        }
        if ((x - 67) % 6 < 3) {
            stage.mapData[13][x] = 1;  // 中層プラットフォーム
        }
    }
    
    // 垂直ブロックチェーン（壁ジャンプ練習）
    for (int y = 8; y < 14; y++) {
        stage.mapData[y][75] = 1;
        stage.mapData[y][80] = 1;
    }
    
    // === セクション5: 洞窟出口（X: 85-100）===
    // 最終チャレンジ - 急な登り
    for (int i = 0; i < 7; i++) {
        for (int y = MAP_HEIGHT - 3 - i; y < MAP_HEIGHT - 1; y++) {
            stage.mapData[y][87 + i] = 1;
        }
    }
    
    // ゴール台座
    for (int x = 90; x < 94; x++) {
        for (int y = MAP_HEIGHT - 10; y < MAP_HEIGHT - 1; y++) {
            stage.mapData[y][x] = 1;
        }
    }
    
    // === 敵の配置（洞窟生物）===
    stage.enemyPositions.push_back({10 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({30 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({50 * TILE_SIZE, (MAP_HEIGHT - 5) * TILE_SIZE});
    stage.enemyPositions.push_back({70 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    stage.enemyPositions.push_back({85 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});
    
    // === アイテム配置（洞窟の宝物）===
    // セクション1
    stage.itemPositions.push_back({{10 * TILE_SIZE, 13 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{20 * TILE_SIZE, 11 * TILE_SIZE - 25}, COIN});
    
    // セクション2
    stage.itemPositions.push_back({{35 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE - 25}, POWER_MUSHROOM});
    
    // セクション3
    stage.itemPositions.push_back({{50 * TILE_SIZE, (MAP_HEIGHT - 7) * TILE_SIZE - 25}, LIFE_UP});
    stage.itemPositions.push_back({{58 * TILE_SIZE, (MAP_HEIGHT - 7) * TILE_SIZE - 25}, COIN});
    
    // セクション4
    stage.itemPositions.push_back({{70 * TILE_SIZE, 7 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{77 * TILE_SIZE, 12 * TILE_SIZE - 25}, COIN});
    
    // セクション5
    stage.itemPositions.push_back({{90 * TILE_SIZE, (MAP_HEIGHT - 8) * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{91 * TILE_SIZE, (MAP_HEIGHT - 11) * TILE_SIZE - 25}, POWER_MUSHROOM});
    
    return stage;
}

// ステージデータの作成（ステージ3）- 横スクロール対応
StageData Game::CreateStage3() {
    StageData stage;
    stage.stageNumber = 3;
    stage.stageName = "Stage 3 - Sky Fortress";
    stage.goalType = GOAL_DOOR;
    stage.goalX = 90 * TILE_SIZE;  // 台座の右端（ブロックのない場所）
    stage.goalY = 4 * TILE_SIZE;   // 台座の上に配置
    stage.playerStartX = 100;
    stage.playerStartY = 300;
    stage.timeLimit = 300;  // 5分（最も長いステージ）
    
    // まず全体を空にする
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            stage.mapData[y][x] = 0;
        }
    }
    
    // === 基本地面（部分的に）===
    for (int x = 0; x < 20; x++) {
        stage.mapData[MAP_HEIGHT - 1][x] = 1;
        stage.mapData[MAP_HEIGHT - 2][x] = 1;
    }
    
    // === セクション1: 地上発射台（X: 0-20）===
    // 発射台への階段
    for (int i = 0; i < 5; i++) {
        for (int y = MAP_HEIGHT - 2 - i; y < MAP_HEIGHT; y++) {
            stage.mapData[y][10 + i] = 1;
        }
    }
    
    // === セクション2: 低空エリア（X: 20-40）===
    // 浮遊プラットフォーム群
    for (int x = 22; x < 26; x++) {
        stage.mapData[14][x] = 1;
    }
    for (int x = 30; x < 34; x++) {
        stage.mapData[12][x] = 1;
    }
    for (int x = 36; x < 40; x++) {
        stage.mapData[10][x] = 1;
    }
    
    // === セクション3: 中空エリア（X: 40-60）===
    // より高い浮遊プラットフォーム
    for (int x = 42; x < 45; x++) {
        stage.mapData[8][x] = 1;
    }
    for (int x = 48; x < 51; x++) {
        stage.mapData[9][x] = 1;
    }
    for (int x = 54; x < 57; x++) {
        stage.mapData[7][x] = 1;
    }
    
    // 縦の柱（ジャンプチャレンジ）
    for (int y = 10; y < 15; y++) {
        stage.mapData[y][46] = 1;
        stage.mapData[y][52] = 1;
    }
    
    // === セクション4: 高空エリア（X: 60-80）===
    // 非常に高いプラットフォーム
    for (int x = 62; x < 65; x++) {
        stage.mapData[5][x] = 1;
    }
    for (int x = 68; x < 71; x++) {
        stage.mapData[4][x] = 1;
    }
    for (int x = 74; x < 77; x++) {
        stage.mapData[6][x] = 1;
    }
    
    // 空中の支柱
    for (int y = 7; y < 12; y++) {
        stage.mapData[y][66] = 1;
        stage.mapData[y][72] = 1;
    }
    
    // === セクション5: 天空城塞（X: 80-100）===
    // 城塞の基礎構造
    for (int x = 82; x < 90; x++) {
        for (int y = 5; y < 8; y++) {
            stage.mapData[y][x] = 1;
        }
    }
    
    // 城塞の塔
    for (int y = 3; y < 8; y++) {
        stage.mapData[y][84] = 1;
        stage.mapData[y][87] = 1;
    }
    
    // ゴールへの最終階段
    for (int i = 0; i < 4; i++) {
        for (int y = 6 + i; y < 8; y++) {
            stage.mapData[y][90 + i] = 1;
        }
    }
    
    // ゴール台座（空中）
    for (int x = 86; x < 90; x++) {
        stage.mapData[5][x] = 1;
    }
    
    // === 敵の配置（空中生物・機械）===
    stage.enemyPositions.push_back({12 * TILE_SIZE, (MAP_HEIGHT - 3) * TILE_SIZE});  // 地上
    stage.enemyPositions.push_back({32 * TILE_SIZE, 11 * TILE_SIZE});              // 低空
    stage.enemyPositions.push_back({49 * TILE_SIZE, 8 * TILE_SIZE});               // 中空
    stage.enemyPositions.push_back({69 * TILE_SIZE, 3 * TILE_SIZE});               // 高空
    stage.enemyPositions.push_back({85 * TILE_SIZE, 4 * TILE_SIZE});               // 城塞
    
    // === アイテム配置（空中の宝物）===
    // セクション1
    stage.itemPositions.push_back({{12 * TILE_SIZE, (MAP_HEIGHT - 6) * TILE_SIZE - 25}, COIN});
    
    // セクション2
    stage.itemPositions.push_back({{24 * TILE_SIZE, 13 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{32 * TILE_SIZE, 11 * TILE_SIZE - 25}, POWER_MUSHROOM});
    stage.itemPositions.push_back({{38 * TILE_SIZE, 9 * TILE_SIZE - 25}, COIN});
    
    // セクション3
    stage.itemPositions.push_back({{43 * TILE_SIZE, 7 * TILE_SIZE - 25}, LIFE_UP});
    stage.itemPositions.push_back({{55 * TILE_SIZE, 6 * TILE_SIZE - 25}, COIN});
    
    // セクション4
    stage.itemPositions.push_back({{63 * TILE_SIZE, 4 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{75 * TILE_SIZE, 5 * TILE_SIZE - 25}, COIN});
    
    // セクション5
    stage.itemPositions.push_back({{86 * TILE_SIZE, 4 * TILE_SIZE - 25}, COIN});
    stage.itemPositions.push_back({{88 * TILE_SIZE, 4 * TILE_SIZE - 25}, POWER_MUSHROOM});
    
    return stage;
}

// 制限時間の更新
void Game::UpdateTimeLimit() {
    if (remainingTime > 0 && !stageCleared) {
        // 60フレーム = 1秒
        if (frameCounter == 0) {  // 1秒ごとに実行
            remainingTime--;
            
            if (remainingTime <= 0) {
                std::cout << "⏰ 時間切れ！" << std::endl;
                PlayerTakeDamage();  // 時間切れでダメージ
            }
        }
    }
}

// ステージクリア条件のチェック
bool Game::CheckStageCleared() {
    if (stageCleared) return true;
    
    // GOAL_COLLECT_ALLの場合は全アイテム収集でクリア
    if (goal && goal->type == GOAL_COLLECT_ALL) {
        for (const auto& item : items) {
            if (item.active && !item.collected) {
                return false;  // まだ未収集のアイテムがある
            }
        }
        return true;  // 全アイテム収集完了
    }
    
    return false;  // その他のゴール種類は直接衝突が必要
}

// === ホロウナイト風システムの実装 ===

// ダッシュ処理
void Game::HandleDash() {
    if (!canDash || dashCooldown > 0) return;
    
    // コントローラー入力を優先
    if (controllerConnected && gameController) {
        if (GetControllerButton(SDL_CONTROLLER_BUTTON_X)) {
            // 現在の向きでダッシュを開始
            StartDash(lastDirection);
        }
    } else {
        // キーボード入力
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_X] || currentKeyStates[SDL_SCANCODE_LSHIFT]) {
            // 現在の向きでダッシュを開始
            StartDash(lastDirection);
        }
    }
}

// ダッシュ開始
void Game::StartDash(int direction) {
    if (soulCount < dashCost) {
        std::cout << "❌ MP不足！ ダッシュに必要なMP: " << dashCost << ", 現在のMP: " << soulCount << std::endl;
        return;
    }
    
    isDashing = true;
    dashTimer = dashDuration;
    canDash = false;
    dashCooldown = 45;  // 0.75秒のクールダウン
    
    // ダッシュの速度を設定
    playerVelX = direction * dashSpeed;
    
    // MP消費
    soulCount -= dashCost;
    
    // ダッシュ中は重力を無効化（短時間）
    if (dashTimer > 10) {
        playerVelY = 0;  // 水平ダッシュ
    }
    
    std::cout << "💨 ダッシュ開始！ 方向=" << direction << ", velX=" << playerVelX << ", MP消費: " << dashCost << std::endl;
}

// ダッシュの更新
void Game::UpdateDash() {
    // ダッシュクールダウンの更新
    if (dashCooldown > 0) {
        dashCooldown--;
        if (dashCooldown <= 0) {
            canDash = true;  // クールダウン終了時にダッシュ回復（空中でも可）
        }
    }
    
    // ダッシュ中の処理
    if (isDashing) {
        dashTimer--;
        
        // ダッシュ終了
        if (dashTimer <= 0) {
            EndDash();
        } else {
            // ダッシュ中の安全な移動処理
            int oldX = playerX;
            float originalVelX = playerVelX;  // 元の速度を保存
            SafeMovePlayerX((int)playerVelX);
            
            // 移動できなかった場合（壁にぶつかった）
            if (playerX == oldX && originalVelX != 0) {
                std::cout << "🔍 ダッシュ移動チェック: oldX=" << oldX << ", newX=" << playerX << ", velX=" << originalVelX << std::endl;
                std::cout << "💥 ダッシュで壁にヒット！ playerX=" << playerX << ", velX=" << originalVelX << std::endl;
                
                // ダッシュ中に壁に当たった場合は強制的に壁接触状態にする
                touchingWall = true;
                std::cout << "🔍 壁接触状態を強制設定: " << (touchingWall ? "接触中" : "離れ") << std::endl;
                
                // 壁登り開始の判定
                if (CanStartWallClimb()) {
                    int direction = (originalVelX > 0) ? 1 : -1;  // ダッシュ方向から壁の方向を判定
                    StartWallClimb(direction);
                    EndDash();  // ダッシュ終了
                    std::cout << "🧗 ダッシュから壁登りに移行！" << std::endl;
                } else {
                    std::cout << "❌ 壁登り開始条件を満たしていません" << std::endl;
                    EndDash();  // 通常のダッシュ終了
                }
            }
        }
    }
}

// ダッシュ終了
void Game::EndDash() {
    isDashing = false;
    playerVelX *= 0.3f;  // 慣性を少し残す
    dashTimer = 0;
}

// 攻撃処理（近接攻撃）
void Game::HandleAttack() {
    if (isAttacking || attackCooldown > 0) return;
    
    StartAttack();
}

// 攻撃開始（近接攻撃）
void Game::StartAttack() {
    isAttacking = true;
    attackTimer = attackDuration;
    attackCooldown = 25;  // 約0.4秒の攻撃間隔
    
    // 攻撃範囲の設定（プレイヤーの向いている方向）
    attackHitbox = {
        playerX + (lastDirection > 0 ? playerRect.w : -40),
        playerY - 5,
        40, 50
    };
    
    // 攻撃エフェクト
    SpawnParticleBurst(attackHitbox.x + attackHitbox.w/2, attackHitbox.y + attackHitbox.h/2, PARTICLE_SPARK, 5);
    StartScreenShake(3, 6);
    
    // プレイヤーの光エフェクト
    playerGlowIntensity = 1.3f;
    
    std::cout << "⚔️ 近接攻撃！" << std::endl;
}

// 攻撃の更新（近接攻撃システム）
void Game::UpdateAttack() {
    // 攻撃クールダウンの更新
    if (attackCooldown > 0) {
        attackCooldown--;
    }
    
    // 攻撃中の処理
    if (isAttacking) {
        attackTimer--;
        
        // 攻撃終了
        if (attackTimer <= 0) {
            EndAttack();
        } else {
            // 敵との攻撃判定
            for (auto& enemy : enemies) {
                if (enemy.active && CheckAttackHit(enemy)) {
                    // 敵にダメージ
                    enemy.TakeDamage(1);
                    
                    // 敵が倒された場合
                    if (!enemy.active) {
                        // 敵死亡エフェクト
                        CreateEnemyDeathEffect(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2);
                        
                        // パーティクル効果
                        SpawnParticleBurst(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, PARTICLE_EXPLOSION, 10);
                        
                        // 魂とスコア獲得（通常攻撃はMP2固定）
                        int soulGain = 2;  // 通常攻撃でMP2固定
                        int scoreGain = 100;
                        
                        switch (enemy.type) {
                            case ENEMY_GOOMBA:
                                scoreGain = 100;
                                break;
                            case ENEMY_SHOOTER:
                                scoreGain = 300;
                                break;
                            case ENEMY_JUMPER:
                                scoreGain = 200;
                                break;
                            case ENEMY_CHASER:
                                scoreGain = 400;
                                break;
                            case ENEMY_FLYING:
                                scoreGain = 250;
                                break;
                        }
                        
                        CollectSoul(soulGain);
                        score += scoreGain;
                        
                        // 画面シェイク
                        StartScreenShake(6, 10);
                        
                        // プレイヤーの光エフェクト
                        playerGlowIntensity = 1.2f;
                        
                        std::cout << "⚔️ 敵を撃破！ +" << scoreGain << "点 +" << soulGain << "MP ✨" << std::endl;
                        DisplayGameStatus();
                    } else {
                        // 敵が生きている場合（ダメージのみ）
                        SpawnParticleBurst(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, PARTICLE_SPARK, 6);
                        StartScreenShake(3, 5);
                        std::cout << "💥 敵にダメージ！ 残りHP: " << enemy.health << std::endl;
                    }
                }
            }
        }
    }
}

// 攻撃終了
void Game::EndAttack() {
    isAttacking = false;
    attackTimer = 0;
}



// 攻撃ヒット判定
bool Game::CheckAttackHit(const Enemy& enemy) {
    return SDL_HasIntersection(&attackHitbox, &enemy.rect);
}

// ウォールジャンプ処理
void Game::HandleWallJump() {
    if (!touchingWall || !canWallJump || wallJumpCooldown > 0) return;
    
    // 壁から離れる方向にジャンプ（より強い力で）
    playerVelY = -15.0f;  // より高くジャンプ
    playerVelX = -lastDirection * 8.0f;  // より強い水平速度
    
    // 強制的に壁から離れる位置調整（安全な移動）
    if (lastDirection == 1) {  // 右の壁にいる場合（左向きジャンプ）
        SafeMovePlayerX(-8);  // 左に安全移動
    } else {  // 左の壁にいる場合（右向きジャンプ）
        SafeMovePlayerX(8);   // 右に安全移動
    }
    
    isOnGround = false;
    isJumping = true;
    
    // ダッシュとエアダッシュ回復
    canDash = true;
    ResetAirDash();
    
    // ウォールジャンプクールダウンを設定（15フレーム）
    wallJumpCooldown = 15;
    
    // 壁接触状態をリセット
    touchingWall = false;
    canWallJump = false;
    
    std::cout << "🧗 ウォールジャンプ！ 方向: " << (lastDirection == 1 ? "左" : "右") << std::endl;
}

// 壁との衝突判定
bool Game::CheckWallCollision() {
    // 左の壁（プレイヤーの少し左をチェック）
    int leftCheck = playerX - 3;
    if (leftCheck >= 0 && CheckHorizontalCollision(leftCheck, playerY)) {
        lastDirection = 1;  // 右向きに設定（左の壁なので右向きジャンプ）
        return true;
    }
    
    // 右の壁（プレイヤーの少し右をチェック）
    int rightCheck = playerX + playerRect.w + 3;
    if (rightCheck < (MAP_WIDTH * TILE_SIZE) && CheckHorizontalCollision(rightCheck, playerY)) {
        lastDirection = -1;  // 左向きに設定（右の壁なので左向きジャンプ）
        return true;
    }
    
    return false;
}

// 壁接触状態の更新
void Game::UpdateWallTouch() {
    // ウォールジャンプクールダウンを減少
    if (wallJumpCooldown > 0) {
        wallJumpCooldown--;
        // クールダウン中は壁判定を無効化
        touchingWall = false;
        canWallJump = false;
        return;
    }
    
    bool previousTouchingWall = touchingWall;
    touchingWall = CheckWallCollision();
    
    // デバッグ: 壁接触状態の変化をログ出力
    if (touchingWall != previousTouchingWall) {
        std::cout << "🧱 壁接触状態変化: " << (touchingWall ? "接触開始" : "接触終了") 
                  << " (地面=" << (isOnGround ? "接触" : "空中") << ")" << std::endl;
    }
    
    // 壁に触れていて、空中にいる場合はウォールジャンプ可能
    canWallJump = touchingWall && !isOnGround;
}

// === 壁登りシステムの実装 ===

// 壁登り開始可能かチェック
bool Game::CanStartWallClimb() {
    // デバッグ出力で各条件をチェック
    std::cout << "🔍 壁登り判定: 地面=" << (isOnGround ? "接触" : "空中") 
              << ", 壁=" << (touchingWall ? "接触" : "離れ") 
              << ", スタミナ=" << wallClimbStamina 
              << ", 壁登り中=" << (isWallClimbing ? "Yes" : "No") << std::endl;
    
    // 壁に接触していて、スタミナがある場合（地面からでも可能）
    bool canStart = touchingWall && wallClimbStamina > 0 && !isWallClimbing;
    std::cout << "🔍 最終判定: " << (canStart ? "壁登り可能" : "壁登り不可") << std::endl;
    return canStart;
}

// 壁登り開始
void Game::StartWallClimb(int direction) {
    if (!CanStartWallClimb()) return;
    
    isWallClimbing = true;
    wallClimbDirection = direction;
    wallClimbTimer = 0;
    playerVelY = 0;  // 落下を停止
    playerVelX = 0;  // 水平移動を停止
    
    // 地面から開始する場合は少し浮上させる
    if (isOnGround) {
        playerY -= 5;  // 5ピクセル上に移動
        isOnGround = false;  // 地面接触を解除
        std::cout << "🚀 地面から壁登りに移行！" << std::endl;
    }
    
    std::cout << "🧗 壁登り開始！ 方向: " << (direction == 1 ? "右壁（右キーで登る）" : "左壁（左キーで登る）") << std::endl;
}

// 壁登り更新
void Game::UpdateWallClimb() {
    if (!isWallClimbing) return;
    
    wallClimbTimer++;
    
    // 壁接触チェック
    if (!CheckWallCollision()) {
        EndWallClimb();
        return;
    }
    
    // スタミナ消費（毎フレーム少しずつ）
    if (wallClimbTimer % 10 == 0) {  // 10フレームごと
        wallClimbStamina--;
    }
    
    // スタミナ切れ
    if (wallClimbStamina <= 0) {
        EndWallClimb();
        return;
    }
    
    // デフォルトで壁滑り（ゆっくり落下）
    playerVelY = wallSlideSpeed;
}

// 壁登り終了
void Game::EndWallClimb() {
    isWallClimbing = false;
    wallClimbDirection = 0;
    wallClimbTimer = 0;
    
    std::cout << "🧗 壁登り終了" << std::endl;
}

// 壁登り中の入力処理
void Game::HandleWallClimbInput() {
    if (!isWallClimbing) return;
    
    const Uint8* currentKeyStates = SDL_GetKeyboardState(nullptr);
    
    // 壁に押し付ける方向キーで登る（直感的操作）
    bool pressingIntoWall = false;
    if (wallClimbDirection == 1) {  // 右壁の場合
        if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
            pressingIntoWall = true;  // 右キーで右壁を登る
        }
    } else {  // 左壁の場合
        if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
            pressingIntoWall = true;  // 左キーで左壁を登る
        }
    }
    
    // 壁に押し付けながら登る
    if (pressingIntoWall && wallClimbStamina > 0) {
        playerVelY = wallClimbSpeed;  // 上向きに移動
        // スタミナを多く消費
        if (wallClimbTimer % 5 == 0) {
            wallClimbStamina--;
        }
    }
    // S/下キー: 壁を滑り降りる（高速）
    else if (currentKeyStates[SDL_SCANCODE_S] || currentKeyStates[SDL_SCANCODE_DOWN]) {
        playerVelY = wallSlideSpeed * 2;  // 通常の2倍の速度で下降
    }
    
    // 壁から離れる方向キー（反対方向）
    bool pressingAway = false;
    if (wallClimbDirection == 1) {  // 右壁の場合
        if (currentKeyStates[SDL_SCANCODE_A] || currentKeyStates[SDL_SCANCODE_LEFT]) {
            pressingAway = true;  // 左キーで右壁から離れる
        }
    } else {  // 左壁の場合
        if (currentKeyStates[SDL_SCANCODE_D] || currentKeyStates[SDL_SCANCODE_RIGHT]) {
            pressingAway = true;  // 右キーで左壁から離れる
        }
    }
    
    // 壁から離れる
    if (pressingAway) {
        EndWallClimb();
        // 壁から離れる方向に軽く押し出す
        playerVelX = -wallClimbDirection * 3.0f;
        canWallJump = true;  // ウォールジャンプ可能にする
    }
    
    // ジャンプキー: ウォールジャンプ
    if (currentKeyStates[SDL_SCANCODE_SPACE]) {
        EndWallClimb();
        HandleWallJump();
    }
}

// ソウルシステムの更新
void Game::UpdateSoulSystem() {
    // 魂の上限チェック
    if (soulCount > maxSoul) {
        soulCount = maxSoul;
    }
}

// 魂を獲得
void Game::CollectSoul(int amount) {
    soulCount += amount;
    if (soulCount > maxSoul) {
        soulCount = maxSoul;
    }
    
    // 魂収集エフェクトを生成
    CreateSoulCollectEffect(playerX + playerRect.w/2, playerY + playerRect.h/2);
    
    std::cout << "✨ 魂 +" << amount << " (合計: " << soulCount << ")" << std::endl;
}

// 回復を使用
void Game::UseHeal() {
    if (soulCount >= 33 && playerHealth < maxHealth) {
        playerHealth++;
        soulCount -= 33;
        
        std::cout << "💚 回復！ HP: " << playerHealth << "/" << maxHealth << std::endl;
        std::cout << "✨ 魂 -33 (残り: " << soulCount << ")" << std::endl;
    }
}

// プレイヤー移動の統合更新
void Game::UpdatePlayerMovement() {
    // ダッシュ中でない場合の水平速度減衰
    if (!isDashing && abs(playerVelX) > 0.1f) {
        playerVelX *= 0.8f;  // 摩擦
        
        // 安全な移動を実行
        SafeMovePlayerX((int)playerVelX);
    }
}

// プレイヤーの向きを更新
void Game::UpdatePlayerDirection() {
    // コントローラー入力から向きを判定（優先）
    if (controllerConnected && gameController) {
        float leftX = GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);
        if (leftX < -0.3f) {
            lastDirection = -1;  // 左向き
        } else if (leftX > 0.3f) {
            lastDirection = 1;   // 右向き
        }
        return;  // コントローラーが接続されている場合はキーボード処理をスキップ
    }
    
            // キーボード入力から向きを判定
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
        lastDirection = -1;  // 左向き
    } else if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
        lastDirection = 1;   // 右向き
    }
}

// プレイヤー状態をリセット
void Game::ResetPlayerState() {
    // ダッシュ状態のリセット
    isDashing = false;
    canDash = true;
    dashCooldown = 0;
    dashTimer = 0;
    playerVelX = 0;
    
    // エアダッシュ状態のリセット
    ResetAirDash();
    lastAirDashDirection = 1;
    
    // ダブルジャンプ状態のリセット
    ResetAirJump();
    
    // 攻撃状態のリセット
    isAttacking = false;
    attackCooldown = 0;
    attackTimer = 0;
    
    // 光線攻撃状態のリセット
    isChargingBeam = false;
    beamChargeTime = 0;
    isFiringBeam = false;
    beamTimer = 0;
    
    // 体力とソウルのリセット
    playerHealth = maxHealth;
    soulCount = 0;
    
    // 壁関連のリセット
    touchingWall = false;
    canWallJump = false;
    wallJumpCooldown = 0;
    
    lastDirection = 1;  // 右向きにリセット
}

// 安全な水平移動（衝突判定付き）
void Game::SafeMovePlayerX(int deltaX) {
    if (deltaX == 0) return;
    
    // 高速移動の場合は段階的に移動して壁すり抜けを防ぐ
    int step = (deltaX > 0) ? 1 : -1;
    int remaining = abs(deltaX);
    
    for (int i = 0; i < remaining; i++) {
        int newPlayerX = playerX + step;
        
        // 境界チェック
        if (newPlayerX < 0 || newPlayerX > (MAP_WIDTH * TILE_SIZE) - playerRect.w) {
            playerVelX = 0;
            break;
        }
        
        // 衝突判定を実行
        if (!CheckHorizontalCollision(newPlayerX, playerY)) {
            playerX = newPlayerX;
        } else {
            // 衝突した場合は移動停止と速度リセット
            playerVelX = 0;
            break;
        }
    }
}

// === エアダッシュシステムの実装 ===

// エアダッシュ処理
void Game::HandleAirDash() {
    if (!CanPerformAirDash()) return;
    
    // 方向決定（入力がない場合は最後の方向）
    int direction = lastDirection;
    
    // 8方向ダッシュ対応
    float dashVelX = 0, dashVelY = 0;
    
    // コントローラー入力を優先
    if (controllerConnected && gameController) {
        float leftX = GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);
        float leftY = GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);
        
        if (leftX < -0.3f) {
            direction = -1;
            dashVelX = -airDashSpeed;
        }
        if (leftX > 0.3f) {
            direction = 1;
            dashVelX = airDashSpeed;
        }
        if (leftY < -0.3f) {  // スティック上
            dashVelY = -airDashSpeed * 0.8f;
        }
        if (leftY > 0.3f) {   // スティック下
            dashVelY = airDashSpeed * 0.6f;
        }
    } else {
        // キーボード入力
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        
        if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
            direction = -1;
            dashVelX = -airDashSpeed;
        }
        if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
            direction = 1;
            dashVelX = airDashSpeed;
        }
        if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) {
            dashVelY = -airDashSpeed * 0.8f;  // 上方向は少し弱く
        }
        if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) {
            dashVelY = airDashSpeed * 0.6f;   // 下方向
        }
    }
    
    // 入力がない場合は水平ダッシュ
    if (dashVelX == 0 && dashVelY == 0) {
        dashVelX = direction * airDashSpeed;
    }
    
    StartAirDash(direction);
    
    // エアダッシュの速度を適用
    playerVelX = dashVelX;
    if (dashVelY != 0) {
        playerVelY = dashVelY;  // 方向ダッシュの場合は重力を一時的にオーバーライド
    }
    
    std::cout << "🌪️ エアダッシュ！ 残り: " << (maxAirDash - airDashCount) << "回" << std::endl;
}

// エアダッシュ開始
void Game::StartAirDash(int direction) {
    airDashCount++;
    lastAirDashDirection = direction;
    isDashing = true;
    dashTimer = 12;  // エアダッシュは短め
    
    // 少しの無敵時間
    if (invincibilityTime <= 0) {
        invincibilityTime = 8;  // 短い無敵時間
    }
}

// エアダッシュ回数リセット
void Game::ResetAirDash() {
    airDashCount = 0;
    canAirDash = true;
}

// エアダッシュ可能判定
bool Game::CanPerformAirDash() {
    return !isOnGround && canAirDash && airDashCount < maxAirDash && !isDashing;
}

// === ダブルジャンプシステムの実装 ===

// 空中ジャンプ処理
void Game::HandleAirJump() {
    // 空中ジャンプ可能判定
    if (CanPerformAirJump()) {
        PerformAirJump();
    }
}

// 空中ジャンプ実行
void Game::PerformAirJump() {
    // 空中ジャンプを実行
    playerVelY = -12.75f;  // 通常ジャンプ(-15.0f)の85%の力
    airJumpCount++;
    
    // エフェクト生成
    SpawnParticleBurst(playerX + playerRect.w/2, playerY + playerRect.h, PARTICLE_SPARK, 8);
    StartScreenShake(3, 10);
    
#ifdef SOUND_ENABLED
    if (jumpSound) {
        PlaySound(jumpSound);
    }
#endif
    
    std::cout << "🦘 空中ジャンプ！ (残り" << (maxAirJump - airJumpCount) << "回)" << std::endl;
}

// 空中ジャンプリセット
void Game::ResetAirJump() {
    airJumpCount = 0;
    canAirJump = true;
}

// 空中ジャンプ可能チェック
bool Game::CanPerformAirJump() {
    return !isOnGround && canAirJump && airJumpCount < maxAirJump && !isDashing;
}







// === ボス戦システムの実装 ===

// ボス初期化
void Game::InitializeBoss() {
    if (boss) delete boss;
    
    boss = new Boss(600, 200);  // 画面右側に出現
    isBossFight = true;
    bossDefeated = false;
    bossIntroComplete = false;
    bossIntroTimer = 180;  // 3秒の登場演出
    
    // ボス戦用のBGM変更やエフェクト
    std::cout << "🐲 強大なボスが現れた！" << std::endl;
}

// ボス更新
void Game::UpdateBoss() {
    if (!isBossFight) return;
    
    // ボス登場演出
    if (!bossIntroComplete) {
        bossIntroTimer--;
        if (bossIntroTimer <= 0) {
            bossIntroComplete = true;
            std::cout << "⚔️ ボス戦開始！" << std::endl;
        }
        return;
    }
    
    if (boss && boss->active) {
        // プレイヤーの位置をボスに渡す（AI用）
        boss->Update();
        
        // ボスの攻撃パターン実行
        if (boss->isAttacking) {
            switch(boss->currentAttack) {
                case Boss::PROJECTILE_ATTACK:
                    // 弾幕攻撃：複数の弾丸を発射
                    if (boss->attackTimer % 20 == 0) {  // 20フレームごと
                        for (int i = 0; i < 3; i++) {
                            float angle = -1.5f + i * 1.5f;  // -1.5, 0, 1.5ラジアン
                            SpawnBossProjectile(boss->x + 40, boss->y + 50, 
                                              cos(angle) * 4, sin(angle) * 4);
                        }
                    }
                    break;
                    
                case Boss::CHARGE_ATTACK:
                    // 突進攻撃：プレイヤー方向に高速移動
                    if (boss->attackTimer > 60) {  // 準備時間
                        if (playerX < boss->x) {
                            boss->velX = -8.0f;
                        } else {
                            boss->velX = 8.0f;
                        }
                    }
                    break;
                    
                case Boss::SLAM_ATTACK:
                    // 地面叩きつけ：強力な衝撃波
                    if (boss->attackTimer == 30) {  // タイミングで衝撃波
                        for (int i = 0; i < 8; i++) {
                            float angle = i * (M_PI / 4);  // 8方向
                            SpawnBossProjectile(boss->x + 40, boss->y + 90,
                                              cos(angle) * 3, sin(angle) * 3);
                        }
                    }
                    break;
                    
                case Boss::TELEPORT_ATTACK:
                    // テレポート攻撃：瞬間移動
                    if (boss->attackTimer == 60) {
                        boss->x = 100 + (rand() % 600);  // ランダム位置
                        boss->y = 150 + (rand() % 100);
                        std::cout << "💫 ボスがテレポートした！" << std::endl;
                    }
                    break;
            }
            
            // 攻撃終了チェック
            if (boss->attackTimer <= 0) {
                boss->isAttacking = false;
                boss->velX *= 0.5f;  // 速度減衰
            }
        }
        
        // プレイヤーとボスの衝突判定
        if (CheckPlayerBossCollision()) {
            PlayerTakeDamage();
        }
        
        // プレイヤーの攻撃がボスに当たったかチェック
        if (CheckAttackBossHit()) {
            boss->TakeDamage(25);  // 1回の攻撃で25ダメージ
            
            // 魂獲得
            CollectSoul(3);
        }
        
        // ボス撃破チェック
        if (boss->IsDefeated()) {
            EndBossFight();
        }
    }
    
    // ボス弾丸の更新
    UpdateBossProjectiles();
}

// ボス描画
void Game::RenderBoss() {
    if (!isBossFight || !boss || !boss->active) return;
    
    // ボス登場演出中
    if (!bossIntroComplete) {
        // フラッシュエフェクト（美化版）
        if ((bossIntroTimer / 10) % 2 == 0) {
            SetRenderColorWithAlpha(ColorPalette::DAMAGE_RED, 0.9f);
        } else {
            SetRenderColorWithAlpha(ColorPalette::UI_ACCENT, 0.9f);
        }
        SDL_RenderFillRect(renderer, &boss->rect);
        
        // 登場演出の光エフェクト
        int centerX = boss->x + boss->rect.w / 2;
        int centerY = boss->y + boss->rect.h / 2;
        DrawGlowEffect(centerX, centerY, 50, ColorPalette::DAMAGE_RED, 1.5f);
        return;
    }
    
    // ボスの影
    if (enableShadows) {
        SDL_Rect shadowRect = {
            boss->x + 5,
            boss->y + boss->rect.h - 10,
            boss->rect.w,
            15
        };
        SetRenderColorWithAlpha(ColorPalette::TILE_SHADOW, 0.7f);
        SDL_RenderFillRect(renderer, &shadowRect);
    }
    
    // ボス本体の描画
    if (boss->isStunned && (boss->stunTimer / 5) % 2 == 0) {
        // スタン時は白く点滅
        SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
    } else {
        // 通常時はダークレッド
        SetRenderColorWithAlpha(ColorPalette::DAMAGE_RED, 0.9f);
    }
    SDL_RenderFillRect(renderer, &boss->rect);
    
    // ボスのハイライト
    SDL_Rect highlightRect = {
        boss->x + 5,
        boss->y + 5,
        boss->rect.w - 15,
        boss->rect.h / 3
    };
    SetRenderColorWithAlpha(ColorPalette::UI_SECONDARY, 0.6f);
    SDL_RenderFillRect(renderer, &highlightRect);
    
    // ボスの縁取り
    SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
    SDL_RenderDrawRect(renderer, &boss->rect);
    
    // ボスの不気味な光エフェクト
    int centerX = boss->x + boss->rect.w / 2;
    int centerY = boss->y + boss->rect.h / 2;
    DrawGlowEffect(centerX, centerY, 30, ColorPalette::DAMAGE_RED, 0.8f);
    
    // ボスHPバーの描画（美化版）
    int barX = 50, barY = 50;
    int barWidth = 300, barHeight = 20;
    
    // HPバー背景
    SDL_Rect hpBarBack = {barX, barY, barWidth, barHeight};
    SetRenderColorWithAlpha(ColorPalette::BACKGROUND_DARK, 0.9f);
    SDL_RenderFillRect(renderer, &hpBarBack);
    
    // HPバー（グラデーション）
    int fillWidth = (barWidth * boss->health) / boss->maxHealth;
    SDL_Rect hpBarFront = {barX, barY, fillWidth, barHeight};
    DrawGradientRect(hpBarFront, ColorPalette::DAMAGE_RED, ColorPalette::HEALTH_GREEN);
    
    // HPバーの縁取り
    SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
    SDL_RenderDrawRect(renderer, &hpBarBack);
    
    // ボス名表示
    std::string bossName = "Dark Guardian";
    RenderText(bossName, barX, barY - 25, ColorPalette::UI_PRIMARY);
}

// ボス戦開始
void Game::StartBossFight() {
    InitializeBoss();
}

// ボス戦終了
void Game::EndBossFight() {
    isBossFight = false;
    bossDefeated = true;
    
    // 大量のスコア獲得
    score += 5000;
    
    // 魂も大量獲得
    CollectSoul(50);
    
    std::cout << "🏆 ボス撃破！ +5000点！" << std::endl;
    std::cout << "✨ 魂を50個獲得！" << std::endl;
    
    // ステージクリア処理
    HandleStageClear();
}

// プレイヤーとボスの衝突判定
bool Game::CheckPlayerBossCollision() {
    if (!boss || !boss->active || invincibilityTime > 0) return false;
    
    return SDL_HasIntersection(&playerRect, &boss->rect);
}

// 攻撃がボスに当たったかチェック
bool Game::CheckAttackBossHit() {
    if (!boss || !boss->active || !isAttacking) return false;
    
    return SDL_HasIntersection(&attackHitbox, &boss->rect);
}

// ボス弾丸の更新
void Game::UpdateBossProjectiles() {
    for (auto& projectile : bossProjectiles) {
        if (projectile.active) {
            projectile.Update();
            
            // プレイヤーとの衝突判定
            if (CheckPlayerProjectileCollision(projectile)) {
                projectile.active = false;
                PlayerTakeDamage();
            }
        }
    }
    
    // 非アクティブな弾丸を削除
    bossProjectiles.erase(
        std::remove_if(bossProjectiles.begin(), bossProjectiles.end(),
                       [](const BossProjectile& p) { return !p.active; }),
        bossProjectiles.end()
    );
}

// ボス弾丸の描画
void Game::RenderBossProjectiles() {
    SDL_SetRenderDrawColor(renderer, 255, 200, 100, 255);  // オレンジ色
    
    for (const auto& projectile : bossProjectiles) {
        if (projectile.active) {
            SDL_RenderFillRect(renderer, &projectile.rect);
        }
    }
}

// プレイヤーと弾丸の衝突判定
bool Game::CheckPlayerProjectileCollision(const BossProjectile& projectile) {
    if (invincibilityTime > 0) return false;
    
    return SDL_HasIntersection(&playerRect, &projectile.rect);
}

// ボス弾丸生成
void Game::SpawnBossProjectile(float x, float y, float velX, float velY) {
    bossProjectiles.emplace_back(x, y, velX, velY);
}

// ボスステージの作成
StageData Game::CreateBossStage() {
    StageData stage;
    stage.stageNumber = 4;
    stage.stageName = "Boss Stage - Final Battle";
    stage.goalType = GOAL_FLAG;  // ボス撃破でクリア
    stage.goalX = 80 * TILE_SIZE;  // 横スクロール対応（ボス戦エリアの右端）
    stage.goalY = 16 * TILE_SIZE;
    stage.playerStartX = 100;
    stage.playerStartY = 300;
    stage.timeLimit = 0;  // 時間制限なし
    stage.bgmFile = "boss_battle.ogg";
    
    // シンプルなボス戦用ステージ
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (y == MAP_HEIGHT - 1) {
                stage.mapData[y][x] = 1;  // 地面
            } else if (x == 0 || x == MAP_WIDTH - 1) {
                stage.mapData[y][x] = 1;  // 壁
            } else {
                stage.mapData[y][x] = 0;  // 空
            }
        }
    }
    
    // プラットフォームを少し追加（ダッシュ・ウォールジャンプ練習用）
    for (int x = 8; x < 12; x++) {
        stage.mapData[15][x] = 1;
    }
    for (int x = 13; x < 17; x++) {
        stage.mapData[15][x] = 1;
    }
    
    // 壁ジャンプ用の縦壁
    for (int y = 10; y < 18; y++) {
        stage.mapData[y][5] = 1;
        stage.mapData[y][19] = 1;
    }
    
    // 敵はなし（ボスのみ）
    stage.enemyPositions.clear();
    
    // 回復アイテムをいくつか配置
    stage.itemPositions.push_back({{3 * TILE_SIZE, 15 * TILE_SIZE}, POWER_MUSHROOM});
    stage.itemPositions.push_back({{21 * TILE_SIZE, 15 * TILE_SIZE}, POWER_MUSHROOM});
    stage.itemPositions.push_back({{9 * TILE_SIZE, 13 * TILE_SIZE}, LIFE_UP});
    stage.itemPositions.push_back({{15 * TILE_SIZE, 13 * TILE_SIZE}, LIFE_UP});
    
    std::cout << "🐲 ボスステージ作成完了" << std::endl;
    
    return stage;
}

// === エフェクトシステムの実装 ===

// パーティクルの更新
void Game::UpdateParticles() {
    // パーティクルの更新
    for (auto& particle : particles) {
        if (particle.active) {
            particle.Update();
        }
    }
    
    // 非アクティブなパーティクルを削除
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
                       [](const Particle& p) { return !p.active; }),
        particles.end()
    );
    
    // パーティクル数の上限チェック
    if ((int)particles.size() > particleLimit) {
        particles.erase(particles.begin(), particles.begin() + ((int)particles.size() - particleLimit));
    }
}

// パーティクルの描画
void Game::RenderParticles() {
    for (auto& particle : particles) {
        if (particle.active) {
            particle.Render(renderer);
        }
    }
}

// パーティクルを生成
void Game::SpawnParticle(float x, float y, float velX, float velY, ParticleType type, float life) {
    particles.emplace_back(x, y, velX, velY, type, life);
}

// パーティクル大量生成
void Game::SpawnParticleBurst(float x, float y, ParticleType type, int count) {
    for (int i = 0; i < count; i++) {
        float angle = (rand() % 360) * M_PI / 180.0f;  // ランダムな角度
        float speed = 1.0f + (rand() % 4);  // 1-5のランダムな速度
        
        float velX = cos(angle) * speed;
        float velY = sin(angle) * speed;
        
        float life = 30 + (rand() % 60);  // 30-90フレームのランダムな生存時間
        
        SpawnParticle(x, y, velX, velY, type, life);
    }
}

// ダッシュ軌跡生成
void Game::CreateDashTrail() {
    if (isDashing) {
        // プレイヤーの位置にダッシュ軌跡を生成
        SpawnParticle(playerX + playerRect.w/2, playerY + playerRect.h/2, 
                     0, 0, PARTICLE_DASH_TRAIL, 20);
    }
}

// 攻撃エフェクト生成
void Game::CreateAttackEffect() {
    if (isAttacking && attackTimer > attackDuration - 5) {  // 攻撃開始直後
        // 攻撃範囲に火花エフェクト
        float effectX = attackHitbox.x + attackHitbox.w/2;
        float effectY = attackHitbox.y + attackHitbox.h/2;
        
        SpawnParticleBurst(effectX, effectY, PARTICLE_SPARK, 8);
        
        // 攻撃時の画面シェイク
        StartScreenShake(3, 8);
    }
}

// 敵死亡エフェクト
void Game::CreateEnemyDeathEffect(float x, float y) {
    // 爆発エフェクト
    SpawnParticleBurst(x, y, PARTICLE_EXPLOSION, 15);
    
    // 煙エフェクト
    SpawnParticleBurst(x, y, PARTICLE_SMOKE, 8);
    
    // 死亡時の画面シェイク
    StartScreenShake(5, 12);
}

// 魂収集エフェクト
void Game::CreateSoulCollectEffect(float x, float y) {
    // 魂パーティクル
    SpawnParticleBurst(x, y, PARTICLE_SOUL, 5);
    
    // 軽い画面シェイク
    StartScreenShake(2, 6);
}

// 画面シェイクの更新
void Game::UpdateScreenShake() {
    if (screenShakeDuration > 0) {
        screenShakeDuration--;
        
        // ランダムなオフセットを生成
        float intensity = screenShakeIntensity * (screenShakeDuration / 30.0f);  // 徐々に弱くなる
        shakeOffsetX = ((rand() % 200) - 100) / 100.0f * intensity;
        shakeOffsetY = ((rand() % 200) - 100) / 100.0f * intensity;
    } else {
        shakeOffsetX = 0;
        shakeOffsetY = 0;
    }
}

// 画面シェイク開始
void Game::StartScreenShake(int intensity, int duration) {
    screenShakeIntensity = intensity;
    screenShakeDuration = duration;
}

// 画面シェイクの適用
void Game::ApplyScreenShake() {
    // 現在は描画時に手動でオフセットを適用
    // より高度な実装では、カメラシステムを使用
}

// === ビジュアルシステムの実装 ===

// ビジュアルエフェクトの更新
void Game::UpdateVisualEffects() {
    // グラデーションのアニメーション
    gradientOffset += 0.5f;
    if (gradientOffset > 360.0f) {
        gradientOffset = 0.0f;
    }
    
    // プレイヤーの光エフェクトアニメーション
    playerGlowTimer += 0.08f;
    if (playerGlowTimer > 2 * M_PI) {
        playerGlowTimer = 0.0f;
    }
    
    // 状態に応じた光の強度調整
    if (isDashing) {
        playerGlowIntensity = 1.5f + sin(playerGlowTimer * 3) * 0.3f;
    } else if (isAttacking) {
        playerGlowIntensity = 1.2f + sin(playerGlowTimer * 4) * 0.2f;
    } else {
        playerGlowIntensity = 0.8f + sin(playerGlowTimer) * 0.2f;
    }
}

// グラデーション背景の描画
void Game::RenderGradientBackground() {
    if (!enableGradientBackground) return;
    
    // 画面全体のグラデーション
    SDL_Rect fullScreen = {0, 0, 800, 600};
    
    // 時間によって変化する背景色
    float timeOffset = sin(gradientOffset * 0.01f) * 0.1f;
    
    SDL_Color topColor = {
        (Uint8)(ColorPalette::BACKGROUND_DARK.r + timeOffset * 10),
        (Uint8)(ColorPalette::BACKGROUND_DARK.g + timeOffset * 15),
        (Uint8)(ColorPalette::BACKGROUND_DARK.b + timeOffset * 20),
        255
    };
    
    SDL_Color bottomColor = {
        (Uint8)(ColorPalette::BACKGROUND_MID.r + timeOffset * 5),
        (Uint8)(ColorPalette::BACKGROUND_MID.g + timeOffset * 10),
        (Uint8)(ColorPalette::BACKGROUND_MID.b + timeOffset * 15),
        255
    };
    
    DrawGradientRect(fullScreen, topColor, bottomColor);
}

// 美化されたプレイヤー描画
void Game::RenderEnhancedPlayer() {
    // カメラオフセットを適用した描画位置を計算
    int screenX = WorldToScreenX(playerX);
    int screenY = WorldToScreenY(playerY);
    
    // 画面外にいる場合は描画しない
    if (screenX + playerRect.w < 0 || screenX > SCREEN_WIDTH || 
        screenY + playerRect.h < 0 || screenY > SCREEN_HEIGHT) {
        return;
    }
    
    // プレイヤーの影を先に描画
    if (enableShadows) {
        SDL_Rect shadowRect = {
            screenX + 2,
            screenY + playerRect.h - 5,
            playerRect.w,
            8
        };
        SetRenderColorWithAlpha(ColorPalette::TILE_SHADOW, 0.5f);
        SDL_RenderFillRect(renderer, &shadowRect);
    }
    
    // プレイヤーの光エフェクト（カメラオフセット適用）
    RenderPlayerGlow();
    
    // プレイヤー本体（立体感のある描画）
    // メイン部分
    SDL_Rect playerScreenRect = {screenX, screenY, playerRect.w, playerRect.h};
    SetRenderColorWithAlpha(ColorPalette::PLAYER_PRIMARY, 1.0f);
    SDL_RenderFillRect(renderer, &playerScreenRect);
    
    // ハイライト部分
    SDL_Rect highlightRect = {
        screenX + 2,
        screenY + 2,
        playerRect.w - 8,
        playerRect.h / 3
    };
    SetRenderColorWithAlpha(ColorPalette::PLAYER_SECONDARY, 0.8f);
    SDL_RenderFillRect(renderer, &highlightRect);
    
    // アウトライン
    SetRenderColorWithAlpha(ColorPalette::UI_ACCENT, 0.9f);
    SDL_RenderDrawRect(renderer, &playerScreenRect);
    
    // 無敵時間中の点滅効果
    if (invincibilityTime > 0 && (invincibilityTime / 5) % 2 == 0) {
        SetRenderColorWithAlpha(ColorPalette::DAMAGE_RED, 0.5f);
        SDL_RenderFillRect(renderer, &playerScreenRect);
    }
}

// プレイヤーの光エフェクト
void Game::RenderPlayerGlow() {
    int centerX = WorldToScreenX(playerX) + playerRect.w / 2;
    int centerY = WorldToScreenY(playerY) + playerRect.h / 2;
    
    // 複数の光の層を重ねて描画
    for (int layer = 0; layer < 3; layer++) {
        int radius = 15 + layer * 8;
        float intensity = playerGlowIntensity * (0.8f - layer * 0.2f);
        
        DrawGlowEffect(centerX, centerY, radius, ColorPalette::PLAYER_GLOW, intensity);
    }
}

// 美化されたタイル描画（カメラオフセット対応）
void Game::RenderEnhancedTiles() {
    // 画面に表示される範囲のタイルのみを計算（最適化）
    int startTileX = (int)cameraX / TILE_SIZE;
    int endTileX = ((int)cameraX + SCREEN_WIDTH) / TILE_SIZE + 1;
    int startTileY = (int)cameraY / TILE_SIZE;
    int endTileY = ((int)cameraY + SCREEN_HEIGHT) / TILE_SIZE + 1;
    
    // 範囲を制限
    if (startTileX < 0) startTileX = 0;
    if (endTileX > MAP_WIDTH) endTileX = MAP_WIDTH;
    if (startTileY < 0) startTileY = 0;
    if (endTileY > MAP_HEIGHT) endTileY = MAP_HEIGHT;
    
    for (int y = startTileY; y < endTileY; y++) {
        for (int x = startTileX; x < endTileX; x++) {
            if (map[y][x] == 1) {  // ブロックタイル
                // 隣接タイルの情報を取得
                bool hasTop = (y > 0 && map[y-1][x] == 1);
                bool hasBottom = (y < MAP_HEIGHT-1 && map[y+1][x] == 1);
                bool hasLeft = (x > 0 && map[y][x-1] == 1);
                bool hasRight = (x < MAP_WIDTH-1 && map[y][x+1] == 1);
                
                // ワールド座標からスクリーン座標に変換して描画
                int screenX = WorldToScreenX(x * TILE_SIZE);
                int screenY = WorldToScreenY(y * TILE_SIZE);
                RenderTile(screenX, screenY, hasTop, hasBottom, hasLeft, hasRight);
            }
        }
    }
}

// 個別タイルの描画
void Game::RenderTile(int x, int y, bool hasTop, bool hasBottom, bool hasLeft, bool hasRight) {
    SDL_Rect tileRect = {x, y, TILE_SIZE, TILE_SIZE};
    
    // 影の描画
    if (enableShadows && !hasBottom) {
        SDL_Rect shadowRect = {x, y + TILE_SIZE, TILE_SIZE, 4};
        SetRenderColorWithAlpha(ColorPalette::TILE_SHADOW, 0.6f);
        SDL_RenderFillRect(renderer, &shadowRect);
    }
    
    // メインタイル
    SetRenderColorWithAlpha(ColorPalette::TILE_MAIN, 1.0f);
    SDL_RenderFillRect(renderer, &tileRect);
    
    // エッジのハイライト
    if (!hasTop) {  // 上端
        SDL_Rect topEdge = {x, y, TILE_SIZE, 2};
        SetRenderColorWithAlpha(ColorPalette::TILE_EDGE, 1.0f);
        SDL_RenderFillRect(renderer, &topEdge);
    }
    
    if (!hasLeft) {  // 左端
        SDL_Rect leftEdge = {x, y, 2, TILE_SIZE};
        SetRenderColorWithAlpha(ColorPalette::TILE_EDGE, 0.8f);
        SDL_RenderFillRect(renderer, &leftEdge);
    }
    
    // 暗い影の部分
    if (!hasBottom) {  // 下端
        SDL_Rect bottomEdge = {x, y + TILE_SIZE - 2, TILE_SIZE, 2};
        SetRenderColorWithAlpha(ColorPalette::TILE_SHADOW, 1.0f);
        SDL_RenderFillRect(renderer, &bottomEdge);
    }
    
    if (!hasRight) {  // 右端
        SDL_Rect rightEdge = {x + TILE_SIZE - 2, y, 2, TILE_SIZE};
        SetRenderColorWithAlpha(ColorPalette::TILE_SHADOW, 0.8f);
        SDL_RenderFillRect(renderer, &rightEdge);
    }
}

// 美化されたUI描画
void Game::RenderEnhancedUI() {
    if (!font) return;
    
    // UIの背景を美化
    SDL_Rect uiArea = {0, 0, 800, 80};
    DrawGradientRect(uiArea, ColorPalette::BACKGROUND_MID, ColorPalette::BACKGROUND_DARK);
    
    // スタイル化されたHPバー
    RenderStylizedHealthBar();
    
    // スタイル化された魂ゲージ
    RenderStylizedSoulMeter();
    
    // その他のUI要素も美化された色で表示
    std::string scoreText = "Score: " + std::to_string(score);
    RenderText(scoreText, 500, 10, ColorPalette::UI_PRIMARY);
    
    std::string livesText = "Lives: " + std::to_string(lives);
    RenderText(livesText, 500, 30, ColorPalette::UI_PRIMARY);
}

// スタイル化されたHPバー
void Game::RenderStylizedHealthBar() {
    int startX = 10, startY = 10;
    int heartSize = 20, spacing = 25;
    
    for (int i = 0; i < maxHealth; i++) {
        SDL_Rect heartRect = {startX + i * spacing, startY, heartSize, heartSize};
        
        if (i < playerHealth) {
            // 満タンのハート（グラデーション効果）
            SetRenderColorWithAlpha(ColorPalette::HEALTH_GREEN, 1.0f);
            SDL_RenderFillRect(renderer, &heartRect);
            
            // ハイライト
            SDL_Rect highlight = {heartRect.x + 2, heartRect.y + 2, heartRect.w - 4, heartRect.h / 2};
            SetRenderColorWithAlpha(ColorPalette::UI_ACCENT, 0.6f);
            SDL_RenderFillRect(renderer, &highlight);
        } else {
            // 空のハート
            SetRenderColorWithAlpha(ColorPalette::UI_SECONDARY, 0.5f);
            SDL_RenderFillRect(renderer, &heartRect);
        }
        
        // ハートの縁取り
        SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
        SDL_RenderDrawRect(renderer, &heartRect);
    }
}

// スタイル化された魂ゲージ
void Game::RenderStylizedSoulMeter() {
    if (soulCount <= 0) return;
    
    int startX = 200, startY = 15;
    int meterWidth = 150, meterHeight = 10;
    
    // 背景
    SDL_Rect backgroundRect = {startX, startY, meterWidth, meterHeight};
    SetRenderColorWithAlpha(ColorPalette::BACKGROUND_DARK, 0.8f);
    SDL_RenderFillRect(renderer, &backgroundRect);
    
    // 魂ゲージ
    int fillWidth = (meterWidth * soulCount) / maxSoul;
    SDL_Rect fillRect = {startX, startY, fillWidth, meterHeight};
    
    // グラデーション効果
    DrawGradientRect(fillRect, ColorPalette::SOUL_BLUE, ColorPalette::UI_ACCENT);
    
    // 縁取り
    SetRenderColorWithAlpha(ColorPalette::UI_PRIMARY, 1.0f);
    SDL_RenderDrawRect(renderer, &backgroundRect);
    
    // 魂の数値
    std::string soulText = std::to_string(soulCount) + "/" + std::to_string(maxSoul);
    RenderText(soulText, startX + meterWidth + 10, startY - 2, ColorPalette::SOUL_BLUE);
}

// ユーティリティ: グラデーション矩形描画
void Game::DrawGradientRect(SDL_Rect rect, SDL_Color topColor, SDL_Color bottomColor) {
    for (int i = 0; i < rect.h; i++) {
        float ratio = (float)i / rect.h;
        
        Uint8 r = topColor.r + (bottomColor.r - topColor.r) * ratio;
        Uint8 g = topColor.g + (bottomColor.g - topColor.g) * ratio;
        Uint8 b = topColor.b + (bottomColor.b - topColor.b) * ratio;
        Uint8 a = topColor.a + (bottomColor.a - topColor.a) * ratio;
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawLine(renderer, rect.x, rect.y + i, rect.x + rect.w, rect.y + i);
    }
}

// ユーティリティ: 光エフェクト描画
void Game::DrawGlowEffect(int x, int y, int radius, SDL_Color color, float intensity) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    for (int r = radius; r > 0; r -= 2) {
        float alpha = intensity * (1.0f - (float)r / radius) * 255;
        if (alpha > 255) alpha = 255;
        if (alpha < 0) alpha = 0;
        
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, (Uint8)alpha);
        
        // 円形の近似として、複数の点を描画
        for (int angle = 0; angle < 360; angle += 10) {
            int px = x + cos(angle * M_PI / 180) * r;
            int py = y + sin(angle * M_PI / 180) * r;
            SDL_RenderDrawPoint(renderer, px, py);
        }
    }
}

// ユーティリティ: アルファ付き色設定
void Game::SetRenderColorWithAlpha(SDL_Color color, float alpha) {
    Uint8 finalAlpha = (Uint8)(color.a * alpha);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, finalAlpha);
}

// === 光線攻撃システムの実装 ===

// 光線攻撃処理
void Game::HandleBeamAttack() {
    // コントローラー入力を優先
    if (controllerConnected && gameController) {
        if (GetControllerButton(SDL_CONTROLLER_BUTTON_Y)) {
            if (!isChargingBeam && !isFiringBeam && soulCount >= beamCost) {
                std::cout << "🎮 コントローラーYボタン押下 - 光線チャージ開始" << std::endl;
                StartBeamCharge();
            }
        } else if (isChargingBeam) {
            // Yボタンを離した時に発射
            std::cout << "🎮 コントローラーYボタン離し - 光線発射" << std::endl;
            FireBeam();
        }
    } else {
        // キーボード入力
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        
        if (currentKeyStates[SDL_SCANCODE_Y]) {
            if (!isChargingBeam && !isFiringBeam && soulCount >= beamCost) {
                std::cout << "⌨️ キーボードYキー押下 - 光線チャージ開始" << std::endl;
                StartBeamCharge();
            }
        } else if (isChargingBeam) {
            // Yキーを離した時に発射
            std::cout << "⌨️ キーボードYキー離し - 光線発射" << std::endl;
            FireBeam();
        }
    }
}

// 光線攻撃状態の更新
void Game::UpdateBeamAttack() {
    // チャージ中の処理
    if (isChargingBeam) {
        beamChargeTime++;
        
        // チャージ進捗を表示（1秒ごと）
        if (beamChargeTime % 60 == 0) {
            std::cout << "🔍 光線チャージ中: " << beamChargeTime << "/" << maxBeamChargeTime << " (" 
                      << (beamChargeTime * 100 / maxBeamChargeTime) << "%)" << std::endl;
        }
        
        // 最大チャージ時間に達した場合は警告のみ（自動発射は無効）
        if (beamChargeTime >= maxBeamChargeTime) {
            std::cout << "🔍 最大チャージ時間到達！ ボタンを離して発射してください" << std::endl;
        }
        
        // チャージエフェクト
        playerGlowIntensity = 1.5f + (float)beamChargeTime / maxBeamChargeTime * 0.5f;
        SpawnParticle(playerX + playerRect.w/2, playerY + playerRect.h/2, 0, -1, PARTICLE_SPARK, 10);
    }
    
    // 発射中の処理
    if (isFiringBeam) {
        beamTimer--;
        
        // 光線の敵判定
        for (auto& enemy : enemies) {
            if (enemy.active && CheckBeamHit(enemy)) {
                enemy.TakeDamage(beamDamage);
                
                if (!enemy.active) {
                    CreateEnemyDeathEffect(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2);
                    SpawnParticleBurst(enemy.x + enemy.rect.w/2, enemy.y + enemy.rect.h/2, PARTICLE_EXPLOSION, 15);
                    CollectSoul(1);  // 光線で倒した敵からは魂を1個獲得
                    score += 300;
                }
            }
        }
        
        // 光線終了
        if (beamTimer <= 0) {
            EndBeamAttack();
        }
    }
}

// 光線チャージ開始
void Game::StartBeamCharge() {
    std::cout << "🔍 StartBeamCharge呼び出し - 現在のMP: " << soulCount << ", 消費予定: " << beamCost << std::endl;
    
    isChargingBeam = true;
    beamChargeTime = 0;
    soulCount -= beamCost;  // MP消費
    
    std::cout << "🔍 MP消費後 - 現在のMP: " << soulCount << std::endl;
    
    // チャージ開始エフェクト
    SpawnParticleBurst(playerX + playerRect.w/2, playerY + playerRect.h/2, PARTICLE_SPARK, 8);
    StartScreenShake(2, 10);
    
    std::cout << "⚡ 光線チャージ開始！ MP消費: " << beamCost << std::endl;
}

// 光線発射
void Game::FireBeam() {
    isChargingBeam = false;
    isFiringBeam = true;
    beamTimer = beamDuration;
    
    // 発射エフェクト
    SpawnParticleBurst(playerX + playerRect.w/2, playerY + playerRect.h/2, PARTICLE_EXPLOSION, 20);
    StartScreenShake(5, 15);
    
    // プレイヤーの光エフェクト
    playerGlowIntensity = 2.0f;
    
    std::cout << "💥 光線発射！ チャージ時間: " << beamChargeTime << "フレーム" << std::endl;
}

// 光線攻撃終了
void Game::EndBeamAttack() {
    isFiringBeam = false;
    beamTimer = 0;
    playerGlowIntensity = 1.0f;
    
    std::cout << "⚡ 光線攻撃終了" << std::endl;
}

// 光線ヒット判定
bool Game::CheckBeamHit(const Enemy& enemy) {
    // 光線の範囲（プレイヤーの向いている方向に直線）
    int beamStartX = playerX + (lastDirection > 0 ? playerRect.w : -100);
    int beamEndX = playerX + (lastDirection > 0 ? playerRect.w + 100 : -100);
    int beamY = playerY + playerRect.h/2;
    
    // 敵の位置が光線の範囲内かチェック
    return (enemy.x < beamEndX && enemy.x + enemy.rect.w > beamStartX &&
            enemy.y < beamY + 20 && enemy.y + enemy.rect.h > beamY - 20);
}

// 光線描画
void Game::RenderBeam() {
    if (!isFiringBeam) {
        std::cout << "🔍 RenderBeam: 発射中ではない (isFiringBeam=false)" << std::endl;
        return;
    }
    
    std::cout << "🎨 光線描画中..." << std::endl;
    
    // 光線の描画
    int beamStartX = playerX + (lastDirection > 0 ? playerRect.w : -100);
    int beamEndX = playerX + (lastDirection > 0 ? playerRect.w + 100 : -100);
    int beamY = playerY + playerRect.h/2;
    
    // 光線の色（チャージ時間に応じて変化）
    SDL_Color beamColor;
    if (beamChargeTime < maxBeamChargeTime / 3) {
        beamColor = {255, 100, 100, 255};  // 赤
    } else if (beamChargeTime < maxBeamChargeTime * 2 / 3) {
        beamColor = {255, 255, 100, 255};  // 黄
    } else {
        beamColor = {100, 255, 255, 255};  // 青
    }
    
    // 光線の描画
    SDL_Rect beamRect = {
        WorldToScreenX(beamStartX),
        WorldToScreenY(beamY - 5),
        abs(WorldToScreenX(beamEndX) - WorldToScreenX(beamStartX)),
        10
    };
    
    SetRenderColorWithAlpha(beamColor, 0.8f);
    SDL_RenderFillRect(renderer, &beamRect);
    
    // 光線の光エフェクト
    DrawGlowEffect(WorldToScreenX(beamStartX), WorldToScreenY(beamY), 30, beamColor, 0.6f);
    DrawGlowEffect(WorldToScreenX(beamEndX), WorldToScreenY(beamY), 30, beamColor, 0.6f);
}

// ステージ読み込み処理