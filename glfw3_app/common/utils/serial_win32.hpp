#pragma once
//=====================================================================//
/*!	@file
	@brief	シリアルＩ／Ｏ（WIN32）@n
			※要ライブラリ： hid setupapi ksguid
    @author 平松邦仁 (hira@rvf-rc45.net)
	@copyright	Copyright (C) 2018 Kunihito Hiramatsu @n
				Released under the MIT license @n
				https://github.com/hirakuni45/glfw_app/blob/master/LICENSE
*/
//=====================================================================//
#include <windows.h>
#include <winioctl.h>
#include <setupapi.h>
#include <string>
#include <vector>

namespace device {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	/*!
		@brief	シリアル通信クラス（WIN32)
	*/
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
	class serial_win32 {
	public:

		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	シリアル・ポート名構造体
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		struct name_t {
			std::string		port;	///< ポート名
			std::string		info;	///< ポート情報
		};
		typedef std::vector<name_t> name_list;


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	パリティー識別子
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class PARITY {
			NONE,	///< パリティー無し
			EVEN,	///< 偶数パリティー
			ODD,	///< 奇数パリティー
		};


		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		/*!
			@brief	フロー制御識別子
		*/
		//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++//
		enum class FLOW {
			NONE,	///< フロー制御無し
			HARD,	///< RTS/CTS 信号によるハードウェアー制御
			SOFT,	///< Xon/Xoff によるソフトウェアー制御 
		};

	private:
		HANDLE		fd_;

		std::string	error_;

		name_list	name_list_;

		bool scan_port_(const name_list& list, const std::string& port) const
		{
			for(const auto& t : list) {
				if(t.port == port) return true;
			}
			return false;
		}

	public:
		//-----------------------------------------------------------------//
		/*!
			@brief	コンストラクター
		*/
		//-----------------------------------------------------------------//
		serial_win32() : fd_(INVALID_HANDLE_VALUE) { }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポート・リストを作成
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool create_list()
		{
			name_list_.clear();

			// デバイス情報セットを取得
			HDEVINFO hi = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT, 0, 0,
				DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
			if(hi == 0) {
				return false;
			}

			SP_DEVINFO_DATA data = { 0 };
			data.cbSize = sizeof(data);
			int idx = 0;
			while(SetupDiEnumDeviceInfo(hi, idx, &data)) {
				name_t t;
				// COM ポート名の取得
				HKEY key = SetupDiOpenDevRegKey(hi, &data, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
				if(key) {
					BYTE name[256];
					DWORD type = 0;
					DWORD size = sizeof(name);
					RegQueryValueEx(key, _T("PortName"), NULL, &type, name, &size);
					t.port = reinterpret_cast<const char*>(name);
				}

				//	デバイスの情報を取得
				DWORD dt = 0;
				PBYTE ptr = nullptr;
				DWORD size = 0;
				SetupDiGetDeviceRegistryProperty(hi, &data, SPDRP_DEVICEDESC, &dt, ptr, size, &size);
				if(size > 0) {
					ptr = new BYTE[size];
					SetupDiGetDeviceRegistryProperty(hi, &data, SPDRP_DEVICEDESC, &dt, ptr, size, &size);
					t.info = reinterpret_cast<const char*>(ptr);
					delete[] ptr;
				}
				++idx;
				name_list_.push_back(t);
			}

			// デバイス情報セットを解放
			SetupDiDestroyDeviceInfoList(hi);

			return !name_list_.empty();
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ポート・リストを取得
			@return ポート・リスト
		*/
		//-----------------------------------------------------------------//
		const name_list& get_list() const { return name_list_; }


		//-----------------------------------------------------------------//
		/*!
			@brief	ポート・リスト比較（新規デバイスでリスト更新確認など）
			@return 同じなら「true」
		*/
		//-----------------------------------------------------------------//
		bool compare(const name_list& list) const
		{
			if(name_list_.empty()) return false;

			for(const auto& t : list) {
				if(!scan_port_(name_list_, t.port)) return false;
			}
			return true;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ポート情報の取得
			@param[in]	port	ポート名
			@return ポート情報
		*/
		//-----------------------------------------------------------------//
		const std::string& get_info(const std::string& port) const
		{
			static std::string tmp;
			if(name_list_.empty()) return tmp;

			for(const auto& t : name_list_) {
				if(t.port == port) {
					return t.info;
				}
			}
			return tmp;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	オープン
			@param[in]	port	ポート名
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool open(const std::string& port)
		{
			std::string full("\\\\.\\");
			full += port;
///			snprintf(port_full_name, sizeof port_full_name - 2u, "\\\\.\\%s", port);
///			if(4 <= verbose_level) {
///				printf("\t\tOpen port: %s\n", port_full_name);
///			}
			fd_ = CreateFile(full.c_str(),
					GENERIC_READ | GENERIC_WRITE,
                    0,
                    NULL,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    0);

			bool ret = false;
			if(INVALID_HANDLE_VALUE == fd_) {
				int error_num = GetLastError();
				char error_string[1024];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
					NULL,
					error_num,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					error_string,
					sizeof error_string,
					NULL);
///					std::cout << fd_ << ": " << error_string << std::endl;
					error_ = error_string;
			} else {
				DCB dcb;
				GetCommState(fd_, &dcb);
				dcb.BaudRate = CBR_115200;
				dcb.ByteSize = 8;
				dcb.StopBits = ONESTOPBIT;
				dcb.Parity   = NOPARITY;
				dcb.fParity  = FALSE;
				dcb.fBinary  = TRUE;
//				dcb.fRtsControl = FALSE;
				SetCommState(fd_, &dcb);

				COMMTIMEOUTS tos;  // 単位（ミリ秒）
				tos.ReadIntervalTimeout         = 250;
				tos.ReadTotalTimeoutConstant    = 250;
				tos.ReadTotalTimeoutMultiplier  = 200;
				tos.WriteTotalTimeoutConstant   = 0;
				tos.WriteTotalTimeoutMultiplier = 0;
				SetCommTimeouts(fd_, &tos);
				FlushFileBuffers(fd_);
				ret = true;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フロー制御の設定
			@param[in]	type	フロー制御識別子
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_flow(FLOW type)
		{
			bool ret = false;
			switch(type) {
			case FLOW::NONE:

				break;
			case FLOW::HARD:

				break;
			case FLOW::SOFT:

				break;
			default:
				break;
			}
			return ret;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ボーレートの設定
			@param[in]	baud	ボーレート
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_baudrate(int baud)
		{
			DCB dcb;
			GetCommState(fd_, &dcb);
			dcb.BaudRate = baud;
			return SetCommState(fd_, &dcb) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	パリティーの設定
			@param[in]	parity	パリティー・タイプ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_parity(PARITY parity)
		{
			DCB dcb;
			GetCommState(fd_, &dcb);
			switch(parity) {
			case PARITY::EVEN:
				dcb.Parity = EVENPARITY;
				break;
			case PARITY::ODD:
				dcb.Parity = ODDPARITY;
				break;
			case PARITY::NONE:
				dcb.Parity = NOPARITY;
				break;
			default:
				return false;
				break;
			}
			return SetCommState(fd_, &dcb) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	DTR の設定（Data Terminal Ready)
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_dtr(bool value)
		{
			int command;
			if(value) {
				command = SETDTR;
			} else {
				command = CLRDTR;
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	RTS の設定（Request To Send)
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_rts(bool value)
		{
			int command;
			if(value) {
				command = SETRTS;
			} else {
				command = CLRRTS;
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	TXD の設定
			@param[in]	value	設定値
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool set_txd(bool value)
		{
			int command;
			if(value) {
				command = SETBREAK;
			} else {
				command = CLRBREAK;
				
			}
			return EscapeCommFunction(fd_, command) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	フラッシュ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool flush()
		{
//			if(4 <= verbose_level) {
//				printf("\t\tFlush IO buffers\n");
//			}
			return PurgeComm(fd_, PURGE_RXCLEAR | PURGE_TXCLEAR) != 0;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	ライト
			@param[in]	src	転送元
			@param[in]	len	長さ（バイト）
			@return 書き込んだサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t write(const void* src, uint32_t len)
		{
			if(src == nullptr || len == 0) return 0;
#if 0
			if(4 <= verbose_level) {
				unsigned char *p = (unsigned char*)buf;
				unsigned int i;
				printf("\t\tsend(%u): ", len);
				for(i = len; 0 < i; --i) {
					printf("%02X ", *p++);
				}
				printf("\n");
			}
#endif
			int bytes_left = len;
			DWORD bytes_written;
			const uint8_t* p = reinterpret_cast<const uint8_t*>(src);
			do {
				if(0 == WriteFile(fd_, p, bytes_left, &bytes_written, NULL)) {
//					fprintf(stderr, "Failed to write to port.\n");
					return false;
				}
				p += bytes_written;
				bytes_left -= bytes_written;
			} while(0 < bytes_left) ;
			return len - bytes_left;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	リード
			@param[out]	dst	転送先
			@param[in]	len	長さ（バイト）
			@return 書き込んだサイズ
		*/
		//-----------------------------------------------------------------//
		uint32_t read(void *dst, int len)
		{
			if(dst == nullptr || len == 0) return 0;

			int bytes_left = len;
			DWORD bytes_read;
			uint8_t* p = reinterpret_cast<uint8_t*>(dst);
			do {
				if(0 == ReadFile(fd_, p, bytes_left, &bytes_read, NULL)) {
///					fprintf(stderr, "Failed to read from port.\n");
//					last_error_ = 1;
					return 0;
				}
				if(0 == bytes_read) {
					break;
				}
				p += bytes_read;
				bytes_left -= bytes_read;
			} while(0 < bytes_left) ;

			uint32_t nbytes = len - bytes_left;
#if 0
			if(4 <= verbose_level) {
				p = dst;
				unsigned int i;
				printf("\t\trecv(%u): ", nbytes);
				for(i = nbytes; 0 < i; --i) {
					printf("%02X ", *pbuf++);
				}
				printf("\n");
			}
#endif
			return nbytes;
		}


		//-----------------------------------------------------------------//
		/*!
			@brief	クローズ
			@return 成功なら「true」
		*/
		//-----------------------------------------------------------------//
		bool close() {
//			if(4 <= verbose_level) {
//				printf("\t\tClose port\n");
//			}
			return CloseHandle(fd_) != 0;
		}
	};
}
