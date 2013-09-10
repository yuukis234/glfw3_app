//=====================================================================//
/*!	@file
	@brief	PCM 波形を扱うクラス（ヘッダー）
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "pcm.hpp"
#include <boost/foreach.hpp>

namespace al {

	template <class _T>
	void _copy_pcm(const pcm<_T>& src, pcm<_T>& dst)
	{
		dst.create(src.get_rate(), src.get_samples());
		for(size_t i = 0; i < src.get_samples(); ++i) {
			_T s;
			src.get(i, s);
			dst.put(i, s);
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	オーディオ・インターフェースの複製を作成する
		@param[in]	src	オーディオ・インターフェースのソース
		@return 複製されたオーディオ・インターフェース
	*/
	//-----------------------------------------------------------------//
	i_audio* copy_audio(const i_audio* src)
	{
		i_audio* dst = 0;

		switch(src->get_type()) {
		case audio_format::PCM8_MONO:
			dst = dynamic_cast<i_audio*>(new audio_mno8);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm8_m v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		case audio_format::PCM8_STEREO:
			dst = dynamic_cast<i_audio*>(new audio_sto8);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm8_s v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		case audio_format::PCM16_MONO:
			dst = dynamic_cast<i_audio*>(new audio_mno16);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm16_m v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		case audio_format::PCM16_STEREO:
			dst = dynamic_cast<i_audio*>(new audio_sto16);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm16_s v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		case audio_format::PCM24_MONO:
			break;
		case audio_format::PCM24_STEREO:
			break;
		case audio_format::PCM32_MONO:
			dst = dynamic_cast<i_audio*>(new audio_mno32);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm32_m v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		case audio_format::PCM32_STEREO:
			dst = dynamic_cast<i_audio*>(new audio_sto32);
			dst->create(src->get_rate(), src->get_samples());
			for(size_t i = 0; i < src->get_samples(); ++i) {
				pcm32_s v;
				src->get(i, v);
				dst->put(i, v);
			}
			break;
		default:
			break;
		}
		return dst;
	}

}
