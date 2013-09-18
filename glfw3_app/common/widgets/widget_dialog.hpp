#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_dialog クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_dialog クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_dialog : public widget {

		typedef widget_dialog value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_dialog パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param		plate_param_;
			color_param		color_param_;

			struct style {
				enum type {
					OK,			///< OK ボタンのみ
					CANCEL_OK,	///< キャンセル、OKボタン
				};
			};
			style::type		style_;		///< ダイアログ・スタイル
			vtx::srect		text_area_;	///< テキスト表示エリア

			bool			return_ok_;
			bool			return_cancel_;

			param() : plate_param_(),
				color_param_(widget_director::default_dialog_color_),
				style_(style::OK),
				text_area_(vtx::spos(8), vtx::spos(0)),
				return_ok_(false), return_cancel_(false)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::glmobj::handle	objh_;

		widget_text*		text_;
		widget_button*		ok_;
		widget_button*		cancel_;

		void destroy_();
	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_dialog(widget_director& wd, const widget::param& wp, const param& p) :
			wd_(wd), widget(wp), param_(p), objh_(0),
			text_(0), ok_(0), cancel_(0)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_dialog() { destroy_(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


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
			@brief	有効・無効の設定
			@param[in]	f	無効にする場合「false」
		*/
		//-----------------------------------------------------------------//
		void enable(bool f = true) { wd_.enable(this, f, true); }


		//-----------------------------------------------------------------//
		/*!
			@brief	文書の設定
			@param[in]	text	文書
		*/
		//-----------------------------------------------------------------//
		void set_text(const std::string& text);


		//-----------------------------------------------------------------//
		/*!
			@brief	文書の取得
			@return	文書
		*/
		//-----------------------------------------------------------------//
		const std::string& get_text() const {
			if(text_) {
				return text_->get_local_param().text_param_.text_;
			} else {
				static std::string s;
				return s;
			}
		}


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
	};
}
