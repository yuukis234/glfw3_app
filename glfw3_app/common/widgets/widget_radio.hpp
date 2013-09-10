#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_radio クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_radio クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_radio : public widget {

		typedef widget_radio value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_check パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			widget::text_param	text_param_;
			float	gray_text_gain_;	///< 不許可時のグレースケールゲイン
			bool	disable_gray_text_;	///< 不許可時、文字をグレースケールする場合
			bool	check_;				///< 許可、不許可の状態
			param(const std::string& text = "") :
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255)),
				gray_text_gain_(0.65f), disable_gray_text_(true),
				check_(false) { }
		};

	private:
		widget_director&	wd_;

		param				param_;

		bool				obj_state_;

		gl::glmobj::handle	ena_h_;
		gl::glmobj::handle	dis_h_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_radio(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p),
			obj_state_(false), ena_h_(0), dis_h_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_radio() { destroy(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	型を取得
		*/
		//-----------------------------------------------------------------//
		type_id type() const { return get_type_id<value_type>(); }


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
			@brief	廃棄
		*/
		//-----------------------------------------------------------------//
		void destroy();

	};

}
