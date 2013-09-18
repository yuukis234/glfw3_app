#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI Widget Image クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI widget_image クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_image : public widget {

		typedef widget_image value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_image パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			const img::i_img*		image_;		///< 画像を使う場合
			param(const img::i_img* image = 0) : image_(image) { }
		};

	private:
		widget_director&	wd_;

		param				param_;

		gl::glmobj::handle	objh_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_image(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p), objh_(0) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_image() { }


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
		bool hybrid() const { return false; }


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
	};

}
