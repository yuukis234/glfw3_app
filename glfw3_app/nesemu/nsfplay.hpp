#pragma once
//=====================================================================//
/*! @file
	@brief  NSF Player クラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "main.hpp"

extern "C" {
	#include "nes.h"
	#include "nesinput.h"
	extern const rgb_t* get_palette();
};

namespace emu {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	nesemu シーン・クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class nsfplay {

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-----------------------------------------------------------------//
		nsfplay() { }



	};
}

