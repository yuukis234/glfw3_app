#pragma once
//=====================================================================//
/*! @file
    @brief  イグナイター・プロジェクト設定クラス
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/RX/blob/master/LICENSE
*/
//=====================================================================//
#include <array>
#include "core/glcore.hpp"
#include "utils/director.hpp"
#include "widgets/widget_dialog.hpp"
#include "widgets/widget_frame.hpp"
#include "widgets/widget_image.hpp"
#include "widgets/widget_button.hpp"
#include "widgets/widget_text.hpp"
#include "widgets/widget_label.hpp"
#include "widgets/widget_spinbox.hpp"
#include "widgets/widget_check.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/input.hpp"
#include "utils/format.hpp"
#include "utils/preference.hpp"
#include "img_io/img_files.hpp"

namespace app {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  イグナイター・プロジェクト設定クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class project {

		utils::director<core>&	director_;

		gui::widget_dialog*		dialog_;

		gui::widget_label*		pbase_;
		gui::widget_label*		pext_;
		gui::widget_label*		pname_[50];
		gui::widget_text*		help_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		project(utils::director<core>& d) : director_(d),
			dialog_(nullptr),
			pbase_(nullptr), pext_(nullptr), pname_{ nullptr }, help_(nullptr)
		{ }


		//-----------------------------------------------------------------//
		/*!
			@brief  ダイアログの取得
			@return ダイアログ
		*/
		//-----------------------------------------------------------------//
		gui::widget_dialog* get_dialog() { return dialog_; }


		//-----------------------------------------------------------------//
		/*!
			@brief  初期化（リソースの構築）
		*/
		//-----------------------------------------------------------------//
		void initialize()
		{
			using namespace gui;
			widget_director& wd = director_.at().widget_director_;

			int w = 1020;
			int h = 670;
			{  // 単体試験設定ダイアログ
				widget::param wp(vtx::irect(120, 120, w, h));
				widget_dialog::param wp_;
				wp_.style_ = widget_dialog::style::OK;
				dialog_ = wd.add_widget<widget_dialog>(wp, wp_);
				dialog_->enable(false);
				{  // 単体試験ベース名設定
					{
						widget::param wp(vtx::irect(20, 20, 100, 40), dialog_);
						widget_text::param wp_("ベース名：");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(20 + 100 + 10, 20, 150, 40), dialog_);
					widget_label::param wp_("", false);
					pbase_ = wd.add_widget<widget_label>(wp, wp_);
				}
				{  // 単体試験拡張子設定
					{
						widget::param wp(vtx::irect(320, 20, 90, 40), dialog_);
						widget_text::param wp_("拡張子：");
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);	
					}
					widget::param wp(vtx::irect(320 + 90 + 10, 20, 150, 40), dialog_);
					widget_label::param wp_(".unt", false);
					pext_ = wd.add_widget<widget_label>(wp, wp_);
				}
				for(int i = 0; i < 50; ++i) {
					int x = (i / 10) * 200;
					int y = 40 + 10 + (i % 10) * 50;
					{
						widget::param wp(vtx::irect(x + 20, y + 20, 50, 40), dialog_);
						std::string no = (boost::format("%d:") % (i + 1)).str();
						widget_text::param wp_(no);
						wp_.text_param_.placement_
							= vtx::placement(vtx::placement::holizontal::LEFT,
											 vtx::placement::vertical::CENTER);
						wd.add_widget<widget_text>(wp, wp_);
					}
					widget::param wp(vtx::irect(x + 60, y + 20, 130, 40), dialog_);
					widget_label::param wp_("", false);
					pname_[i] = wd.add_widget<widget_label>(wp, wp_);
				}
				{
					widget::param wp(vtx::irect(20, h - 100, 200, 40), dialog_);
					widget_text::param wp_;
					wp_.text_param_.placement_
						= vtx::placement(vtx::placement::holizontal::LEFT,
										 vtx::placement::vertical::CENTER);
					help_ = wd.add_widget<widget_text>(wp, wp_);	
				}
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  更新
		*/
		//-----------------------------------------------------------------//
		void update()
		{
			if(dialog_->get_state(gui::widget::state::ENABLE)) {
				std::string s;
				for(int i = 0; i < 50; ++i) {
					if(pname_[i]->get_focus()) {
						if(pname_[i]->get_text().empty()) continue;
						s = pbase_->get_text();
						s += pname_[i]->get_text();
						s += pext_->get_text();
						break;
					}
				}
				help_->set_text(s);
			}
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool save(sys::preference& pre)
		{
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief  セーブ
			@param[in]	pre	プリファレンス（参照）
			@return 正常なら「true」
		*/
		//-----------------------------------------------------------------//
		bool load(sys::preference& pre)
		{
			return true;
		}
	};
}
