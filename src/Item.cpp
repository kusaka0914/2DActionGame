#include "Item.h"
#include <iostream>

// アイテムコンストラクタ: x,y位置とアイテム種類を指定して初期化
Item::Item(int x, int y, ItemType type) : x(x), y(y), type(type), active(true), collected(false), animationTimer(0.0f) {
    // アイテムの種類に応じて矩形サイズを設定
    switch (type) {
        case COIN:
            rect = {x, y, 20, 20};      // コインは小さめ
            break;
        case POWER_MUSHROOM:
            rect = {x, y, 24, 24};      // キノコは中サイズ
            break;
        case LIFE_UP:
            rect = {x, y, 24, 24};      // 1UPキノコも中サイズ
            break;
        default:
            rect = {x, y, 20, 20};      // デフォルトサイズ
            break;
    }
}

void Item::Update() {
    // アニメーション用タイマーを更新（浮遊効果などに使用）
    animationTimer += 0.1f;
    if (animationTimer > 360.0f) {
        animationTimer = 0.0f;
    }
    
    // コインの場合は簡単な浮遊アニメーション
    if (type == COIN) {
        // サインカーブで上下に浮遊
        rect.y = y + (int)(sin(animationTimer * 0.1f) * 3.0f);
    }
} 