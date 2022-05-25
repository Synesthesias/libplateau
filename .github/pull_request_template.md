## 関連リンク
<!-- libcitygmlのPR等、関連するPRがあれば書く。 -->

## 実装内容
<!-- 実装した内容、背景について書く。妥協点があれば書いておく。 -->

## レビュー前確認項目
- [ ] 自動ビルド・テストが通っていること

## マージ前確認項目
- [ ] 自動ビルド・テストが通っていること
- [ ] Squash and Mergeが選択されていること
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

## 動作確認
<!-- レビュアーが動作確認するのに必要な手順と結果を書く。 -->

## その他
<!-- 気になる点、特にレビューしてほしい点等があれば書く。 -->
