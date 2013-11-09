#pragma once
//=====================================================================//
/*!	@file
	@brief	イメージのインターフェースクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img.hpp"
#include "utils/vtx.hpp"

namespace img {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	イメージ・インターフェース・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class i_img {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~i_img() { };


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを確保する
			@param[in]	size	サイズ
			@param[in]	alpha	アルファ・チャネルを有効にする場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual void create(const vtx::spos& size, bool alpha) = 0;


 		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを設定
			@param[in]	idx	テーブルの位置
			@param[in]	c	設定するカラー
		*/
		//-----------------------------------------------------------------//
		virtual void put_clut(int idx, const rgba8& c) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルを得る
			@param[in]	idx	テーブルの位置
			@param[in]	c	受け取るカラー参照ポイント
		*/
		//-----------------------------------------------------------------//
		virtual void get_clut(int idx, rgba8& c) const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画する IDX カラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool put_pixel(int x, int y, const idx8& c) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool get_pixel(int x, int y, idx8& c) const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画する GRAY カラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool put_pixel(int x, int y, const gray8& c) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool get_pixel(int x, int y, gray8& c) const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージに点を描画
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画する RGBA カラー
			@return 領域なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool put_pixel(int x, int y, const rgba8& c) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージの点を得る
			@param[in]	x	描画位置Ｘ
			@param[in]	y	描画位置Ｙ
			@param[in]	c	描画されたカラーを受け取るリファレンス
		*/
		//-----------------------------------------------------------------//
		virtual bool get_pixel(int x, int y, rgba8& c) const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのタイプを得る。
			@return	イメージタイプ
		*/
		//-----------------------------------------------------------------//
		virtual IMG::type get_type() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのアドレスを得る。
			@return	イメージのポインター
		*/
		//-----------------------------------------------------------------//
		virtual const void* get_image() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージのサイズを得る
			@return	イメージのサイズ
		*/
		//-----------------------------------------------------------------//
		virtual const vtx::spos& get_size() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	カラー・ルック・アップ・テーブルの最大数を返す
			@return	最大数
		*/
		//-----------------------------------------------------------------//
		virtual int get_clut_max() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	アルファ・チャネルが有効か調べる
			@return	アルファ・チャネルが有効なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool test_alpha() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	利用している色数の総数をカウントする
			@return	利用している色数
		*/
		//-----------------------------------------------------------------//
		virtual uint32_t count_color() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	画像が「空」か検査する
			@return	「空」なら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool empty() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	イメージを廃棄する
		*/
		//-----------------------------------------------------------------//
		virtual void destroy() = 0;

	};

};
