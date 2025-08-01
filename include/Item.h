#pragma once

#include <SDL.h>
#include <cmath>

// アイテムの種類を定義する列挙型
enum ItemType {
    COIN = 0,           // コイン（スコア+100）
    POWER_MUSHROOM = 1, // パワーアップキノコ（サイズアップ）
    LIFE_UP = 2         // 1UPキノコ（ライフ+1）
};

// ゲーム内アイテムクラス: プレイヤーが収集できるオブジェクト
class Item {
public:
    int x, y;                    // アイテムの位置座標
    ItemType type;               // アイテムの種類（COIN, POWER_MUSHROOMなど）
    bool active;                 // アイテムがアクティブ（表示・衝突判定有効）かどうか
    bool collected;              // アイテムが収集済みかどうか
    SDL_Rect rect;               // 衝突判定用の矩形
    float animationTimer;        // アニメーション用のタイマー
    
    // コンストラクタ: x,y位置とアイテム種類を指定して初期化
    Item(int x, int y, ItemType type);
    
    // アイテムの状態を1フレーム分更新（アニメーションなど）
    void Update();
}; 