#pragma once
//=====================================================================//
/*!	@file
	@brief	Graphics Library Core (glfw3)
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <string>
#include <stdint.h>
#include "gl_fw/IGLcore.hpp"
#include "device.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Graphics Core (glfw3) Library クラス@n
				※ハードウェアー依存の操作を扱う部分を集積したクラス。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class glcore : public IGLcore {
	public:
		static IGLcore* glcore_;
		static device::bitsets	bitsets_;
		static device::locator	locator_;
	private:

		std::string		current_path_;

		device			device_;

		fonts			fonts_;

		GLFWwindow*		window_;

		vtx::spos		best_size_;
		vtx::spos		limit_size_;
		vtx::spos		size_;
		vtx::spos		locate_;

		int				recv_file_id_;
		utils::strings	recv_file_path_;

		uint32_t	frame_count_;
		double		frame_ref_;
		double		frame_time_;

		double		machine_cycle_;
		double		cpu_ghz_;
		bool		cpu_spd_enable_;
		bool		swap_ctrl_;

		bool		exit_signal_;
		bool		full_screen_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		glcore() : window_(0),
				   best_size_(0), limit_size_(0), size_(0), locate_(0),
				   recv_file_id_(0),
				   frame_count_(0), frame_ref_(0.0), frame_time_(0.0),
				   machine_cycle_(0.0), cpu_ghz_(0.5),
				   cpu_spd_enable_(false), swap_ctrl_(false),
				   exit_signal_(false), full_screen_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~glcore() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化プロセス
			@param[in]	current_path	カレント・パス
			@return 正常終了したら「true」
		*/
		//-----------------------------------------------------------------//
		bool initialize(const std::string& current_path);


		//-----------------------------------------------------------------//
		/*!
			@brief	最大解像度を返す
			@return	最大解像度
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_best_size() const { return best_size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	最小解像度を設定
			@param[in]	size	最小解像度
		*/
		//-----------------------------------------------------------------//
		void set_limit_size(const vtx::spos& size) { limit_size_ = size; }


		//-----------------------------------------------------------------//
		/*!
			@brief	最小解像度を返す
			@return	最小解像度
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_limit_size() const { return limit_size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	解像度を返す
			@return	解像度
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクリーン位置を返す
			@return	位置
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_locate() const { return locate_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル受信 ID の取得
			@return	ファイル受信 ID
		*/
		//-----------------------------------------------------------------//
		int get_recv_file_id() const { return recv_file_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル受信 path の取得
			@return	ファイル受信 path
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get_recv_file_path() const { return recv_file_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	平均フレームレートを得る（標準60Hz）
			@return	平均フレームレート
		*/
		//-----------------------------------------------------------------//
		float get_frame_rate() const { return (1.0 / frame_time_); }


		//-----------------------------------------------------------------//
		/*!
			@brief	平均 CPU のクロックを得る。
			@return	平均 CPU クロック(GHz)
		*/
		//-----------------------------------------------------------------//
		float get_cpu_clock() const { return cpu_ghz_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	終了シグナルの受取
			@return	「true」の場合、終了
		*/
		//-----------------------------------------------------------------//
		bool get_exit_signal() const { return exit_signal_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	セットアップ・プロセス
			@param[in]	rect	描画領域と位置
			@param[in]	title	タイトルの設定
			@param[in]	fullscreen	全画面の場合は「ture」
			@return 正常終了したら「true」
		*/
		//-----------------------------------------------------------------//
		bool setup(const vtx::srect& rect, const std::string& title, bool fullscreen);


		//-----------------------------------------------------------------//
		/*!
			@brief	タイトルの登録
			@param[in]	title	タイトル文字列
		*/
		//-----------------------------------------------------------------//
		void set_title(const std::string& title);


		//-----------------------------------------------------------------//
		/*!
			@brief	カーソル（マウスポインター）の設定
			@param[in]	cursortype	カーソル・タイプ
		*/
		//-----------------------------------------------------------------//
///		void set_cursor(cursor cursortype) { ::glutSetCursor(cursortype); }


		//-----------------------------------------------------------------//
		/*!
			@brief	全画面の設定
			@param[in]	flag	全画面の場合「true」
		*/
		//-----------------------------------------------------------------//
		void full_screen(bool flag = true);


		//-----------------------------------------------------------------//
		/*!
			@brief	全画面の設定
			@return 全画面なら「true」
		*/
		//-----------------------------------------------------------------//
		bool is_full_screen() const { return full_screen_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントパスを取得
			@return ファイルス・リソース
		*/
		//-----------------------------------------------------------------//
		const std::string& get_current_path() const { return current_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	フレーム・サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	フレーム・フリップ
		*/
		//-----------------------------------------------------------------//
		void flip_frame();


		//-----------------------------------------------------------------//
		/*!
			@brief	廃棄プロセス
		*/
		//-----------------------------------------------------------------//
		void destroy();


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・クラスを受け取る。
			@return デバイス・クラス（const）
		*/
		//-----------------------------------------------------------------//
		const device& get_device() const { return device_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・クラスを参照
			@return デバイス・クラス
		*/
		//-----------------------------------------------------------------//
		device& at_device() { return device_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	デバイス・クラスを参照
			@return デバイス・クラス
		*/
		//-----------------------------------------------------------------//
		fonts& at_fonts() { return fonts_; }
	};
}

