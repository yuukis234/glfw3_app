#pragma once
//=====================================================================//
/*! @file
	@brief  Logic Edit クラス @n
			コマンド形式の波形編集
	@author 平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "logic.hpp"

namespace tools {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief  ロジック編集クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class logic_edit {

		logic&	logic_;

	public:
		//-------------------------------------------------------------//
		/*!
			@brief  コンストラクター
		*/
		//-------------------------------------------------------------//
		logic_edit(logic& lg) : logic_(lg) { }






	};

}
