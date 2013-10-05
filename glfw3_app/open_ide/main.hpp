#pragma once
//=====================================================================//
/*! @file
	@brief  player サンプル
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "snd_io/sound.hpp"
#include "widgets/widget_director.hpp"
#include "widgets/widget_filer.hpp"
#include "utils/preference.hpp"

namespace app {

	struct core {

		al::sound	sound_;

		gui::widget_director	widget_director_;
		gui::widget_filer		widget_filer_;

		sys::preference		preference_;

		core() : widget_filer_(widget_director_) { }
	};
}