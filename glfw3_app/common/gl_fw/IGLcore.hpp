#pragma once
//=====================================================================//
/*!	@file
	@brief	Graphics Library コア・インターフェース・クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/device.hpp"
#include "gl_fw/gl_info.hpp"
#include "gl_fw/glfonts.hpp"
#include "utils/file_io.hpp"
#include "utils/string_utils.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	gl_fw Graphics Library のインターフェースクラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class IGLcore {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	仮想デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~IGLcore() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化プロセス
			@param[in]	current_path	カレント・パス
			@return 正常終了したら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool initialize(const std::string& current_path_) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	セットアップ・プロセス
			@param[in]	rect	描画領域と位置
			@param[in]	title	タイトルの設定
			@param[in]	fullscreen	全画面の場合は「ture」
			@return 正常終了したら「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool setup(const vtx::srect& rect, const std::string& title, bool fullscreen) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	最大解像度を取得
			@return	最大解像度
		*/
		//-----------------------------------------------------------------//
		virtual const vtx::spos& get_best_size() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	最小解像度を設定
			@param[in]	size	最小解像度
		*/
		//-----------------------------------------------------------------//
		virtual void set_limit_size(const vtx::spos& size) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	最小解像度を取得
			@return	最小解像度
		*/
		//-----------------------------------------------------------------//
		virtual const vtx::spos& get_limit_size() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	現在の解像度を取得
			@return	解像度
		*/
		//-----------------------------------------------------------------//
		virtual const vtx::spos& get_size() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	現在のスクリーン位置を返す
			@return	位置
		*/
		//-----------------------------------------------------------------//
		virtual const vtx::spos& get_locate() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief  ファイル受信 ID の取得
			@return ファイル受信 ID
		*/
		//-----------------------------------------------------------------//
		virtual int get_recv_file_id() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief  ファイル受信 path の取得
			@return ファイル受信 path
		*/
		//-----------------------------------------------------------------//
		virtual const utils::strings& get_recv_file_path() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	平均フレームレートを得る（標準60Hz）
			@return	平均フレームレート
		*/
		//-----------------------------------------------------------------//
		virtual float get_frame_rate() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	平均 CPU のクロックを得る。
			@return	平均 CPU クロック(GHz)
		*/
		//-----------------------------------------------------------------//
		virtual float get_cpu_clock() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	終了シグナルの受取
			@return	「true」の場合、終了
		*/
		//-----------------------------------------------------------------//
		virtual bool get_exit_signal() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	タイトルの設定
			@param[in]	title	タイトル文字列
		*/
		//-----------------------------------------------------------------//
		virtual void set_title(const std::string& title) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル（マウス・ポインター）の設定
			@param[in]	cursortype	カーソル・タイプ
		*/
		//-----------------------------------------------------------------//
///		virtual void set_cursor(cursor cursortype) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	全画面の設定
			@param[in]	flag	全画面の場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual void full_screen(bool flag) = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	全画面の取得
			@return 全画面の場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool is_full_screen() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	毎フレーム・サービス
		*/
		//-----------------------------------------------------------------//
		virtual void service() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄プロセス
		*/
		//-----------------------------------------------------------------//
		virtual void destroy() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フレーム・フリップ
		*/
		//-----------------------------------------------------------------//
		virtual void flip_frame() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	カレント・パスを取得
			@return カレント・パス
		*/
		//-----------------------------------------------------------------//
		virtual const std::string& get_current_path() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・クラスを受け取る。
			@return デバイス・クラス（const）
		*/
		//-----------------------------------------------------------------//
		virtual const device& get_device() const = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・クラスを参照
			@return デバイス・クラス（const）
		*/
		//-----------------------------------------------------------------//
		virtual device& at_device() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	フォント・クラスを参照
			@return フォント・クラス
		*/
		//-----------------------------------------------------------------//
		virtual fonts& at_fonts() = 0;


		//-----------------------------------------------------------------//
		/*!
			@brief	日本語キーボードの検査
			@return 日本語キーボードの場合「true」
		*/
		//-----------------------------------------------------------------//
		virtual bool keyboard_japan() const = 0;
	};


	//-----------------------------------------------------------------//
	/*!
		@brief	コアの生成
	*/
	//-----------------------------------------------------------------//
	void create_glcore();


	//-----------------------------------------------------------------//
	/*!
		@brief	コアのインターフェースクラスを得る
		@return コアのインターフェースクラス
	*/
	//-----------------------------------------------------------------//
	IGLcore* get_glcore();


	//-----------------------------------------------------------------//
	/*!
		@brief	コアの廃棄
	*/
	//-----------------------------------------------------------------//
	void destroy_glcore();

}

