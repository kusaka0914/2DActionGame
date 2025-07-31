// ゲームクラスの定義を読み込み（Game.hファイルをインクルード）
#include "Game.h"
// C++標準ライブラリ: コンソール出力（std::cout）用
#include <iostream>

// 画面幅の定数定義（ピクセル単位）- マリオ風ゲームに合わせて調整
const int SCREEN_WIDTH = 800;
// 画面高さの定数定義（ピクセル単位）- マリオ風ゲームに合わせて調整
const int SCREEN_HEIGHT = 608;  // 19タイル × 32ピクセル = 608ピクセル
// 目標フレームレート（1秒間に60回更新）
const int FPS = 60;
// 1フレームあたりの時間（ミリ秒）: 1000ms ÷ 60fps = 約16.67ms
const int FRAME_DELAY = 1000 / FPS;

// メイン関数: プログラムの開始地点（エントリーポイント）
// argc: コマンドライン引数の個数, argv: コマンドライン引数の配列
int main(int argc, char* argv[]) {
    // フレーム開始時刻を記録する変数（SDL_GetTicks()で取得）
    Uint32 frameStart;
    // 1フレームの処理時間を計算する変数（ミリ秒）
    int frameTime;
    
    // Gameクラスのインスタンスを動的に作成（ヒープメモリに確保）
    Game* game = new Game();
    
    // ゲーム初期化を実行: ウィンドウ作成、SDL初期化など
    // タイトル="Mario-style 2D Game", 位置=画面中央, サイズ=800x608, フルスクリーン=false
    if (game->Initialize("Mario-style 2D Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                        SCREEN_WIDTH, SCREEN_HEIGHT, false)) {
        
        // 初期化成功時のメッセージをコンソールに出力
        std::cout << "🍄 マリオ風ゲーム開始！" << std::endl;
        // プレイヤーへの操作説明をコンソールに出力
        std::cout << "操作方法:" << std::endl;
        std::cout << "  A/← : 左に移動" << std::endl;
        std::cout << "  D/→ : 右に移動" << std::endl;
        std::cout << "  W/Space : ジャンプ" << std::endl;
        std::cout << "  ウィンドウを閉じてゲーム終了" << std::endl;
        std::cout << "🎮 重力とジャンプでプラットフォームアクションを楽しもう！" << std::endl;
        
        // メインゲームループ: ゲームが終了するまで繰り返し実行
        while (game->Running()) {
            // フレーム処理開始時刻を記録（フレームレート制御用）
            frameStart = SDL_GetTicks();
            
            // イベント処理: キーボード入力、マウス操作、ウィンドウ閉じるボタンなど
            game->HandleEvents();
            // ゲーム状態更新: プレイヤー位置、物理計算、敵AI、衝突判定など
            game->Update();
            // 画面描画: 背景クリア、マップ描画、プレイヤー・敵描画、画面更新
            game->Render();
            
            // 1フレームの処理にかかった時間を計算（ミリ秒）
            frameTime = SDL_GetTicks() - frameStart;
            
            // フレームレート制御: 処理が早く終わった場合は待機して60FPSを維持
            if (FRAME_DELAY > frameTime) {
                // 不足分の時間だけスリープ（CPUの無駄遣いを防ぐ）
                SDL_Delay(FRAME_DELAY - frameTime);
            }
        }
    } else {
        // 初期化失敗時のエラーメッセージをコンソールに出力
        std::cout << "❌ ゲームの初期化に失敗しました" << std::endl;
    }
    
    // 動的に確保したGameオブジェクトのメモリを解放（メモリリーク防止）
    delete game;
    // プログラム正常終了を示す戻り値（0 = 成功）
    return 0;
}