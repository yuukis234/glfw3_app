//=====================================================================//
/*!	@file
	@brief	日本語 FEP をインターフェースするクラス @n
			Copyright 2017 Kunihito Hiramatsu
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "core/jfep_io.hpp"

namespace input {


// ImmReleaseContext(HWND, himc_);


	//-----------------------------------------------------------------//
	/*!
		@brief	初期化
	 */
	//-----------------------------------------------------------------//
	void jfep_io::initialize()
	{
		himc_ = ImmGetContext(0);


	}



}
