// Gameクラスのヘッダファイルをインクルード（クラス定義を読み込み）
#include "Game.h"
// C++標準ライブラリ: コンソール出力（std::cout, std::endl）用
#include <iostream>

// 静的メンバ変数の初期化: すべてのGameインスタンスで共有されるレンダラー
// 初期値はnullptr（無効なポインタ）で、Initialize関数内で実際のレンダラーを設定
SDL_Renderer* Game::renderer = nullptr;

// === 敵キャラクタークラスのメソッド実装 ===
void Enemy::Update() {
    // 敵の水平移動（方向 × 速度）
    x += speed * direction;
    // 描画用矩形の位置を更新
    rect.x = x;
    rect.y = y;
}

// コンストラクタ: Gameオブジェクト作成時に呼ばれる初期化処理
// メンバ初期化リストを使用して各メンバ変数を初期値で設定
Game::Game() : isRunning(false), window(nullptr), 
               playerX(100), playerY(300), playerSpeed(5),
               playerVelY(0), gravity(0.8f), isOnGround(false), isJumping(false) {
    
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
    // グーンバ風の敵1（地面を歩く）
    Enemy goomba1;
    goomba1.x = 400;
    goomba1.y = (MAP_HEIGHT - 3) * TILE_SIZE;  // 地面の上
    goomba1.speed = 1;
    goomba1.direction = -1;  // 左向きに移動
    goomba1.rect = {goomba1.x, goomba1.y, 30, 30};
    goomba1.active = true;
    enemies.push_back(goomba1);
    
    // グーンバ風の敵2（プラットフォーム上）
    Enemy goomba2;
    goomba2.x = 320;
    goomba2.y = 11 * TILE_SIZE;  // 中央プラットフォーム上
    goomba2.speed = 2;
    goomba2.direction = 1;   // 右向きに移動
    goomba2.rect = {goomba2.x, goomba2.y, 30, 30};
    goomba2.active = true;
    enemies.push_back(goomba2);
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
        // 0: レンダラーフラグ（0で標準設定）
        renderer = SDL_CreateRenderer(window, -1, 0);
        if (renderer) {
            // レンダラーのデフォルト描画色を白色（R=255, G=255, B=255, A=255）に設定
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            std::cout << "レンダラー作成成功" << std::endl;
        }
        
        // 初期化が完了したのでゲーム実行フラグをtrueに設定
        isRunning = true;
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
    // イベントキューから1つのイベントを取得（イベントがない場合は何もしない）
    SDL_PollEvent(&event);
    
    // イベントの種類に応じて処理を分岐
    switch (event.type) {
        case SDL_QUIT:  // ウィンドウの×ボタンが押された場合
            isRunning = false;  // ゲームループを終了
            break;
        default:  // その他のイベントは無視
            break;
    }
    
    // リアルタイムキーボード入力処理（押されている間継続的に反応）
    // SDL_PollEventとは異なり、現在のキーの押下状態を取得
    const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
    
    // === 水平移動（マリオ風の左右移動） ===
    // 左移動: 左矢印キー または Aキー が押されている場合
    if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) {
        playerX -= playerSpeed;  // X座標を移動速度分だけ減少（左に移動）
    }
    // 右移動: 右矢印キー または Dキー が押されている場合
    if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) {
        playerX += playerSpeed;  // X座標を移動速度分だけ増加（右に移動）
    }
    
    // === ジャンプ（マリオ風のジャンプアクション） ===
    // ジャンプ: スペースキー または Wキー が押され、かつ地面に接触している場合のみ
    if ((currentKeyStates[SDL_SCANCODE_SPACE] || currentKeyStates[SDL_SCANCODE_W]) && isOnGround) {
        playerVelY = -15.0f;  // 上向きの初速度（負の値が上方向）
        isOnGround = false;   // 地面から離れる
        isJumping = true;     // ジャンプ状態に設定
    }
    
    // === 画面境界チェック（X軸のみ、Y軸は物理システムで管理） ===
    if (playerX < 0) playerX = 0;  // 左端を超えた場合、左端に固定
    if (playerX > (MAP_WIDTH * TILE_SIZE) - playerRect.w) {
        playerX = (MAP_WIDTH * TILE_SIZE) - playerRect.w;  // 右端制限
    }
}

// ゲーム状態更新関数: 1フレームごとのゲームロジック処理
void Game::Update() {
    // === プレイヤーの物理計算（重力とジャンプ） ===
    // 地面に接触していない場合は重力を適用
    if (!isOnGround) {
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
    
    // === 敵キャラクターの更新 ===
    for (auto& enemy : enemies) {
        if (enemy.active) {
            enemy.Update();
            
            // 敵の画面端・プラットフォーム端での方向転換
            // 左端に到達した場合
            if (enemy.x <= 0) {
                enemy.direction = 1;   // 右向きに変更
            }
            // 右端に到達した場合
            else if (enemy.x >= (MAP_WIDTH * TILE_SIZE) - enemy.rect.w) {
                enemy.direction = -1;  // 左向きに変更
            }
            
            // プラットフォームの端での方向転換チェック
            // 敵の足元のタイル座標を計算
            int enemyTileX = (enemy.x + enemy.rect.w / 2) / TILE_SIZE;
            int enemyTileY = (enemy.y + enemy.rect.h) / TILE_SIZE + 1;  // 足元の下
            
            // マップ範囲内で、足元が空の場合は方向転換
            if (enemyTileX >= 0 && enemyTileX < MAP_WIDTH && 
                enemyTileY >= 0 && enemyTileY < MAP_HEIGHT) {
                if (map[enemyTileY][enemyTileX] == 0) {  // 足元が空
                    enemy.direction *= -1;  // 方向転換
                }
            }
        }
    }
}

// 衝突判定処理: プレイヤーと地面・プラットフォームの衝突をチェック
void Game::CheckCollisions(int x, float& y) {
    // プレイヤーの足元のタイル座標を計算
    int tileX = (x + playerRect.w / 2) / TILE_SIZE;  // プレイヤー中央のX座標
    int tileY = ((int)y + playerRect.h) / TILE_SIZE; // プレイヤー足元のY座標
    
    // マップ範囲内かチェック
    if (tileX >= 0 && tileX < MAP_WIDTH && tileY >= 0 && tileY < MAP_HEIGHT) {
        // 地面タイル（1）に衝突した場合
        if (map[tileY][tileX] == 1) {
            // プレイヤーを地面の上に正確に配置
            y = tileY * TILE_SIZE - playerRect.h;
            playerVelY = 0;       // 落下速度をリセット
            isOnGround = true;    // 地面接触フラグをON
            isJumping = false;    // ジャンプ終了
            return;
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

// 描画処理関数: 画面をクリアし、すべてのゲームオブジェクトを描画
void Game::Render() {
    // 背景色を空色（マリオ風の青空）に設定
    SDL_SetRenderDrawColor(renderer, 135, 206, 235, 255);  // スカイブルー
    // 画面全体を背景色でクリア（前フレームの描画内容を消去）
    SDL_RenderClear(renderer);
    
    // === マップ（地面・プラットフォーム）の描画 ===
    RenderMap();
    
    // === プレイヤーキャラクターの描画 ===
    // プレイヤーを赤色（マリオ風）で描画
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // 赤色
    SDL_RenderFillRect(renderer, &playerRect);
    
    // === 敵キャラクターの描画 ===
    // 敵を紫色（グーンバ風）で描画
    SDL_SetRenderDrawColor(renderer, 139, 0, 139, 255);  // 紫色
    for (const auto& enemy : enemies) {
        if (enemy.active) {
            SDL_RenderFillRect(renderer, &enemy.rect);
        }
    }
    
    // バックバッファの内容を画面に表示（ダブルバッファリング）
    // この関数が呼ばれるまで描画内容は画面に表示されない
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
}