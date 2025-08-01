# Mario-style 2D Game Project 🍄

C++とSDL2を使用したマリオ風プラットフォーマーゲームです。

## 🎮 機能

- **プラットフォームアクション**: 重力とジャンプによるリアルな物理システム
- **タイルベースマップ**: マリオ風のステージデザイン
- **敵キャラクター**: グーンバ風の敵が徘徊
- **ウィンドウ管理**: 60FPSの滑らかなゲームループ
- **キーボード操作**: 直感的な移動とジャンプ操作

## 🕹️ 操作方法

- **A** または **←**: 左に移動
- **D** または **→**: 右に移動  
- **W** または **Space**: ジャンプ（地面にいる時のみ）
- ウィンドウを閉じてゲーム終了

## 🎯 ゲームシステム

### 物理システム
- **重力**: 現実的な落下システム
- **ジャンプ**: 地面接触時のみジャンプ可能
- **衝突判定**: プラットフォームとの正確な当たり判定

### マップシステム
- **タイルベース**: 32x32ピクセルのタイル構成
- **プラットフォーム**: 様々な高さの足場
- **地面**: 基本となる地面ブロック

### 敵システム
- **自動移動**: 敵キャラクターが自動で徘徊
- **方向転換**: プラットフォームの端で自動的に方向転換

## 📋 必要な依存関係

- CMake (3.16以上)
- SDL2
- SDL2_image
- C++17対応コンパイラ

## 🛠️ macOSでのセットアップ

### Homebrewを使用してSDL2をインストール

```bash
# Homebrewがインストールされていない場合
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# SDL2とSDL2_imageをインストール
brew install sdl2 sdl2_image cmake
```

## 🔨 ビルド方法

```bash
# プロジェクトディレクトリに移動
cd 2Dgame

# ビルドディレクトリを作成（初回のみ）
mkdir build
cd build

# CMakeでプロジェクトを設定
cmake ..

# ビルドを実行
make

# ゲームを実行
./2DGame
```

## 📁 プロジェクト構造

```
2Dgame/
├── CMakeLists.txt      # ビルド設定
├── README.md           # このファイル
├── include/            # ヘッダファイル
│   └── Game.h          # ゲームクラス・敵クラス定義
├── src/                # ソースファイル
│   ├── Game.cpp        # ゲームロジック実装
│   └── main.cpp        # メイン関数
├── assets/             # ゲームアセット（将来の拡張用）
└── build/              # ビルド生成物
    └── 2DGame          # 実行ファイル
```

## 🚀 今後の拡張予定

### Phase 1: ビジュアル強化
1. **スプライト画像**: マリオとグーンバの画像
2. **アニメーション**: 歩行・ジャンプアニメーション
3. **背景**: パララックススクロール背景

### Phase 2: ゲームプレイ拡張
4. **プレイヤー・敵衝突**: 踏みつけ攻撃システム
5. **アイテム**: コインやパワーアップアイテム
6. **スコアシステム**: ポイント・ライフシステム

### Phase 3: ステージ拡張
7. **サイドスクロール**: カメラ追従システム
8. **複数ステージ**: レベル選択とクリア条件
9. **ボスキャラクター**: ステージボス戦

### Phase 4: 高度な機能
10. **効果音・BGM**: SDL_mixerによる音響システム
11. **パーティクル**: 爆発・ジャンプエフェクト
12. **セーブシステム**: 進行状況の保存

## 🎨 ゲーム画面

- **赤色**: プレイヤーキャラクター（マリオ）
- **茶色**: 地面・プラットフォーム
- **紫色**: 敵キャラクター（グーンバ）
- **空色**: 背景（青空）

## 🏆 学習ポイント

このプロジェクトを通して学べること：

- **ゲーム物理**: 重力・ジャンプの実装
- **衝突判定**: タイルベース衝突システム
- **ゲームループ**: 60FPSでの安定した更新
- **オブジェクト管理**: 敵キャラクターの配列管理
- **C++実践**: クラス設計・メモリ管理

## 📝 開発履歴

- **v1.0**: 基本的な2Dゲームエンジン
- **v2.0**: マリオ風プラットフォーマーに進化 🍄

---

**Have fun jumping around! 🎮✨** 