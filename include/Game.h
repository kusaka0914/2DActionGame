// ヘッダガード: このファイルが重複してインクルードされることを防ぐ
#pragma once

// SDL2のメインライブラリ: ウィンドウ作成、イベント処理、描画機能など
#include <SDL.h>
// SDL2の画像処理ライブラリ: PNG、JPEG等の画像ファイル読み込み
#include <SDL_image.h>
// C++標準ライブラリ: スマートポインタ（unique_ptr, shared_ptrなど）用
#include <memory>
// C++標準ライブラリ: 動的配列（std::vector）用
#include <vector>

// 敵キャラクタークラス: マリオ風ゲームの敵（グーンバなど）
class Enemy {
public:
    int x, y;              // 敵の位置座標
    int speed;             // 敵の移動速度
    int direction;         // 移動方向（1=右, -1=左）
    SDL_Rect rect;         // 描画・衝突判定用の矩形
    bool active;           // 敵が有効かどうか（倒されたらfalse）
    
    // 敵キャラクターの状態を更新（移動、方向転換など）
    void Update();
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
    
    // === 物理システム（マリオ風ジャンプアクション用） ===
    // Y方向の速度（浮動小数点で精密な物理計算）
    float playerVelY;
    // 重力の強さ（毎フレーム速度に加算される）
    float gravity;
    // プレイヤーが地面に接触しているかのフラグ
    bool isOnGround;
    // ジャンプ中かどうかのフラグ
    bool isJumping;
    
    // === マップシステム（タイルベースのステージ） ===
    // マップの幅（タイル単位）
    static const int MAP_WIDTH = 25;
    // マップの高さ（タイル単位）
    static const int MAP_HEIGHT = 19;
    // 1タイルのピクセルサイズ
    static const int TILE_SIZE = 32;
    // マップデータ（2次元配列: 0=空、1=地面ブロック）
    int map[MAP_HEIGHT][MAP_WIDTH];
    
    // === 敵キャラクターシステム ===
    // 敵キャラクターの配列（複数の敵を管理）
    std::vector<Enemy> enemies;
    
    // === プライベートメソッド（内部処理用） ===
    // 衝突判定処理: プレイヤーと地面・プラットフォームの衝突をチェック
    void CheckCollisions(int x, float& y);
    // マップ描画処理: タイルベースのステージを画面に描画
    void RenderMap();
}; // クラス定義の終了