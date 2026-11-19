#pragma once
#include <string_view>
#include <format>
#include <sstream>
#include <algorithm>
#include <Windows.h>
#include <DbgHelp.h>
#include "VCUWide.h"
#include "Base/Manager/IssueManager.h"
#include "Base/ManagerRegistry.h"

// DbgHelp ライブラリを自動リンク
#pragma comment(lib, "dbghelp.lib")

// マルチバイト文字列（SJISやUTF-8など）を UTF-16 の std::wstring に変換する関数
inline std::wstring widen_string(
    std::string_view narrowStr,
    UINT codePage = CP_ACP)
{
	if (narrowStr.empty())
	{
		return std::wstring{};
	}

	// 必要な文字数を計算（NULL文字を含む）
	int wideSize = MultiByteToWideChar(codePage, 0, narrowStr.data(), (int)narrowStr.size(), nullptr, 0);

	if (wideSize <= 0)
	{
		return std::wstring{};
	}

	std::wstring wideStr(wideSize, L'\0');

	// 実際に変換を実行
	MultiByteToWideChar(codePage, 0, narrowStr.data(), (int)narrowStr.size(), &wideStr[0], wideSize);

	return wideStr;
}

// すでに std::wstring の場合はそのまま返すオーバーロードもあると便利です
inline std::wstring widen_string(
    std::wstring_view wideStr)
{
	return std::wstring(wideStr);
}

// コールスタック文字列を取得するヘルパー関数
inline std::wstring GetCallStack(
    DWORD skipFrames = 1,
    DWORD maxFrames  = 32)
{
	HANDLE process = GetCurrentProcess();

	// 初回のみシンボルハンドラーを初期化（プロセスごとに1回が望ましいですが、簡易的にここで実施）
	static bool isSymInitialized = []()
	{
		SymInitialize(GetCurrentProcess(), NULL, TRUE);
		return true;
	}();

	void* stack[32];
	SecureZeroMemory(stack, sizeof(stack));

	// スタックトレースの取得
	WORD frames = CaptureStackBackTrace(skipFrames, std::min(maxFrames, 32UL), stack, NULL);

	std::wostringstream oss;

	SYMBOL_INFO* symbol = (SYMBOL_INFO*)malloc(sizeof(SYMBOL_INFO) + 256 * sizeof(TCHAR));
	if (!symbol)
		return L"スタックトレースの取得に失敗しました (メモリ確保エラー)\r\n";

	SecureZeroMemory(symbol, sizeof(SYMBOL_INFO) + 256 * sizeof(TCHAR));
	symbol->MaxNameLen   = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	IMAGEHLP_LINE64 line;
	SecureZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

	for (WORD i = 0; i < frames; ++i)
	{
		DWORD64 address = (DWORD64)(stack[i]);

		// 関数名の取得
		if (SymFromAddr(process, address, 0, symbol))
		{
			oss << std::format(L"  at {} (", widen_string(symbol->Name).c_str());  // ※1
		}
		else
		{
			oss << L"  at <unknown> (";
		}

		// ファイル名と行番号の取得
		DWORD displacement = 0;
		if (SymGetLineFromAddr64(process, address, &displacement, &line))
		{
			// フルパスが長すぎる場合はファイル名だけにするなどの調整も可能です
			std::wstring filename = widen_string(line.FileName);
			// ファイル名だけを抽出したい場合は以下のようにする
			// size_t lastSlash = filename.find_last_of(L"\\/");
			// if (lastSlash != std::wstring::npos) filename = filename.substr(lastSlash + 1);

			oss << std::format(L"{}:line {})\r\n", filename, line.LineNumber);
		}
		else
		{
			oss << std::format(L"address: 0x{:X})\r\n", address);
		}
	}

	free(symbol);
	return oss.str();
}
// ↑コールスタックも表示する方法をgeminiくんに聞いてみた

#ifdef _DEBUG
#define VCU_ASSERT(expr)                                                                                                 \
	do                                                                                                                   \
	{                                                                                                                    \
		if (!expr)                                                                                                       \
		{                                                                                                                \
			std::wstring stackTrace = GetCallStack(1, 16);                                                               \
			std::wstring text       = std::format(                                                                       \
                VCU_WIDE("式:{}\r\nファイル:{}\r\n行数:{}\r\n最終エラーコード:{}\r\n\r\n[コールスタック]:\r\n{}"), \
                VCU_WIDE(#expr),                                                                                   \
                VCU_WIDE(__FILE__),                                                                                \
                (unsigned int)__LINE__,                                                                            \
                GetLastError(),                                                                                    \
                stackTrace);                                                                                       \
			std::wstring caption{ L"Assertion Failed!" };                                                                \
			UINT type{ MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_DEFBUTTON2 };                                              \
			int ret = MessageBox(nullptr, text.c_str(), caption.c_str(), type);                                          \
			switch (ret)                                                                                                 \
			{                                                                                                            \
			case IDRETRY:                                                                                                \
				__debugbreak();                                                                                          \
				break;                                                                                                   \
			case IDABORT:                                                                                                \
			case IDIGNORE:                                                                                               \
			default:                                                                                                     \
				PostQuitMessage(-1);                                                                                     \
				break;                                                                                                   \
			}                                                                                                            \
		}                                                                                                                \
	}                                                                                                                    \
	while (false)
#else
#define VCU_ASSERT(expr)                                                                                                 \
	do                                                                                                                   \
	{                                                                                                                    \
		if (!expr)                                                                                                       \
		{                                                                                                                \
			std::wstring stackTrace = GetCallStack(1, 16);                                                               \
			std::wstring text       = std::format(                                                                       \
                VCU_WIDE("式:{}\r\nファイル:{}\r\n行数:{}\r\n最終エラーコード:{}\r\n\r\n[コールスタック]:\r\n{}"), \
                VCU_WIDE(#expr),                                                                                   \
                VCU_WIDE(__FILE__),                                                                                \
                (unsigned int)__LINE__,                                                                            \
                GetLastError(),                                                                                    \
                stackTrace);                                                                                       \
			std::wstring caption{ L"Assertion Failed!" };                                                                \
			UINT type{ MB_ABORTRETRYIGNORE | MB_ICONSTOP | MB_DEFBUTTON2 };                                              \
			cha::ManagerRegistry::Ref<cha::IssueManager>().Error(text);                                                  \
		}                                                                                                                \
	}                                                                                                                    \
	while (false)
#endif