* [33m183afec[m[33m ([m[1;36mHEAD -> [m[1;32mfeature/grid_merge[m[33m)[m コメント修正
* [33m362d5de[m[33m ([m[1;31morigin/feature/grid_merge[m[33m)[m メモリ安全性向上
* [33m45572c9[m やっかいなメモリバグを修正
* [33me922363[m meshMergerのC++側テストを実装
* [33m540f384[m テスト追記
* [33m7f571ac[m MultiTextureの情報をC#側に渡す機能
* [33md5d6ca4[m リネーム
* [33mccb68f3[m 最小地物のグリッド分類は親の主要地物に合わせるようにしました
* [33mfc057ac[m GridMergeで子CityObjまで含めるのは冗長だったので中止、コメント追記
* [33ma551781[m コードの警告に対処
* [33mc60ae1b[m グリッドごとのメッシュ形状をUnityに送れるように
* [33m6634dc6[m MeshMerger.GridMergeの結果をC#側に渡す機能、そのテスト
* [33mf8d520a[m メッシュのグリッドマージ機能を試作中
* [33m730ffd3[m 少し整理
* [33m404fdb5[m リファクタ
* [33mb516047[m CityModel中のCityObjectを、その位置に応じてグリッドにグループ分けする機能
* [33m2fb99dd[m[33m ([m[1;31morigin/main[m[33m, [m[1;31morigin/HEAD[m[33m, [m[1;32mmain[m[33m)[m ジオメトリのパースをスキップするオプションを作成、そのテストを作成 (#71)
[31m|[m * [33m7e6a31a[m[33m ([m[1;31morigin/feature/skip_geometry_parse[m[33m, [m[1;32mfeature/skip_geometry_parse[m[33m)[m submoduleをmainに
[31m|[m * [33mf854f23[m リネーム
[31m|[m * [33m03b37e1[m ジオメトリのパースをスキップするオプションを作成、そのテストを作成
[31m|[m[31m/[m  
[31m|[m * [33m8028e13[m[33m ([m[1;31morigin/feature/install_gltf_sdk[m[33m)[m test for github windows build5
[31m|[m * [33m8f33159[m test for github windows build4
[31m|[m * [33m03e4143[m test for github windows build3
[31m|[m * [33m6cb564b[m test for github windows build2
[31m|[m * [33m24525fb[m test for github windows build
[31m|[m * [33m5601a90[m test for linux9
[31m|[m * [33m90b8466[m test for linux8
[31m|[m * [33m4bae565[m test for linux7
[31m|[m * [33m5021714[m test for linux6
[31m|[m * [33me5ddeb2[m test for linux5
[31m|[m * [33m28ced2a[m test for linux4
[31m|[m * [33mea85b0b[m test for linux3
[31m|[m * [33me87e780[m test for linux2
[31m|[m * [33mee8e3ad[m test for linux on github2
[31m|[m * [33m4d2f432[m test for linux on github
[31m|[m[31m/[m  
* [33m50c1c29[m Fix/shift jis filename trouble (#67)
[33m|[m * [33m9e8b692[m[33m ([m[1;31morigin/feature/udx[m[33m)[m マルチバイト文字対応
[33m|[m * [33ma0a8a8a[m Change mesh converter api
[33m|[m * [33m90f123c[m API追加
[33m|[m * [33m9ed476d[m Add data
[33m|[m * [33m9f6f234[m Add API
[33m|[m * [33m671c7df[m Update submodule
[33m|[m * [33m97823e8[m スマポ渡しに変更
[33m|[m * [33m2c0a953[m Add test data
[33m|[m * [33m974477d[m Add udx module
[33m|[m [34m|[m * [33m222ecb9[m[33m ([m[1;31morigin/fix/shift-jis_filename_trouble[m[33m)[m use u8string instead of string
[33m|[m [34m|[m * [33ma040557[m use u8path instead of path
[33m|[m [34m|[m[33m/[m  
[33m|[m[33m/[m[34m|[m   
* [34m|[m [33m9eab5e5[m 橋梁等一部の地物についてメッシュ変換されない問題の対応 (#63)
* [34m|[m [33m96ac29b[m Converterがエクスポートしたobjファイルパスを配列で返すC,C#ラッパーを作りました。 (#59)
[35m|[m [34m|[m * [33md64fbdc[m[33m ([m[1;32mfeature/convert_returns_file_names[m[33m)[m 実装すべきか検討不十分なものを実装してしまった部分を削除
[35m|[m [34m|[m * [33mc2742f7[m Converterがエクスポートしたファイル名を配列で返すようにしました。AttributesMapのGetValueSlashSeparatedを実装しました。
[35m|[m [34m|[m[35m/[m  
[35m|[m[35m/[m[34m|[m   
* [34m|[m [33mdadecbb[m CityModel::GetAllCityObjectsOfType のバグを修正しました。テストを書きました。 (#58)
[36m|[m [34m|[m * [33m6e0481d[m[33m ([m[1;32mfix/get_all_city_objects_of_type[m[33m)[m libcitygmlをmasterブランチに
[36m|[m [34m|[m * [33ma4ed098[m 高速化のために keys.size()==1 の場合分けを追加
[36m|[m [34m|[m * [33mad3d004[m CityModel内に存在する地物タイプが1種類であり、かつ引数に複数のタイプを含むタイプフラグを渡すとバグる問題を修正しました。そのテストを記載しました。
[36m|[m [34m|[m[36m/[m  
[36m|[m[36m/[m[34m|[m   
* [34m|[m [33mbfdb49e[m C++でAttributesMapToString() を実装することで属性を確認しやすくしました。 (#57)
[34m|[m[34m/[m  
[34m|[m * [33m2eac3c5[m[33m ([m[1;32mfix/minor_code_problem[m[33m)[m submoduleをmasterブランチに
[34m|[m * [33m217d8af[m attributesmap.cppの不要な前方宣言を削除
[34m|[m * [33mfd7667f[m AttributesMap.Operator<< を実装しました。
[34m|[m * [33me10541b[m 微修正
[34m|[m * [33m2ee5d96[m コード整理
[34m|[m * [33m16ef12d[m コード整理
[34m|[m * [33m6e77657[m AttributesMapToStringを実装して属性の中身をフォーマットして表示するようにしました。
[34m|[m * [33me52e8f0[m AttributesMapの中身表示の実装をC#からC++に移動中
[34m|[m *   [33m03db7fc[m Merge branch 'main' into fix/minor_code_problem
[34m|[m [1;33m|[m[34m\[m  
[34m|[m [1;33m|[m[34m/[m  
[34m|[m[34m/[m[1;33m|[m   
* [1;33m|[m [33m9e61a83[m 主要な都市オブジェクトタイプの判別APIの追加 (#56)
* [1;33m|[m [33md84f37f[m Update submodule (#54)
[1;34m|[m * [33meae313b[m 同上
[1;34m|[m * [33mecb4d2a[m AttributesMap.ToString でAttributesMapの中身を再帰的に文字列かするようにしました。
[1;34m|[m * [33m050e3de[m CitygmlParserParamsのコンストラクタの引数を省略できるようにしたつもりでしたが、実際に省略すると意図しないコンストラクタが呼ばれるバグがあるので省略できないようにしました。
[1;34m|[m[1;34m/[m  
[1;34m|[m *   [33mf7e7f10[m[33m ([m[1;31morigin/feature/parse_ade_object[m[33m)[m Merge branch 'feature/parse_ade_object' of https://github.com/Synesthesias/libplateau into feature/parse_ade_object
[1;34m|[m [1;35m|[m[1;34m\[m  
[1;34m|[m [1;35m|[m[1;34m/[m  
[1;34m|[m[1;34m/[m[1;35m|[m   
* [1;35m|[m [33mcc4e027[m 座標変換追加 (#50)
* [1;35m|[m [33mddd9726[m MeshConvertOptionsDataのマーシャリングの問題を修正しました。 (#48)
[1;36m|[m * [33mfb0e55a[m Update submodule
[1;36m|[m [1;36m|[m * [33m4552b15[m[33m ([m[1;32mfix/mesh_convert_options_data_marshaling[m[33m)[m MeshConvertOptionsDataのマーシャリングの問題を修正しました。
[1;36m|[m [1;36m|[m[1;36m/[m  
[1;36m|[m[1;36m/[m[1;36m|[m   
* [1;36m|[m [33m241eaf7[m メッシュ変換のLOD複数選択対応 (#45)
* [1;36m|[m [33mfea5dfa[m C#の名前空間を変更 (#44)
[31m|[m [1;36m|[m * [33m76cf27e[m[33m ([m[1;32mfeature/change_namespace[m[33m)[m ObjWriterの名前空間を変更
[31m|[m [1;36m|[m * [33m324828b[m 同上
[31m|[m [1;36m|[m * [33mdfb1c2e[m 同上:
[31m|[m [1;36m|[m * [33mf6288f9[m CI修正
[31m|[m [1;36m|[m * [33m722f42f[m README追記
[31m|[m [1;36m|[m * [33m88d75d1[m 名前空間変更
[31m|[m [1;36m|[m[31m/[m  
[31m|[m[31m/[m[1;36m|[m   
* [1;36m|[m [33m6a14ee7[m Feature/delete no poly (#42)
[32m|[m [1;36m|[m * [33m065ddd3[m[33m ([m[1;32mfeature/delete_no_poly[m[33m)[m プルリクでご指摘の箇所を修正しました
[32m|[m [1;36m|[m * [33m82166c0[m readme微修正
[32m|[m [1;36m|[m * [33m5a0a3d3[m 同上
[32m|[m [1;36m|[m * [33md571049[m 同上
[32m|[m [1;36m|[m * [33m1b7cf65[m 同上
[32m|[m [1;36m|[m * [33m2cd6e49[m 同上
[32m|[m [1;36m|[m * [33mf405fbc[m 同上
[32m|[m [1;36m|[m * [33m445249c[m 同上:
[32m|[m [1;36m|[m * [33mb329d42[m 同上
[32m|[m [1;36m|[m * [33m9f6f3c9[m 同上
[32m|[m [1;36m|[m * [33m469e9d2[m 同上
[32m|[m [1;36m|[m * [33mad1e85e[m 同上
[32m|[m [1;36m|[m * [33m71b4505[m github actions の upload-dlls でOS別のアーティファクトをまとめるようにしています
[32m|[m [1;36m|[m * [33m4b9befb[m objWriterで変換した結果、頂点数が0であった場合、そのobjとmatを削除して例外を出すようにしました。
[32m|[m [1;36m|[m * [33m01ef3d4[m MeshGranualityの設定がPerCityAreaのとき、唯一のメッシュの名前をgml名に設定するようにしました。そうすることでメッシュ名の重複がなくなるのでgmlファイルとの紐付けがうまくいきます。
[32m|[m [1;36m|[m * [33m7d06c2b[m Unityでインポート後に一部変換元ファイルがプロセス使用中のままになっているのを解決するため、例外時にもファイルストリームをクローズするようにしました。
[32m|[m [1;36m|[m[32m/[m  
[32m|[m[32m/[m[1;36m|[m   
* [1;36m|[m [33m83e49c5[m ユニットテスト追加 (#39)
* [1;36m|[m [33m491b28a[m DLL内のログをUnity側に転送できるようにしました。 (#38)
[1;36m|[m[1;36m/[m  
[1;36m|[m * [33me2daa4a[m[33m ([m[1;32mfix/obj_writer[m[33m)[m プルリクでご指摘の箇所を修正
[1;36m|[m * [33ma97a95e[m obj_writerの標準出力を dll_logger_ による出力に置き換え
[1;36m|[m * [33mabc14f3[m DLLのログをDLL利用者に転送する機能をgmlパーサーにも適用しました。
[1;36m|[m * [33m4f4ea85[m DLLのログをDLLの利用者に転送するPlateauDLLLogger, C#でログを受け取る DllLogger を作りました。
[1;36m|[m *   [33maaedcc9[m Merge branch 'main' into fix/obj_writer
[1;36m|[m [35m|[m[1;36m\[m  
[1;36m|[m [35m|[m[1;36m/[m  
[1;36m|[m[1;36m/[m[35m|[m   
* [35m|[m [33m3b99ab7[m Feature/parse unknown tag (#31)
[36m|[m * [33m5e79ec6[m コメント追記
[36m|[m * [33m6e9f7c4[m 同上
[36m|[m * [33m0b1fa59[m 同上
[36m|[m * [33m21c5381[m logの標準出力
[36m|[m * [33mfcacc63[m Linuxでビルド通らないバグを修正中
[36m|[m * [33ma138d80[m 同上
[36m|[m * [33mfae520d[m 同上
[36m|[m * [33m1500ada[m objWriterのデバッグ情報少し追加
[36m|[m * [33mc8df12e[m objWriterのログメッセージをDLLの利用者に渡すようにしました。
[36m|[m * [33m852ec27[m plateau_obj_writer_get_reference_pointの戻り値をAPIResultに
[36m|[m * [33mcff8a54[m obj_writer_cで戻り値がvoidのものをAPIResultに変更
[36m|[m * [33m85ec75d[m plateau_obj_writer_set_mesh_granularityの戻り値をAPIResultに変更
[36m|[m * [33m8fb26a9[m plateau_obj_writer_writeの戻り値をAPIResultに変更
[36m|[m * [33m0179672[m plateau_create_obj_writerの戻り値をAPIResultに変更
[36m|[m [36m|[m * [33mcee26e7[m[33m ([m[1;31morigin/feature/parse_unknown_tag[m[33m)[m Update submodule
[36m|[m [36m|[m * [33m81df042[m for libcitygml update
[36m|[m [36m|[m * [33m72005e3[m for libcitygml update
[36m|[m [36m|[m * [33mcae264b[m for libcitygml update
[36m|[m [36m|[m *   [33m8fbe3ea[m Merge branch 'feature/parse_unknown_tag' of https://github.com/Synesthesias/libplateau into feature/parse_unknown_tag
[36m|[m [36m|[m [1;32m|[m[1;33m\[m  
[36m|[m [36m|[m [1;32m|[m * [33m0923fa0[m for libcitygml update, after rebase.
[36m|[m [36m|[m [1;32m|[m * [33m10f656f[m for libcitygml update, after rebase.
[36m|[m [36m|[m [1;32m|[m * [33m066babe[m for libcitygml update3
[36m|[m [36m|[m [1;32m|[m * [33m9842feb[m for libcitygml update2
[36m|[m [36m|[m [1;32m|[m * [33m0103c82[m for libcitygml update
[36m|[m [36m|[m * [1;33m|[m [33m44ff8da[m for libcitygml update
[36m|[m [36m|[m[36m/[m [1;33m/[m  
[36m|[m[36m/[m[36m|[m [1;33m|[m   
* [36m|[m [1;33m|[m [33mab155ea[m Visual Studioのデバッガ表示をUTF-8に変更
[36m|[m[36m/[m [1;33m/[m  
* [1;33m|[m [33m9530b06[m IdからCityObjectを検索するMapを実装しました。 (#30)
[1;33m|[m [1;33m|[m * [33mb01755c[m[33m ([m[1;32mfeature/id_to_obj2[m[33m)[m libcitygmlをmasterにマージ
[1;33m|[m [1;33m|[m * [33m18c340d[m 余計なコメントを削除
[1;33m|[m [1;33m|[m * [33mb9d61ec[m 余計なファイルを削除
[1;33m|[m [1;33m|[m * [33m544ca17[m 同上
[1;33m|[m [1;33m|[m * [33m7984433[m メソッドリネーム
[1;33m|[m [1;33m|[m * [33m7be4912[m libxerces-c.aをCIの成果物から除外
[1;33m|[m [1;33m|[m * [33m7cda7c6[m IdからCityObjectを検索できるMapをCityModelに実装しました。
[1;33m|[m [1;33m|[m * [33mb9df83d[m C# StrToUtf8Byte修正
[1;33m|[m [1;33m|[m[1;33m/[m  
[1;33m|[m[1;33m/[m[1;33m|[m   
* [1;33m|[m [33m27bc5a1[m UbuntuのUnityに対応、codelistがないときにクラッシュする問題を解決 (#29)
[1;34m|[m [1;33m|[m *   [33m8e4b8af[m[33m ([m[1;32mfeature/unity_str[m[33m)[m Merge branch 'main' into feature/unity_str
[1;34m|[m [1;33m|[m [1;35m|[m[1;34m\[m  
[1;34m|[m [1;33m|[m[1;34m_[m[1;35m|[m[1;34m/[m  
[1;34m|[m[1;34m/[m[1;33m|[m [1;35m|[m   
* [1;33m|[m [1;35m|[m [33m355332b[m Feature/add mesh granularity (#27)
[1;36m|[m [1;33m|[m * [33me862aac[m shared library構築
[1;36m|[m [1;33m|[m * [33mbdf1b43[m 同上
[1;36m|[m [1;33m|[m * [33me556f9f[m linuxでsharedとstatic両方ビルドできるように
[1;36m|[m [1;33m|[m * [33m6c8cc02[m CIでwindowsのlibをダウンロードできるように
[1;36m|[m [1;33m|[m * [33m9e42ba3[m c_wrapperをsrc内に移動し、出力されるdllはplateau_cではなくplateauにしました。
[1;36m|[m [1;33m|[m [1;36m|[m * [33m722a11d[m[33m ([m[1;32mfeature/id_to_obj[m[33m)[m IdからCityObjectを検索できるMapをCityModelに実装しました。
[1;36m|[m [1;33m|[m [1;36m|[m * [33m400e298[m C# StrToUtf8Byte修正
[1;36m|[m [1;33m|[m [1;36m|[m[1;36m/[m  
[1;36m|[m [1;33m|[m * [33m5c3a727[m 余計なコメントを削除
[1;36m|[m [1;33m|[m * [33m9428c71[m action整理
[1;36m|[m [1;33m|[m * [33m1e75c49[m CMakeコード整理
[1;36m|[m [1;33m|[m * [33m6385b92[m なんかうまくいかないので元に戻す
[1;36m|[m [1;33m|[m * [33mdfc32d6[m Revert "同上"
[1;36m|[m [1;33m|[m * [33m2e08229[m 同上
[1;36m|[m [1;33m|[m * [33mc95c391[m 同上
[1;36m|[m [1;33m|[m * [33m927d2a3[m 同上
[1;36m|[m [1;33m|[m * [33md116f89[m 同上
[1;36m|[m [1;33m|[m * [33m08946cf[m 同上
[1;36m|[m [1;33m|[m * [33m69f9919[m CMake整理
[1;36m|[m [1;33m|[m * [33m3a71c98[m 同上
[1;36m|[m [1;33m|[m * [33m808da67[m CMakeListsにコメントを追記
[1;36m|[m [1;33m|[m * [33m5acb39a[m readme追記
[1;36m|[m [1;33m|[m * [33m9a71c25[m readme更新
[1;36m|[m [1;33m|[m * [33m9cc39f9[m readme更新
[1;36m|[m [1;33m|[m * [33m2c7a0f7[m 同上
[1;36m|[m [1;33m|[m * [33m0c39407[m 同上
[1;36m|[m [1;33m|[m * [33m864c5fd[m 同上
[1;36m|[m [1;33m|[m * [33m28f93dd[m 同上
[1;36m|[m [1;33m|[m * [33m1f47149[m 同上
[1;36m|[m [1;33m|[m * [33m4b08d6d[m 同上
[1;36m|[m [1;33m|[m * [33me3c24bf[m 同上
[1;36m|[m [1;33m|[m * [33mbcfd555[m 同上
[1;36m|[m [1;33m|[m * [33ma3eca51[m 同上
[1;36m|[m [1;33m|[m * [33m75ea620[m 同上
[1;36m|[m [1;33m|[m * [33m3a05bb0[m ubuntu18でビルドが通らない問題
[1;36m|[m [1;33m|[m * [33m16cb1b5[m Revert "filesystemがubuntu18で通らない問題"
[1;36m|[m [1;33m|[m * [33m9a7a484[m filesystemがubuntu18で通らない問題
[1;36m|[m [1;33m|[m * [33m04ea8e9[m 同上
[1;36m|[m [1;33m|[m * [33m1934a22[m 同上
[1;36m|[m [1;33m|[m * [33mb1de36f[m action.yml修正
[1;36m|[m [1;33m|[m * [33maac2f55[m github actions ubuntuバージョンを合わせる、readme追記
[1;36m|[m [1;33m|[m * [33m54e39ff[m 同上
[1;36m|[m [1;33m|[m * [33m3895e15[m 出力されるdllを一本化、readme更新
[1;36m|[m [1;33m|[m * [33m1f3d36c[m コード整理
[1;36m|[m [1;33m|[m * [33m455eff6[m codelistが見つからないときにその旨のエラーを表示するようにしました。
[1;36m|[m [1;33m|[m * [33mc8917ad[m Codelistsの見つからない時にUnityが落ちる問題を修正中です。
[1;36m|[m [1;33m|[m * [33mc89401e[m DLLとの文字列のやりとりで空文字を渡すとバグる問題を修正
[1;36m|[m [1;33m|[m[1;36m/[m  
[1;36m|[m[1;36m/[m[1;33m|[m   
[1;36m|[m [1;33m|[m *   [33me877e05[m[33m ([m[1;31morigin/feature/add_mesh_granularity[m[33m)[m Merge branch 'main' of https://github.com/Synesthesias/libplateau into feature/add_mesh_granularity
[1;36m|[m [1;33m|[m [32m|[m[1;36m\[m  
[1;36m|[m [1;33m|[m[1;36m_[m[32m|[m[1;36m/[m  
[1;36m|[m[1;36m/[m[1;33m|[m [32m|[m   
* [1;33m|[m [32m|[m [33m4cbb7dc[m UTF8化、CI強化、dotnetバージョン変更 (#25)
[33m|[m [1;33m|[m * [33m974692d[m add submodule 3rdparty/xerces-c
[33m|[m [1;33m|[m *   [33ma226d9f[m add submodule 3rdparty/libcitygml
[33m|[m [1;33m|[m [34m|[m[33m\[m  
[33m|[m [1;33m|[m[33m_[m[34m|[m[33m/[m  
[33m|[m[33m/[m[1;33m|[m [34m|[m   
* [1;33m|[m [34m|[m [33m76e6ffe[m コードリストからパースされた属性データが空文字列になるバグの修正 (#28)
[35m|[m [1;33m|[m * [33m76fde2f[m add mesh granularity for c++ and c wrapper, again
[35m|[m [1;33m|[m * [33m7627af2[m add submodule
[35m|[m [1;33m|[m *   [33m65c4c50[m Merge branch 'main' of https://github.com/Synesthesias/libplateau into feature/add_mesh_granularity
[35m|[m [1;33m|[m [36m|[m[35m\[m  
[35m|[m [1;33m|[m[35m_[m[36m|[m[35m/[m  
[35m|[m[35m/[m[1;33m|[m [36m|[m   
* [1;33m|[m [36m|[m [33m873238c[m Update pull_request_template.md
[1;31m|[m [1;33m|[m * [33med85c74[m add mesh granularity for c# wrapper
[1;31m|[m [1;33m|[m * [33m6beb770[m add mesh granularity for c++ and c wrapper
[1;31m|[m [1;33m|[m * [33m431998c[m update submodules
[1;31m|[m [1;33m|[m[1;33m/[m  
[1;31m|[m [1;33m|[m * [33m87fc20c[m[33m ([m[1;33mtag: test-tag-3[m[33m, [m[1;32mfeature/dotnet_standard_2.0[m[33m)[m 同上
[1;31m|[m [1;33m|[m * [33m50b7bb5[m[33m ([m[1;33mtag: test-2[m[33m)[m github actionsのWindowsDLL出力にlibも含めるように
[1;31m|[m [1;33m|[m * [33m5c6f266[m[33m ([m[1;33mtag: test-tag[m[33m)[m タグ作成時にgithub workflowを実行:
[1;31m|[m [1;33m|[m * [33m1ead3e6[m workflows/upload-dlls.ymlを作成
[1;31m|[m [1;33m|[m * [33m5993380[m 同上
[1;31m|[m [1;33m|[m * [33m1481025[m 同上
[1;31m|[m [1;33m|[m * [33m4b3f541[m github actionのビルド部分を別ファイルに切り出し
[1;31m|[m [1;33m|[m * [33m45fff3b[m 余計なコメントの削除
[1;31m|[m [1;33m|[m * [33mb716a32[m readme追記
[1;31m|[m [1;33m|[m *   [33mb852928[m Merge branch 'main' into feature/dotnet_standard_2.0
[1;31m|[m [1;33m|[m [1;33m|[m[1;31m\[m  
[1;31m|[m [1;33m|[m[1;31m_[m[1;33m|[m[1;31m/[m  
[1;31m|[m[1;31m/[m[1;33m|[m [1;33m|[m   
* [1;33m|[m [1;33m|[m [33m4139663[m PRテンプレート追加 (#24)
[1;34m|[m [1;33m|[m * [33m1958962[m 余計なコメントを削除
[1;34m|[m [1;33m|[m * [33m1175276[m C#ビルドバグ修正
[1;34m|[m [1;33m|[m * [33m4102dd3[m マージに伴うバグ修正
[1;34m|[m [1;33m|[m * [33m8dff1ae[m updated libcitygml
[1;34m|[m [1;33m|[m *   [33m4f33281[m merge main
[1;34m|[m [1;33m|[m [1;35m|[m[1;34m\[m  
[1;34m|[m [1;33m|[m[1;34m_[m[1;35m|[m[1;34m/[m  
[1;34m|[m[1;34m/[m[1;33m|[m [1;35m|[m   
* [1;33m|[m [1;35m|[m [33mf64d6bb[m コードリストのテストデータ追加 (#23)
[1;33m|[m[1;33m/[m [1;35m/[m  
[1;33m|[m * [33m58a96d1[m 同上
[1;33m|[m * [33m02d73db[m 同上
[1;33m|[m * [33mbce7a4f[m 同上
[1;33m|[m * [33ma369377[m 同上
[1;33m|[m * [33m27b215e[m 同上
[1;33m|[m * [33m98331f0[m 同上
[1;33m|[m * [33m543a62a[m 同上
[1;33m|[m * [33mc8265b3[m 同上
[1;33m|[m * [33m93153f4[m 同上
[1;33m|[m * [33m5c61bf3[m 同上
[1;33m|[m * [33m8681d1f[m 同上
[1;33m|[m * [33mdf9e562[m 同上
[1;33m|[m * [33m07abad6[m 同上
[1;33m|[m * [33m960cfa7[m linux,macもCIでdllをアップロードするように
[1;33m|[m * [33m7f57b17[m CIでWindowsのDLLを自動でアップロードする機能を作っています。
[1;33m|[m * [33mb2c25a0[m submodule更新
[1;33m|[m * [33m85aa704[m 同上
[1;33m|[m * [33m8dcea47[m 同上
[1;33m|[m * [33m55723d1[m submodule更新
[1;33m|[m * [33m08e0de2[m 同上
[1;33m|[m * [33mf079412[m 同上
[1;33m|[m * [33mf255839[m 同上
[1;33m|[m * [33mddfc72f[m codelistを自動テストでも導入できるようにしました。
[1;33m|[m * [33m08eee4f[m 同上
[1;33m|[m * [33mec76a2e[m 自動テストが通らないのを修正中
[1;33m|[m * [33m67247d8[m libplateauのC++側でgmlのパース結果をUTF-8で保持するようにしたのに対応中です。
[1;33m|[m * [33m331f052[m dllから文字列pointerを読むときに結果がnullだとUnityEditorごとクラッシュするのでnullの時は空文字に変更
[1;33m|[m * [33mccdf08f[m obj_writer.cppでマテリアルファイルの書き込み後に close していないミスを修正
[1;33m|[m * [33m644d8e9[m 自動テストで1つ失敗しても他のmatrixは継続するようにしました
[1;33m|[m * [33m492b8cd[m README微修正
[1;33m|[m * [33md82b203[m やっぱりTargetFrameworkをnet standard 2.0 に変更
[1;33m|[m * [33m49d55a4[m 自動テストが通らない問題を解決中
[1;33m|[m * [33m1eec600[m Unityのdotnetバージョンに合わせるため dotnet6.0 から 4.6に移行
[1;33m|[m[1;33m/[m  
* [33m8e88f1b[m テクスチャ、テクスチャマッピングのC,C#ラッパーを実装しました。 (#22)
[1;36m|[m * [33m86091eb[m[33m ([m[1;32mfeature_city_object_c_wrapper_3[m[33m)[m テストが通らない問題を調査中
[1;36m|[m * [33m516db87[m 微修正
[1;36m|[m * [33m65af691[m 前回のプルリクからの変更をこのコミットにまとめます
[1;36m|[m[1;36m/[m  
* [33mf8495c7[m C,C#ラッパーの共通処理をまとめて書きやすくしました。Address,Geometry,Polygon,LinearRingのC,C#ラッパーを実装しました。(テクスチャ・マテリアルを除きます) (#21)
* [33ma28d563[m skip LOD0 Geometry. (#20)
* [33m20c410f[m (2版) object.cpp で属性を取得・設定する機能をDLLに公開しました。 (#18)
* [33mc98f81a[m Feature/python wrapper (#16)
[31m|[m * [33m58b1f74[m[33m ([m[1;32mexperimental/wchar[m[33m)[m 実験的　charをwchar化中
[31m|[m * [33m0a312d0[m デバッグ用にbyte列を出力
[31m|[m * [33md4f3783[m 日本語版でないWindowsで文字化けになるのを抑止するため、C++ソースファイルの文字コードをUTF-8に変換し、コンパイル時も UTF-8として解釈されるように設定しました。
[31m|[m * [33mc4c42b5[m c#側でAttributeValueクラスの基礎を作成
[31m|[m * [33m9efc34d[m AttributesMapから値を文字列で取得する試作
[31m|[m * [33mbfbe613[m namespace変更
[31m|[m * [33md620471[m コードのディレクトリを移動
[31m|[m * [33m95385d5[m コード整理
[31m|[m * [33me0fe252[m 文字列の配列をDLLから取得する機能をユーティリティクラスにまとめました。
[31m|[m * [33mac81aaa[m コード整理
[31m|[m * [33mf9c72cf[m 同上
[31m|[m * [33m397e454[m AttributesDictionary.csの一部分だけをunsafeからsafeにしました。
[31m|[m * [33m597ce23[m Ubuntuでのエラーを修正中です
[31m|[m * [33m4c33511[m plateau_attributes_map_get_keysを仮実装しました。
[31m|[m * [33m5efc870[m plateau_atributes_map_get_key_count, 同get_key_sizes を実装しました。
[31m|[m * [33mb0a9af8[m 同上
[31m|[m * [33m960392d[m チーム共有のフォーマットルール（sln.DotSettings）を導入しました。
[31m|[m * [33mf359533[m CityObject.GeometriesCount のC,C#側実装とテストを書きました。
[31m|[m * [33m0a06d61[m FeatureObject.SetEnvelope(), GetEnvelope() をC,C# で書いてテストを通しました。
[31m|[m * [33m98d05bd[m FeatureObject.GetEnvelope()について失敗するテストを記載
[31m|[m * [33m4fd0767[m ObjectのSetAttributeのoverwriteオプションが動作するようにしました
[31m|[m * [33m619b34d[m コードの細かい部分を修正しました。
[31m|[m * [33m57f0870[m git上で不要なユーザー設定を削除してgitignoreに追加
[31m|[m *   [33m79e2ec5[m Merge branch 'main' into feature/object_c_wrapper
[31m|[m [33m|[m[31m\[m  
[31m|[m [33m|[m[31m/[m  
[31m|[m[31m/[m[33m|[m   
* [33m|[m   [33mceed51b[m Merge pull request #17 from Synesthesias/feature/introduce_github_actions2
[35m|[m[36m\[m [33m\[m  
[35m|[m [36m|[m * [33mc40503b[m コメント追記
[35m|[m [36m|[m * [33me28cae9[m GetAttributesでセパレーターがkey, valueと被った場合のエラー処理を追加しました。
[35m|[m [36m|[m * [33mf16b1d7[m plateau_object_get_keys_valuesでseparatorを任意の文字列に設定できるようにしました。
[35m|[m [36m|[m * [33m98b440f[m libcitygmlにバグを発見したので、バグ発生箇所を回避しました（属性設定時のoverwriteが動作しないので利用しない）
[35m|[m [36m|[m * [33mc6b9da4[m 失敗テストを1つ解決
[35m|[m [36m|[m * [33mfea0e03[m 失敗するテストを記述
[35m|[m [36m|[m * [33mc54d621[m テスト用の記述を削除
[35m|[m [36m|[m * [33mf3bcf6a[m 属性のキーとバリューの一覧を取得する機能を仮実装してDLLにエクスポーズしてテストを書きました。
[35m|[m [36m|[m * [33mff6bb72[m 存在しない属性でgetした場合の処理を追記
[35m|[m [36m|[m * [33m0f09c81[m Ubuntuで出ているエラーを解消中
[35m|[m [36m|[m * [33m4ceddad[m ubuntu向けには strcpy_s 関数がなかったのでstrcpyに置き換えました
[35m|[m [36m|[m * [33mc643e6d[m バッファサイズが少ない場合のハンドリングを追加
[35m|[m [36m|[m * [33m2eebbd5[m plateau_object_get_attributeの余計なnewを排除し、StringBufferで渡す形式にしました。
[35m|[m [36m|[m * [33mba55c95[m plateau_object_set_attributeでの余計なnewを排除
[35m|[m [36m|[m * [33m8c40056[m 微修正
[35m|[m [36m|[m * [33mf00e2a9[m 仮実装でとりあえず基本的なテストが通る段階
[35m|[m [36m|[m * [33m2cbefdf[m Riderで既存のテストを通すための調整
[35m|[m [36m|[m * [33m3360c22[m Objectのラッパーを実装中です
[35m|[m [36m|[m * [33m95f9e6c[m Merge branch 'main' into feature/object_c_wrapper
[35m|[m [36m|[m[36m/[m[35m|[m 
[35m|[m [36m|[m[35m/[m  
[35m|[m[35m/[m[36m|[m   
* [36m|[m [33me65359f[m CityObjectの一部C#ラッパー実装 (#15)
[1;32m|[m * [33m859fc3c[m コードの余計な行を削除しました
[1;32m|[m * [33mf25b944[m 同上
[1;32m|[m * [33m6411437[m 同上
[1;32m|[m * [33maa978bb[m ワークフローステータスバッジを導入
[1;32m|[m * [33mcfe5050[m 自動テストをMacに対応中です。
[1;32m|[m * [33mb301325[m 自動テストをMacに対応中です。
[1;32m|[m * [33m6120c67[m 自動テストをMacに対応中です。
[1;32m|[m * [33m2efa1de[m 自動テストをMacに対応中です。
[1;32m|[m * [33mac79db5[m 自動テストをMacに対応中です。
[1;32m|[m * [33mce73b60[m 自動テストをMacに対応中です。
[1;32m|[m * [33m3952418[m 自動テストをMacに対応中です。
[1;32m|[m * [33m407f336[m 同上
[1;32m|[m * [33m60460ff[m 同上
[1;32m|[m * [33m8553752[m 自動テストをUbuntuに対応中です
[1;32m|[m * [33m80a5876[m 同上
[1;32m|[m * [33m6171d21[m cmake バージョン3.13 でcmakeを構成できない問題を修正しました。
[1;32m|[m * [33mdff1cb8[m 同上
[1;32m|[m * [33m9be3254[m 同上
[1;32m|[m * [33m47da494[m 同上
[1;32m|[m * [33mf04900b[m 同上
[1;32m|[m * [33m7bcdff4[m 同上
[1;32m|[m * [33mcd53dbe[m 同上
[1;32m|[m * [33mf91242b[m 同上
[1;32m|[m * [33m0deb5a3[m 自動テスト導入を試験中
[1;32m|[m * [33me05a638[m 自動テスト導入を試験中
[1;32m|[m * [33m53119a3[m 自動テスト導入を試験中
[1;32m|[m * [33mccf8b3e[m 自動テスト導入を試験中
[1;32m|[m * [33m6b6c0f5[m 自動テスト導入を試験中
[1;32m|[m * [33m5ec88fb[m 自動テスト導入を試験中
[1;32m|[m * [33mbc85d3f[m 自動テスト導入を試験中
[1;32m|[m * [33mada64ec[m 自動テスト導入を試験中
[1;32m|[m * [33m677449a[m 同上
[1;32m|[m * [33m84ca6bf[m 同上
[1;32m|[m * [33m291e4b9[m 同上
[1;32m|[m * [33mb7b8546[m 同上
[1;32m|[m * [33m06f8b86[m 同上
[1;32m|[m * [33m083d25f[m 同上
[1;32m|[m * [33mfe517d3[m 同上
[1;32m|[m * [33mae44ec1[m 同上
[1;32m|[m * [33m65dcada[m 同上
[1;32m|[m * [33m2f63e9b[m 同上
[1;32m|[m * [33m04bb4aa[m 同上
[1;32m|[m * [33m76c14ae[m 同上
[1;32m|[m * [33me9e8637[m 同上
[1;32m|[m * [33m11df774[m 同上
[1;32m|[m * [33m3694bf5[m 同上
[1;32m|[m * [33m6ae9360[m 同上
[1;32m|[m * [33me834aae[m 同上
[1;32m|[m * [33m9d3574f[m 同上
[1;32m|[m * [33m51df57d[m 同上
[1;32m|[m * [33m6c83a12[m 同上
[1;32m|[m * [33ma9feabd[m 同上
[1;32m|[m * [33mc996a03[m クロスプラットフォームのビルドを試験中
[1;32m|[m * [33m75f5f35[m コンパイラオプション fPIC を付けるかどうかコンパイラによって分岐
[1;32m|[m * [33m3dee42c[m 同上
[1;32m|[m * [33m23a63c5[m 同上
[1;32m|[m * [33m9f9fa91[m ubuntuでビルドエラーになる箇所を対応
[1;32m|[m * [33m1ad5144[m 同上11
[1;32m|[m * [33m27a74d6[m 同上10
[1;32m|[m * [33mc97add2[m 同上9
[1;32m|[m * [33m2ef3679[m 同上8
[1;32m|[m * [33m7fd16bf[m 同上7
[1;32m|[m * [33m67fc846[m 同上6
[1;32m|[m * [33m0928da1[m 同上5
[1;32m|[m * [33mf9d037c[m 同上5
[1;32m|[m * [33mdd833dc[m 同上4
[1;32m|[m * [33mb883491[m 同上3
[1;32m|[m * [33m5e192d5[m 同上2
[1;32m|[m * [33m1eef3e1[m 同上
[1;32m|[m * [33m3ebf4dd[m yml改善中
[1;32m|[m * [33m00049c5[m .github/workflow/cmake.ymlを仮組み
[1;32m|[m[1;32m/[m  
* [33mec55895[m Fix cmake error
* [33m86c4818[m Fix Debug build error (#14)
*   [33m20598a0[m Merge pull request #13 from Synesthesias/feat/c_wrapper_macro
[1;33m|[m[1;34m\[m  
[1;33m|[m * [33m06f523c[m マクロの書き方を自動インデントがずれない形式にしました。
[1;33m|[m * [33mf34adfe[m APIのC実装でtry-catchのコピペだった部分をマクロに置き換えました。
[1;33m|[m[1;33m/[m  
*   [33m3e49e2f[m Merge pull request #12 from Synesthesias/feat/expose_writer_config_func
[1;35m|[m[1;36m\[m  
[1;35m|[m * [33m28cc6ee[m プルリクでご指摘のあった点を修正しました。メソッドconst化とメソッド名ミス修正です。
[1;35m|[m * [33m4e591b6[m p/invokeで enumをintで渡していたのをenumで渡すように変更
[1;35m|[m * [33m3365461[m get_dest_axesを実装、dllにexposeしてgetとsetでの一致を確認するテストを実装
[1;35m|[m * [33m7966680[m get_merge_mesh_flg関数を作り、setとgetで値が一致するテストを書きました。
[1;35m|[m * [33ma278796[m setDestAxes, setMergeMeshFlg メソッドをエクスポーズしました。
[1;35m|[m[1;35m/[m  
* [33m14566ec[m Feature/csharp (#11)
* [33m2b332da[m Feature/add lod1 output (#10)
* [33m03beb8b[m first commit (#9)
* [33me2ec995[m add reference point functions (#8)
* [33ma576880[m add axes conversion function (#7)
* [33m110129c[m Feature/sample data (#6)
* [33m7bad941[m add merge mesh option (#5)
*   [33mdbb2f7d[m Merge pull request #4 from Synesthesias/feature/obj_write_with_texture
[31m|[m[32m\[m  
[31m|[m * [33m4ebf2fc[m output .obj file with texture 2
[31m|[m * [33m2aaac0f[m output .obj file with texture
[31m|[m[31m/[m  
*   [33mf88452d[m Merge pull request #3 from Synesthesias/feature/obj_write
[33m|[m[34m\[m  
[33m|[m * [33m6cb6643[m Update obj_writer.cpp
[33m|[m * [33mf794f16[m Delete PolarToPlaneCartesian.cpp
[33m|[m * [33mf29cd73[m Delete PolarToPlaneCartesian.h
[33m|[m * [33m12939d0[m reply to change requests 2
[33m|[m * [33md90e6e6[m reply to change requests
[33m|[m * [33me571ddf[m output .obj file without texture
[33m|[m * [33m824f3fc[m test for 13111-bldg-147301 again
[33m|[m * [33m07867b1[m test for 13111-bldg-147301
[33m|[m[33m/[m  
* [33mef615bc[m Add obj writer (#1)
* [33m21a26e2[m Update README.md
* [33m5dcd257[m Add project
* [33mf768694[m Initial commit
