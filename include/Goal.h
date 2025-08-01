#pragma once

#include <SDL.h>
#include <cmath>

// ゴールの種類を定義する列挙型
enum GoalType {
    GOAL_FLAG = 0,      // フラッグポール（マリオ風）
    GOAL_DOOR = 1,      // ドア
    GOAL_COLLECT_ALL = 2 // 全アイテム収集
};

// ゲーム内ゴールクラス: ステージクリア条件を管理
class Goal {
public:
    int x, y;                    // ゴールの位置座標
    GoalType type;               // ゴールの種類（GOAL_FLAG, GOAL_DOORなど）
    bool active;                 // ゴールがアクティブかどうか
    SDL_Rect rect;               // 衝突判定用の矩形
    float animationTimer;        // アニメーション用のタイマー
    
    // コンストラクタ: x,y位置とゴール種類を指定して初期化
    Goal(int x, int y, GoalType type);
    
    // ゴールの状態を1フレーム分更新（アニメーションなど）
    void Update();
}; 