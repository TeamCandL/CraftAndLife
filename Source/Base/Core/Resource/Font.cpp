#include "pch\pch.h"
#include "Font.h"

bool cha::FontRegistry::TryLoad(
    FontLoadConfig&& _config,
    FontResource& _out)
{
	HRESULT hResult = S_OK;

	// TODO: マジックを消す
	const fs::path FONT_FILE{ "Font/YuseiMagic/YuseiMagic-Regular.ttf" };

	// 1. フォントファイルの参照を作成 (例: 実行ファイルと同じ場所の "myfont.ttf")
	ComPtr<IDWriteFontFile> pFontFile{};
	hResult = pDWriteFactory_->CreateFontFileReference(FONT_FILE.c_str(), nullptr, &pFontFile);
	if (FAILED(hResult))
	{
		return hResult;
	}

	ComPtr<IDWriteFontFaceReference> pFontFaceReference{};
	hResult = pDWriteFactory_->CreateFontFaceReference(
	    pFontFile.Get(),
	    0,                             // faceIndex (通常は0)
	    DWRITE_FONT_SIMULATIONS_NONE,  // フォントのシミュレーション（斜体や太字の強制変形なし）
	    &pFontFaceReference);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 2. フォントセットビルダーを使って、このファイルを含むフォントセットを作成
	ComPtr<IDWriteFontSetBuilder> pFontSetBuilder{};
	hResult = pDWriteFactory_->CreateFontSetBuilder(&pFontSetBuilder);
	if (FAILED(hResult))
	{
		return hResult;
	}

	hResult = pFontSetBuilder->AddFontFaceReference(pFontFaceReference.Get());
	if (FAILED(hResult))
	{
		return hResult;
	}

	ComPtr<IDWriteFontSet> pFontSet{};
	hResult = pFontSetBuilder->CreateFontSet(&pFontSet);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 3. フォントセットからフォントコレクションを作成
	ComPtr<IDWriteFontCollection1> pFontCollection{};
	hResult = pDWriteFactory_->CreateFontCollectionFromFontSet(pFontSet.Get(), &pFontCollection);
	if (FAILED(hResult))
	{
		return hResult;
	}

	// 4. 作成したフォントコレクションを渡して TextFormat を作成
	// ※フォントファミリ名は、システム名ではなく「フォントファイルが内部で持っている正式名」を指定します
	hResult = pDWriteFactory_->CreateTextFormat(
	    L"Yusei Magic",  // NOTE: フォント内の正確なファミリ名でなければいけない
	    pFontCollection.Get(),
	    DWRITE_FONT_WEIGHT_NORMAL,
	    DWRITE_FONT_STYLE_NORMAL,
	    DWRITE_FONT_STRETCH_NORMAL,
	    32.0f,
	    L"ja-jp",
	    pTextFormat_.GetAddressOf());

	return hResult;
}
