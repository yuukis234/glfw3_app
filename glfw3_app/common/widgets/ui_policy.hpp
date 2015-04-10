#pragma once
//=====================================================================//
/*!	@file
	@brief	ユーザー・インターフェース・ポリシー（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "widgets/widget_director.hpp"

namespace gui {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	ユーザーインターフェース（操作）ポリシー 
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class ui_policy {
		gui::widget_director& wd_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		ui_policy(gui::widget_director& wd) : wd_(wd) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	デストラクター
		*/
		//-----------------------------------------------------------------//
		~ui_policy() { }


		//-----------------------------------------------------------------//
		/*!
			@brief	更新
		*/
		//-----------------------------------------------------------------//
		void update() {

		}
	};
}
