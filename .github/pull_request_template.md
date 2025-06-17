
## 🔗 関連リンク
<!-- 関連するPR(libcitygmlのPRなど)、仕様書のリンク(社外の方を除く)、Jiraチケットのリンク(社外の方を除く)等を書いてください。 -->

## ✅ レビュー前確認項目
- [ ] 自動ビルド・テストが通っていること

## ✅ マージ前確認項目
- [ ] (libcitygmlの変更がある場合)libcitygmlがmasterの最新版になっていること
<!--
 libcitygmlの変更がある場合、以下の手順でlibcitygmlのPRを先にマージしてからsubmoduleをmasterに更新する。
1. libcitygmlのPRをmasterにマージ
2. 以下のコマンドでsubmoduleをmasterに更新
```
# libcitygmlをmasterの最新版にする
cd 3rdparty/libcitygml
git checkout master
git pull

# submoduleを更新する
cd ../..
git add 3rdparty/libcitygml
git commit -m "Update submodule"
git push origin {ブランチ名}
```
-->


<!-- 社内の人向け: 以下の項目は、開発当初のストーリー設計書通りであれば省略可能です。
                 設計に変更があった場合のみ、変更点を周知するために記載してください。 -->

## 🚀 実装内容
<!-- GUIに関する実装がある場合はスクリーンショットを貼ってください。実装した内容について書いてください。 -->
## 🌐 影響範囲
<!-- 既存の実装に対して行った変更や、影響がある部分があれば書いてください(特に同時並行している他の作業に影響する時、今後必要な変更を書く)。また社内の人は、影響先の担当者に対して周知をお願いします。 -->
## 🛠️ 動作確認
<!-- レビュアーが動作確認するのに必要な手順と結果を書く。 -->
## ⚠️ 懸念点
<!-- 気になる点、特にレビューしてほしい点等があれば書く。 -->


