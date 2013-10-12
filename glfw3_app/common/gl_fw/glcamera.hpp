#pragma once
//=====================================================================//
/*!	@file
	@brief	OpenGL カメラ・クラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "gl_fw/gl_info.hpp"
#include "utils/quat.hpp"
#include "gl_fw/glmatrix.hpp"
#include "core/device.hpp"

namespace gl {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	glcamera クラス
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class camera {

		device::key::type	translate_key_;
		device::key::type	rotate_key_;
		device::key::type	zoom_key_;

		vtx::spos	mouse_pos_;
		vtx::spos	mouse_left_first_pos_;
		vtx::fvtx	eye_first_;
		vtx::fvtx	target_first_;
		vtx::fvtx	up_first_;

		vtx::fpos	quat_handle_;
		vtx::fpos	quat_handle_first_;
		qtx::fquat	quat_;

		matrixf		glmat_;

		vtx::fpos	size_;
		float		aspect_;
		float		fov_;
		float		z_near_;
		float		z_far_;
		vtx::fvtx	eye_;
		vtx::fvtx	target_;
		vtx::fvtx	up_;

		bool		touch_;

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		 */
		//-----------------------------------------------------------------//
		camera() :
			translate_key_(device::key::CONTROL),
			rotate_key_(device::key::SHIFT),
			zoom_key_(device::key::ALT),
			mouse_pos_(0), mouse_left_first_pos_(0),
			quat_handle_(0.0f), quat_handle_first_(0.0f), quat_(), glmat_(),
			size_(0.0f), aspect_(1.0f), fov_(45.0f), z_near_(5.0f), z_far_(5000.0f),
			eye_(0.0f, -15.0f, 6.0f),
			target_(0.0f, 0.0f, 0.0f),
			up_(0.0f, 1.0f, 0.0f), touch_(false) { glmat_.initialize(); }


		//-----------------------------------------------------------------//
		/*!
			@brief	update
		 */
		//-----------------------------------------------------------------//
		void update();


		//-----------------------------------------------------------------//
		/*!
			@brief	サービス
		 */
		//-----------------------------------------------------------------//
		void service();


		//-----------------------------------------------------------------//
		/*!
			@brief	サイズ取得
			@return サイズ
		 */
		//-----------------------------------------------------------------//
		const vtx::fpos& get_size() const { return size_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アスペクト取得
			@return アスペクト
		 */
		//-----------------------------------------------------------------//
		float get_aspect() const { return aspect_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	fov 取得
			@return fov
		 */
		//-----------------------------------------------------------------//
		float get_fov() const { return  fov_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	near 取得
			@return near
		 */
		//-----------------------------------------------------------------//
		float get_near() const { return z_near_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	far 取得
			@return far
		 */
		//-----------------------------------------------------------------//
		float get_far() const { return z_far_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	eye 取得
			@return eye
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_eye() const { return eye_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	target 取得
			@return target
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_target() const { return target_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	カメラ上向き取得
			@return カメラ上向き
		 */
		//-----------------------------------------------------------------//
		const vtx::fvtx& get_up() const { return up_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	アップデート中にあった操作
			@return 「true」なら操作有り
		 */
		//-----------------------------------------------------------------//
		bool get_touch() const { return touch_; }
	};

}
