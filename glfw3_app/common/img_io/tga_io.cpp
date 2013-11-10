//=====================================================================//
/*!	@file
	@brief	TGA 画像を扱うクラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "img_io/tga_io.hpp"
#include <tga.h>

namespace img {

	static int getc_(void* context)
	{
		char ch;
		if(static_cast<utils::file_io*>(context)->get(ch)) {
			return static_cast<uint8_t>(ch);
		} else {
			return -1;
		}
	}

	static size_t read_(void* ptr, size_t size, void* context)
	{
		return static_cast<utils::file_io*>(context)->read(ptr, size);
	}

	static int putc_(char ch, void* context)
	{
		if(static_cast<utils::file_io*>(context)->put(ch)) {
			return 0;
		} else {
			return -1;
		}
	}

	static size_t write_(const void* ptr, size_t size, void* context)
	{
		return static_cast<utils::file_io*>(context)->write(ptr, size);
	}

	static int seek_(size_t offset, int mode, void* context)
	{
		using namespace utils;

		bool f = false;
		switch(mode) {
		case SEEK_SET:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::set);
			break;
		case SEEK_CUR:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::cur);
			break;
		case SEEK_END:
			f = static_cast<file_io*>(context)->seek(offset, file_io::seek::end);
			break;
		default:
			break;
		}
		if(f) return 0;
		else return -1;
	}

	static size_t tell_(void* context)
	{
		return static_cast<utils::file_io*>(context)->tell();
	}

	static TGA* create_(utils::file_io* fin)
	{
		TGA* tga = TGAInit();

		tga->fd = fin;

		tga->file.getc = getc_;
		tga->file.read = read_;
		tga->file.putc = putc_;
		tga->file.write = write_;
		tga->file.seek = seek_;
		tga->file.tell = tell_;

		tga->error = 0;

		return tga;
	}

	static TGA* info_(utils::file_io* fin, img::img_info& fo)
	{
		TGA* tga = create_(fin);
		if(TGAReadHeader(tga) != TGA_OK) {
			TGAFree(tga);
			return 0;
		}

// std::cout << "Map type: " << static_cast<int>(tga->hdr.map_t) << std::endl;
// std::cout << "Img type: " << static_cast<int>(tga->hdr.img_t) << std::endl;
// std::cout << "Map depth: " << static_cast<int>(tga->hdr.map_entry) << std::endl;
// std::cout << "ID: " << static_cast<int>(tga->hdr.id_len) << std::endl;
// std::cout << "X: " << static_cast<int>(tga->hdr.x) << std::endl;
// std::cout << "Y: " << static_cast<int>(tga->hdr.y) << std::endl;

		// Indexed color
		if(tga->hdr.map_t == 1 || tga->hdr.map_t == 9) {
			fo.i_depth = tga->hdr.depth;
			fo.clut_num = tga->hdr.map_len;
// std::cout << "Map depth: " << static_cast<int>(tga->hdr.map_entry) << std::endl;
			if(tga->hdr.map_entry == 15) {
				fo.r_depth = 5;
				fo.g_depth = 5;
				fo.b_depth = 5;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 16) {
				fo.r_depth = 5;
				fo.g_depth = 6;
				fo.b_depth = 5;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 24) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(tga->hdr.map_entry == 32) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			}
			fo.grayscale = false;
		}
		if(tga->hdr.img_t == 1 || tga->hdr.img_t == 9) ;
		else if(tga->hdr.img_t == 2 || tga->hdr.img_t == 10) {
// std::cout << static_cast<int>(tga->hdr.depth) << std::endl;
			if(tga->hdr.depth == 24) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 0;
			} else if(tga->hdr.depth == 32) {
				fo.r_depth = 8;
				fo.g_depth = 8;
				fo.b_depth = 8;
				fo.a_depth = 8;
			}
			fo.i_depth = 0;
			fo.clut_num = 0;
			fo.grayscale = false;
		} else if(tga->hdr.img_t == 3 || tga->hdr.img_t == 11) {
			fo.r_depth = 0;
			fo.g_depth = 0;
			fo.b_depth = 0;
			fo.a_depth = tga->hdr.alpha;
			fo.i_depth = tga->hdr.depth;
			fo.clut_num = 0;
			fo.grayscale = true;
		} else {
			TGAFree(tga);
			return 0;
		}

		fo.width = tga->hdr.width;
		fo.height = tga->hdr.height;
		fo.mipmap_level = 0;
		fo.multi_level = 0;

		return tga;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルか確認する
		@param[in]	fin	file_io クラス
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::probe(utils::file_io& fin)
	{
		size_t pos = fin.tell();

		TGA* tga = create_(&fin);
		bool f = false;
		if(TGAReadHeader(tga) == TGA_OK) {
			f = true;
		}
		TGAFree(tga);

		fin.seek(pos, utils::file_io::seek::set);

		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	画像ファイルの情報を取得する
		@param[in]	fin	file_io クラス
		@param[in]	fo	情報を受け取る構造体
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::info(utils::file_io& fin, img::img_info& fo)
	{
		size_t pos = fin.tell();

		TGA* tga = info_(&fin, fo);
		TGAFree(tga);

		fin.seek(pos, utils::file_io::seek::set);

		return tga != 0;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイル、ロード(utils::file_io)
		@param[in]	fin	ファイル I/O クラス
		@param[in]	opt	フォーマット固有の設定文字列
		@return エラーなら「false」を返す
	*/
	//-----------------------------------------------------------------//
	bool tga_io::load(utils::file_io& fin, const std::string& opt)
	{
		img::img_info fo;
		TGA* tga = info_(&fin, fo);

		if(tga == 0) {
			return false;
		}

		bool alpha = false;
		if(fo.a_depth) alpha = true;

		bool f = true;

		tuint32 flags = 0;

		if(fo.clut_num) {
			tbyte* cmap = 0;
			int n = TGAReadColorMap(tga, &cmap, flags);
			if(n != 0) {
				img_.destroy();
				idx_.create(vtx::spos(fo.width, fo.height), alpha);

				for(int i = 0; i < fo.clut_num; ++i) {
					img::rgba8 c;
					if(fo.r_depth == 8 && fo.a_depth == 0) {
						c.b = cmap[i * 3 + 0];
						c.g = cmap[i * 3 + 1];
						c.r = cmap[i * 3 + 2];
						c.a = 255;
					} else if(fo.r_depth == 8 && fo.a_depth == 8) {
						c.b = cmap[i * 4 + 0];
						c.g = cmap[i * 4 + 1];
						c.r = cmap[i * 4 + 2];
						c.a = cmap[i * 4 + 3];
					}
					idx_.put_clut(i, c);
				}
				free(cmap);

				tbyte* pix = new tbyte[fo.width];
				for(int y = 0; y < fo.height; ++y) {
					if(TGAReadScanlines(tga, pix, 0, 1, flags) == 0) {
						f = false;
						break;
					}
					int ofs;
					if(tga->hdr.vert) ofs = y;
					else ofs = fo.height - y - 1;
					for(int x = 0; x < fo.width; ++x) {
						idx8 i(pix[x]);
						idx_.put_pixel(x, ofs, i);
					}
				}
				delete[] pix;
			} else {
				f = false;
			}
		} else {
			idx_.destroy();
			img_.create(vtx::spos(fo.width, fo.height), alpha);

			tbyte* pix = new tbyte[fo.width * 4];
			uint32_t n = 3;
			if(alpha) ++n;
			n *= fo.width;
			for(int y = 0; y < fo.height; ++y) {
				if(TGAReadScanlines(tga, pix, 0, 1, flags) == 0) {
					f = false;
					break;
				}
				int ofs;
				if(tga->hdr.vert) ofs = y;
				else ofs = fo.height - y - 1;
				for(int x = 0; x < fo.width; ++x) {
					rgba8 c;
					if(alpha) {
						c.b = pix[x * 4 + 0];
						c.g = pix[x * 4 + 1];
						c.r = pix[x * 4 + 2];
						c.a = pix[x * 4 + 3];
					} else {
						c.b = pix[x * 3 + 0];
						c.g = pix[x * 3 + 1];
						c.r = pix[x * 3 + 2];
					} 
					img_.put_pixel(x, ofs, c);
				}
			}
			delete[] pix;
		}

		TGAFree(tga);

		return f;
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	BMP ファイルをセーブする
		@param[in]	fout	ファイル I/O クラス
		@param[in]	opt		フォーマット固有の設定文字列
		@return エラーがあれば「false」
	*/
	//-----------------------------------------------------------------//
	bool tga_io::save(utils::file_io& fout, const std::string& opt)
	{
		if(imf_ == 0) {
			return false;
		}

		TGA* tga = create_(&fout);
		TGAHeader& h = tga->hdr;
        h.id_len = 0;
		bool indexed = false;
		bool rle = false;
		if(opt == "rle") rle = true;

		if(imf_->get_type() == IMG::INDEXED8) {
			h.map_t = 1;
			h.img_t = rle ? 9 : 1;
			h.map_first = 0;
			h.map_len = imf_->get_clut_max();
			h.map_entry = imf_->test_alpha() ? 32 : 24;
			h.depth = 8;
			indexed = true;
		} else if(imf_->get_type() == IMG::FULL8) {
			h.map_t = 0;
			h.img_t = rle ? 10 : 2;
			h.map_first = 0;
			h.map_len = 0;
			h.map_entry = 0;
			h.depth = imf_->test_alpha() ? 32 : 24;
		} else {
			TGAFree(tga);
			return false;
		}
		h.x = 0;
		h.y = 0;
		h.width  = imf_->get_size().x;
		h.height = imf_->get_size().y;
		h.alpha  = imf_->test_alpha() ? 8 : 0;
// std::cout << "Depth: " << static_cast<int>(h.depth) << std::endl;
// std::cout << "Alpha: " << static_cast<int>(h.alpha) << std::endl;
		h.horz = 0;
    	h.vert = 0;

		// ヘッダー出力
		if(TGAWriteHeader(tga) != TGA_OK) {
			TGAFree(tga);
			return false;
		}

		tuint32 flags = 0;
		if(rle) flags |= TGA_RLE_ENCODE;

		if(indexed) {
			// カラールックアップテーブル出力
			{
				int n = imf_->test_alpha() ? 4 : 3;
				tbyte* clut = new tbyte[imf_->get_clut_max() * n];
				for(int i = 0; i < imf_->get_clut_max(); ++i) {
					rgba8 c;
					imf_->get_clut(i, c);
					clut[i * n + 0] = c.b;
					clut[i * n + 1] = c.g;
					clut[i * n + 2] = c.r;
					if(imf_->test_alpha()) clut[i * n + 3] = c.a;
				}
				TGAWriteColorMap(tga, clut, 0);
				delete[] clut;
			}

			// インデックス出力
			{
				tbyte* pix = new tbyte[imf_->get_size().x];
				for(int y = 0; y < imf_->get_size().y; ++y) {
					int ofs = imf_->get_size().y - y - 1;
					for(int x = 0; x < imf_->get_size().x; ++x) {
						idx8 i;
						imf_->get_pixel(x, ofs, i);
						pix[x] = i.i;
					}
					TGAWriteScanlines(tga, pix, 0, 1, flags);
				} 
				delete[] pix;
			}
		} else {
			// フルカラー出力
			int n = imf_->test_alpha() ? 4 : 3;
			tbyte* pix = new tbyte[imf_->get_size().x * n];
			for(int y = 0; y < imf_->get_size().y; ++y) {
				int ofs = imf_->get_size().y - y - 1;
				for(int x = 0; x < imf_->get_size().x; ++x) {
					rgba8 c;
					imf_->get_pixel(x, ofs, c);
					pix[x * n + 0] = c.b;
					pix[x * n + 1] = c.g;
					pix[x * n + 2] = c.r;
					if(imf_->test_alpha()) pix[x * n + 3] = c.a;
				}
				TGAWriteScanlines(tga, pix, 0, 1, flags);
			}
			delete[] pix;
		}

		return true;
	}
}
