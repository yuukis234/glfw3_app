//=====================================================================//
/*!	@file
	@brief	BDF フォント・ファイルを扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include <iostream>
#include "img_io/bdf_io.hpp"
#include "utils/string_utils.hpp"
#include <boost/lexical_cast.hpp>

namespace img {

	// http://homepage1.nifty.com/docs/algo/j/jis2sjis.html
	// で収集した変換ソースコード
	static unsigned int jis_to_sjis_(unsigned int jis)
	{
		unsigned int hib, lob;

		hib = (jis >> 8) & 0xff;
		lob = jis & 0xff;
		lob += (hib & 1) ? 0x1f : 0x7d;
		if (lob >= 0x7f) lob++;
		hib = ((hib - 0x21) >> 1) + 0x81;
		if (hib > 0x9f) hib += 0x40;

		return (hib << 8) | lob;
	}

	// sjis コードをリニア表に変換する。
	// 上位バイト： 0x81 to 0x9f, 0xe0 to 0xef
	// 下位バイト： 0x40 to 0x7e, 0x80 to 0xfc
	static uint16_t sjis_to_liner_(uint16_t sjis)
	{
		uint16_t code;
		uint8_t up = sjis >> 8;
		uint8_t lo = sjis & 0xff;
		if(0x81 <= up && up <= 0x9f) {
			code = up - 0x81;
		} else if(0xe0 <= up && up <= 0xef) {
			code = (0x9f + 1 - 0x81) + up - 0xe0;
		} else {
			return 0xffff;
		}
		int loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
		if(0x40 <= lo && lo <= 0x7e) {
			code *= loa;
			code += lo - 0x40;
		} else if(0x80 <= lo && lo <= 0xfc) {
			code *= loa;
			code += 0x7e + 1 - 0x40;
			code += lo - 0x80;
		} else {
			return 0xffff;
		}
		return code;
	}


	static unsigned int mirror_(unsigned int bits, int count)
	{
		unsigned int v = 0;
		unsigned int m = 1 << (count - 1);
		for(int i = 0; i < count; ++i) {
			if(bits & 1) {
				v |= m;
			}
			m >>= 1;
			bits >>= 1;
		}
		return v;
	}


	void bdf_io::initialize()
	{
		uint32_t loa = (0x7e + 1 - 0x40) + (0xfc + 1 - 0x80);
		loa *= (0x9f + 1 - 0x81) + (0xef + 1 - 0xe0);

		lin_code_max_ = loa;
		codemap_.clear();

		jis_code_ = 0;
		bitmap_ = false;

		bbx_width_  = 0;
		bbx_height_ = 0;
	}


	bool bdf_io::load(const std::string& filename)
	{
		utils::file_io fin;
		if(fin.open(filename, "rb") == false) {
			return false;
		}

		bool retcode = true;
		std::string line;
		map_max_ = 0;
		while(fin.get_line(line) == true) {
			utils::strings ss;
			utils::split_text(line, " ", ss);
			if(ss.size() == 5) {
				if(ss[0] == "FONTBOUNDINGBOX") {
					bbx_width_  = boost::lexical_cast<int>(ss[1]);
					bbx_height_ = boost::lexical_cast<int>(ss[2]);
				}
			} else if(ss.size() == 1) {
				if(bitmap_) {
					if(ss[0] == "ENDCHAR") {
						uint16_t sjis = jis_to_sjis_(jis_code_);
						uint16_t lin = sjis_to_liner_(sjis);
						if(lin == 0xffff) {
							std::cerr << "Error JIS code map: " << jis_code_
								<< std::endl; 
							retcode = false;
						} else {
							if(lin_code_max_ > lin) {
								if(map_max_ < lin) map_max_ = lin;
								codemap_.push_back(sjis);
								int len = bbx_width_ * bbx_height_;
								if(len & 7) {
									// バイト単位になるように埋める
									for(int i = len; i <= (len | 7); ++i) {
										bit_array_.put_bit(0);
									}
								}
							} else {
								std::cerr << "Out of code area (Shift-JIS: "
									<< sjis << "): " << lin << std::endl;
								retcode = false;
							}
						}
						bitmap_ = false;
					} else {
						uint32_t bits;
						char ch;
						const char *p = ss[0].c_str();
						int n = 0;
						while((ch = *p++) != 0) {
							bits <<= 4;
							if('0' <= ch && ch <= '9') bits |= ch - '0';
							else if('A' <= ch && ch <= 'F') bits |= ch - 'A' + 10;
							else if('a' <= ch && ch <= 'f') bits |= ch - 'a' + 10;
							else {
								n = -1;
								break;
							}
							++n;
						}
						if(n > 0) {
							uint32_t v = mirror_(bits, n * 4);
							bit_array_.put_bits(v, bbx_width_);
						} else {
							std::cerr << "Bitmap hex-decimal decode error (width): "
								<< ss[0] << std::endl;
							retcode = false;
						}
					}
				} else {
					if(ss[0] == "BITMAP") {
						bitmap_ = true;
						bit_array_.clear();
					}
				}
			} else if(ss.size() == 2) {
				if(ss[0] == "ENCODING") {
					jis_code_ = boost::lexical_cast<int>(ss[1]);
				}
			}
			line.clear();
		}
		fin.close();

		return retcode;
	}


	bool bdf_io::save(const std::string& filename)
	{
		utils::file_io fout;

		if(fout.open(filename, "wb") == false) {
			return false;
		}

///		fout.write(&bitmaps_[0], bitmaps_.size());

		fout.close();

		return false;
	}
}
