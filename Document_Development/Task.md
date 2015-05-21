## ロードマップ

### 2015.1 (現在実装中)
* 公式サイト設置
* 2D形状描画
* 2D衝突判定
* ファイルシステム
* Java対応
* Win、Linux、Mac対応

### 2015.2
* 2D物理
* ALSL(専用シェーダー言語)
* アニメーションオブジェクト
* D言語対応
* C#からC++自動生成(ユーザーに影響なし)

### 2015.3
* 3D
* 配置ツール
* リアルタイム変数プレビュー

## タスク一覧

### サンプル
* ジャンル毎のサンプル
個別の機能のサンプルでなく、小規模なSTG、アクション、RPGといったサンプルを追加する。

### 2D

#### アニメーションオブジェクト
* 仕様策定

#### 物理・衝突判定
* BOX2D(予定)の組み込み

### 3D
#### 文字表示

#### ポイントライト
* シャドウマップ生成

#### スポットライト
* シャドウマップ生成

#### ディレクショナルライト
* リファクタリング
* シャドウマップ生成改良

#### マップ

#### 木
* 本当に必要なのか議論(外部ツールが発展しているため)

#### 流体

#### 水面

#### リフレクションキューブ
* オブジェクトとして配置可能にする
* 任意の位置のリフレクションキューブを生成可能にする

#### スカイドーム
* キューブマップ形式からスカイドームを表示できるようにする

#### フォグ

#### シェーディング
* Gバッファ内容確定
* Gバッファ出力
* ディレクショナルライトを描画に反映
	-ディフューズの処理
	-スペキュラの処理
* ポイントライトを描画に反映
	-ディフューズの処理
	-スペキュラの処理
* スポットライトを描画に反映
	-ディフューズの処理
	-スペキュラの処理
* リフレクションを描画に反映
* ライトマップを描画に反映

#### ライトマップ
* ライトマップ生成アルゴリズム開発
* 実装

#### 3D向けポストエフェクト
* トーンマッピング
* ぼけ
* モーションブラー
* アンチエイリアス
* SSリフレクション
* SSAO改良

速度向上、精度向上

#### 物理・衝突判定
* Bulletの組み込み

#### カリング
* 組み込み

#### 並列度向上
* コマンド化して並列度向上

### 2D、3D共通ポストエフェクト
* ガウス性能向上
* 放射方向ブラー

### ALSL(専用シェーダー言語)
C#のサブセットとしてシェーダー言語を実装する。コンパイルした時にHLSLやGLSLのコードを生成し、ACEで読み込めるようにする。
構文解析にはRoslynを使用する。

* 仕様策定
C#とHLSL、GLSLの仕様を比較し、使用可能な機能を抽出する。

* コード変換
C#からシェーダーに変換できるようにする。

### 入力
#### インプットメソッド
* ウインドウメッセージが取れるよう、glfwへコミット、もしくは改造
* 実装

### サウンド
#### 3Dサウンド
* アルゴリズム開発
* 実装

### ファイルシステム

* 仕様策定
* 一括読み込み実装
* ストリーム読み込み実装
* (Zip読み込み実装)

### C++の自動生成
C#のソースコードからC++のコードを生成する。それにより、C++とC#のコードの一致を図る。
C#の構文解析にはRoslynを使用する。

### 他言語対応
C#のソースコードから他の言語のコードを生成する。
Java、Python、Ruby、D等に対応する。

### ツール
#### パッケージ生成ツールの開発(CUI)

#### パッケージ生成ツールの開発(GUI)

#### サウンド編集ツールの開発
* 本当に必要なのか議論

#### 3Dモデル、素材設定、2D配置、3D配置ツールの開発
* どこまで一つのツールにまとめるか議論
* 仕様策定
* 開発

### デバッグ・プロファイル
* リアルタイム変数プレビュー
* 他未定

### ネットワーク
* 未定

### 並列化
* 並列化できる部分を考える
* 実装
* プロファイラによる検証

### エンジン

### リソース
#### 動的リロード
* 3Dモデルの動的リロード対応
* エフェクトの動的リロード対応
* 音の動的リロード対応

#### フォーマット対応
* FBX

* DDS(OpenGL)
* 他画像形式
* 他音形式
* 他モデル形式