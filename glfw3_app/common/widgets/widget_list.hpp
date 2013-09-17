#pragma once
//=====================================================================//
/*!	@file
	@brief	GUI widget_list クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_null.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	GUI WidgetList クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	struct widget_list : public widget {

		typedef widget_list value_type;

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	widget_button パラメーター
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct param {
			plate_param	plate_param_;	///< プレート・パラメーター
			color_param	color_param_;	///< カラー・パラメーター
			text_param	text_param_;	///< テキスト描画のパラメーター
			color_param	color_param_select_;	///< 選択時カラー・パラメーター

			utils::strings	text_list_;	///< テキスト・リスト

			uint32_t	select_pos_;	///< テキスト・リストの選択位置

			bool		drop_box_;		///< ドロップ・ボックスの表示

			bool		open_before_;
			bool		open_;

			param(const std::string& text = "") :
				plate_param_(),
				color_param_(widget_director::default_list_color_),
				text_param_(text, img::rgba8(255, 255), img::rgba8(0, 255),
					vtx::placement(vtx::placement::holizontal::LEFT,
						vtx::placement::vertical::CENTER)),
				color_param_select_(widget_director::default_list_color_select_),
				select_pos_(0), drop_box_(true),
				open_before_(false), open_(false)
			{ }
		};

	private:
		widget_director&	wd_;

		param				param_;

		widget_label*		root_;
		widget_null*		frame_;
		widget_labels		list_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		widget_list(widget_director& wd, const widget::param& bp, const param& p) :
			wd_(wd), widget(bp), param_(p),
			root_(0), frame_(0), list_()
			{ }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		virtual ~widget_list() { destroy(); }


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
