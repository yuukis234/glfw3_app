#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget ターミナル（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "utils/terminal.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI terminal クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_terminal : public widget {

		typedef widget_terminal value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_terminal パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			std::string		font_;			///< ターミナル描画フォント
			uint32_t		font_width_;	///< フォント幅（初期化で設定される）
			uint32_t		font_height_;	///< フォント高
			uint32_t		height_;		///< 行の高さ

			param() : font_("Inconsolata"), font_width_(0), font_height_(16), height_(18) { }
		};

		widget_director&	wd_;

		param				param_;

		utils::terminal		terminal_;
		uint32_t			interval_;

		bool				focus_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_terminal(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p), terminal_(wd.at_keyboard()), interval_(0),
			focus_(false) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_terminal() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	widget 型の基本名称を取得
			@return widget 型の基本名称
		*/
		//-----------------------------------------------------------------//
		const char* type_name() const { return "terminal"; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ハイブリッド・ウィジェットのサイン
			@return ハイブリッド・ウィジェットの場合「true」を返す。
		*/
		//-----------------------------------------------------------------//
		bool hybrid() const { return true; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得(ro)
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		const param& get_local_param() const { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	個別パラメーターへの取得
			@return 個別パラメーター
		*/
		//-----------------------------------------------------------------//
		param& at_local_param() { return param_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	１文字出力
			@param[in]	wch	文字
		*/
		//-----------------------------------------------------------------//
		void output(wchar_t wch);


		//-----------------------------------------------------------------//
		/*!
			@brief	テキストの出力
			@param[in]	text	テキスト
		*/
		//-----------------------------------------------------------------//
		void output(const std::string& text);


		//-----------------------------------------------------------------//
		/*!
			@brief	初期化
		*/
		//-----------------------------------------------------------------//
		void initialize();


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート
		*/
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	レンダリング
		*/
		//-----------------------------------------------------------------//
		void render();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		*/
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のセーブ
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre);


		//-----------------------------------------------------------------//
		/*!
			@brief	状態のロード
			@param[in]	pre	プリファレンス参照
			@return エラーが無い場合「true」
		*/
		//-----------------------------------------------------------------//
		bool load(const sys::preference& pre);
	};
}

