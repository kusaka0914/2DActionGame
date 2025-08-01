#include "Goal.h"
#include <iostream>

// ゴールコンストラクタ: x,y位置とゴール種類を指定して初期化
Goal::Goal(int x, int y, GoalType type) : x(x), y(y), type(type), active(true), animationTimer(0.0f) {
    // ゴールの種類に応じて矩形サイズを設定
    switch (type) {
        case GOAL_FLAG:
            rect = {x, y, 32, 64};      // フラッグポールは縦長
            break;
        case GOAL_DOOR:
            rect = {x, y, 48, 64};      // ドアは大きめ
            break;
        case GOAL_COLLECT_ALL:
            rect = {x, y, 32, 32};      // コレクションゴールは中サイズ
            break;
        default:
            rect = {x, y, 32, 64};      // デフォルトサイズ
            break;
    }
}

void Goal::Update() {
    // アニメーション用タイマーを更新
    animationTimer += 0.05f;
    if (animationTimer > 360.0f) {
        animationTimer = 0.0f;
    }
    
    // フラッグポールの場合は風になびくようなアニメーション
    if (type == GOAL_FLAG) {
        // サインカーブで軽い揺れ
        rect.x = x + (int)(sin(animationTimer * 0.1f) * 2.0f);
    }
} 