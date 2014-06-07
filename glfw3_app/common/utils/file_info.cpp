//=====================================================================//
/*!	@file
	@brief	ファイル情報クラス
	@author	平松邦仁 (hira@rvf-rc45.net)
*/
//=====================================================================//
#include "utils/file_info.hpp"
#include <dirent.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>

namespace utils {

	//-----------------------------------------------------------------//
	/*!
		@brief	ディレクトリーのファイルリストを作成
		@param[in]	root	ルート・パス
		@param[out]	list	ファイルリストを受け取るクラス
		@return リストの取得に失敗した場合「false」
	*/
	//-----------------------------------------------------------------//
	bool create_file_list(const std::string& root, file_infos& list)
	{
		if(root.empty()) return false;

#ifdef WIN32
		utils::wstring wsr;
		utf8_to_utf16(root, wsr);
		wchar_t* wtmp = new wchar_t[wsr.size() + 1];
		for(uint32_t i = 0; i < wsr.size(); ++i) {
			wtmp[i] = wsr[i];
		}
		wtmp[wsr.size()] = 0;
		_WDIR* dir = _wopendir(wtmp);
		delete[] wtmp;
#else
//		std::string rsjis;
//		utils::utf8_to_sjis(root, rsjis);
		DIR* dir = opendir(root.c_str());
#endif
		if(dir) {
			int i = 0;
#ifdef WIN32
			struct _wdirent* ent;
			while((ent = _wreaddir(dir)) != 0) {
				struct _stat st;
				utils::wstring fn;
				utf8_to_utf16(root, fn);
				fn += '/';
				{
					wchar_t wch;
					wchar_t* p = ent->d_name;
					while((wch = *p++) != 0) {					
						fn += wch;
					}
				}
				wchar_t* wfn = new wchar_t[fn.size() + 1];
				for(uint32_t i = 0; i < fn.size(); ++i) {
					wfn[i] = fn[i];
				}
				wfn[fn.size()] = 0;
				if(ent->d_namlen > 0 && _wstat(wfn, &st) == 0) {
					bool d = S_ISDIR(st.st_mode);
					wstring ws;
					wchar_t wch;
					wchar_t* p = ent->d_name;
					while((wch = *p++) != 0) {
						ws += wch;
					}
					std::string s;
					utf16_to_utf8(ws, s);
					file_info info(s, d, st.st_size, st.st_mtime, st.st_mode);
					list.push_back(info);
					i++;
				}
				delete[] wfn;
			}
			_wclosedir(dir);
#else
			struct dirent* ent;
			while((ent = readdir(dir)) != 0) {
				struct stat st;
///				std::string fn = rsjis;
				std::string fn = root;
				fn += '/';
				fn += ent->d_name;
				if(::stat(fn.c_str(), &st) == 0) {
#ifdef __PPU__
					bool d = false;
					if(st.st_mode & CELL_FS_S_IFDIR) d = true;
#else
					bool d = S_ISDIR(st.st_mode);
#endif
					file_info info(ent->d_name, d, st.st_size, st.st_mtime, st.st_mode);
					list.push_back(info);
					i++;
				}
			}
			closedir(dir);
#endif
			if(i) return true;
			else return false;
		} else {
			return false;
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報からフォーマットされた文字列を生成
		@param[in]	list	fileinfos クラス
		@param[in]	format	フォーマット（ディレクトリー出力専用形式）@n
							%s ---> ファイル名@n
							%n ---> ファイルサイズ@n
							%/ ---> ディレクトリーなら、'/'を追加
		@param[in]	dst		出力先列
	*/
	//-----------------------------------------------------------------//
	void create_format_file_infos(const file_infos& list, const std::string& format, utils::strings& dst)
	{
		BOOST_FOREACH(const file_info& fi, list) {
			if(format.empty()) {
				std::string s = fi.get_name();
				if(fi.is_directory()) s += '/';
				dst.push_back(s);
			} else {
				std::string s;
				char m = 0;
				BOOST_FOREACH(char c, format) {
					if(m) {
						if(c == '%') ;
						else if(c == 's' || c == 'S') {
							s += fi.get_name();
							c = 0;
						} else if(c == 'n') {
							s += boost::io::str(boost::format("%d") % fi.get_size());
							c = 0;
						} else if(c == '/') {
							if(!fi.is_directory()) c = 0;
						}
						m = 0;
					} else if(c == '%') {
						m = c;
						c = 0;
					}
					if(c) s += c;
				}
				dst.push_back(s);
			}
		}
	}


	//-----------------------------------------------------------------//
	/*!
		@brief	ファイル情報リストを拡張子フィルターで再構成
		@param[in]	src		fileinfos クラス
		@param[in]	filter	拡張子文字列
		@param[in]	dst		出力先列
	*/
	//-----------------------------------------------------------------//
	void filter_file_infos(const file_infos& src, const std::string& filter, file_infos& dst)
	{
		utils::strings ss;
		if(!filter.empty()) {
			utils::split_text(filter, ",", ss);
		}

		BOOST_FOREACH(const file_info& f, src) {
			bool add = false;
			if(f.is_directory() || ss.empty()) add = true;
			else {
				const std::string& n = f.get_name();
				BOOST_FOREACH(const std::string& s, ss) {
					const char* p = strrchr(n.c_str(), '.');
					if(p) {
						if(utils::no_capital_strcmp(std::string(p + 1), s) == 0) {
							add = true;
							break;
						}
					}
				}
			}
			if(add) dst.push_back(f);
		}
	}

}
