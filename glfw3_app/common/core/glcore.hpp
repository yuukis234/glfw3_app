#pragma once
//=====================================================================//
/*!	@file
	@brief	Graphics Library Core (for glfw3) 
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2017, 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw3_app/blob/master/LICENSE
*/
//=====================================================================//
#include <string>
#include <thread>
#include <future>
#include <unistd.h>
#include "utils/singleton_policy.hpp"
#include "core/device.hpp"
#include "gl_fw/glfonts.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	Graphics Core (glfw3) Library クラス@n
				※ハードウェアー依存の操作を扱う部分を集積したクラス。
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class core : public utils::singleton_policy<core> {

		friend struct utils::singleton_policy<core>;

	public:
		static device::bits_t	bits_;
		static device::locator	locator_;

	private:
		std::string		current_path_;
		std::string		exec_path_;
		utils::strings	command_path_;

		device			device_;

		fonts			fonts_;

		GLFWwindow*		window_;

		vtx::spos		best_size_;
		vtx::spos		limit_size_;
		vtx::spos		size_;
		vtx::srect		rect_;

		vtx::spos		psize_;
		vtx::fpos		dpi_;

		int				recv_files_id_;
		utils::strings	recv_files_path_;

		utils::lstring	recv_text_;

		std::string		title_;

#ifdef __APPLE__
		std::future<uint32_t>	v_sync_wait_;
		uint32_t	sync_count_;
#endif

		uint32_t	frame_count_;
		double		frame_time_;
		double		machine_cycle_;
		double		cpu_ghz_;

		float		scale_;

		bool		cpu_spd_enable_;
		bool		soft_sync_;

		bool		exit_signal_;
		bool		full_screen_;

		bool		keyboard_jp_;

		bool		scaled_;

#ifdef __APPLE__
		static uint32_t wait_sync_task_(uint32_t in) {
			++in;
			usleep(16000);	// 16ms
			return in;
		}

		void start_sync_() {
			v_sync_wait_ = std::async(std::launch::async,
									  wait_sync_task_,
									  sync_count_);
		}

		void wait_sync_() {
			sync_count_ = v_sync_wait_.get();
		}
#endif

		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		core() : window_(0),
				 best_size_(0), limit_size_(0), size_(0), rect_(0), psize_(0), dpi_(0),
				 recv_files_id_(0), recv_files_path_(),
				 title_(),
#ifdef __APPLE__
				 sync_count_(0),
#endif
				 frame_count_(0), frame_time_(0.0),
				 machine_cycle_(0.0), cpu_ghz_(0.0),
				 scale_(1.0f),
				 cpu_spd_enable_(false), soft_sync_(false),
				 exit_signal_(false), full_screen_(false), keyboard_jp_(false),
				 scaled_(false) { }

		core(const core& rhs);
		core& operator = (const core& rhs);

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~core() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化プロセス
			@param[in]	argc	起動パラメーター数
			@param[in]	argv	起動パラメーター
			@return 正常終了したら「true」
		*/
		//-----------------------------------------------------------------//
		bool initialize(int argc, char** argv);


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
			@brief	解像度を返す（実際の大きさ）
			@return	解像度
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクリーンの物理サイズを返す（ｍｍ）
			@return	スクリーンの物理サイズ
		*/
		//-----------------------------------------------------------------//
		const vtx::spos& get_physical_size() const { return psize_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	DPI を返す
			@return	DPI
		*/
		//-----------------------------------------------------------------//
		const vtx::fpos& get_dpi() const { return dpi_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	DPI scale
			@return	DPI scale
		*/
		//-----------------------------------------------------------------//
		float get_dpi_scale() const { return scale_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	スクリーン位置、サイズを返す（論理的な大きさ）
			@return	配置
		*/
		//-----------------------------------------------------------------//
		const vtx::srect& get_rect() const { return rect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	コマンド・パスの取得
			@return	コマンド・パス
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get_command_path() const { return command_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル受信の設定
			@param[in] num	受信ファイル数
			@param[in] path 受信ファイルパス
		*/
		//-----------------------------------------------------------------//
		void set_recv_files(int num, const char** path) {
			++recv_files_id_;
			recv_files_path_.clear();
			for(int i = 0; i < num; ++i) {
				std::string file;
				utils::code_conv(std::string(path[i]), '\\', '/', file);
				recv_files_path_.push_back(file);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル受信 ID の取得
			@return	ファイル受信 ID
		*/
		//-----------------------------------------------------------------//
		int get_recv_files_id() const { return recv_files_id_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ファイル受信 path の取得
			@return	ファイル受信 path
		*/
		//-----------------------------------------------------------------//
		const utils::strings& get_recv_files_path() const { return recv_files_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボード文字列の取得
			@return キーボード文字列
		*/
		//-----------------------------------------------------------------//
		const utils::lstring& get_recv_text() const { return recv_text_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	キーボード文字列の参照
			@return キーボード文字列
		*/
		//-----------------------------------------------------------------//
		utils::lstring& at_recv_text() { return recv_text_; }


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
			@brief	実行パスを取得
			@return ファイル・パス
		*/
		//-----------------------------------------------------------------//
		const std::string& get_exec_path() const { return exec_path_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カレントパスを取得
			@return ファイル・パス
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


		//-----------------------------------------------------------------//
		/*!
			@brief	日本語キーボードか検査
			@return 日本語キーボードの場合「true」が返る
		*/
		//-----------------------------------------------------------------//
		bool keyboard_japan() const { return keyboard_jp_; }

	};
}

