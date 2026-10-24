#include "pch\pch.h"
#include "Font.h"
#include "Base/Manager/D2DManager.h"
#include "Base/ManagerRegistry.h"

bool cha::FontRegistry::TryLoad(
    FontLoadConfig&& _config,
    FontResource& _out)
{
	HRESULT hResult = S_OK;

	ManagerRegistry::Ref<D2DManager>().RefWriteFactory(
	    [&_config, &_out, &hResult](const ComPtr<IDWriteFactory3>& _pDWriteFactory) -> HRESULT
	    {
		    VCU_LOG("フォント読み込み:{}", _config.file.string());

		    // 1. フォントファイルの参照を作成 (例: 実行ファイルと同じ場所の "myfont.ttf")
		    ComPtr<IDWriteFontFile> pFontFile{};
		    hResult = _pDWriteFactory->CreateFontFileReference(_config.file.c_str(), nullptr, &pFontFile);
		    if (FAILED(hResult))
		    {
			    return hResult;
		    }

		    ComPtr<IDWriteFontFaceReference> pFontFaceReference{};
		    hResult = _pDWriteFactory->CreateFontFaceReference(
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
		    hResult = _pDWriteFactory->CreateFontSetBuilder(&pFontSetBuilder);
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
		    hResult = _pDWriteFactory->CreateFontCollectionFromFontSet(pFontSet.Get(), &pFontCollection);
		    if (FAILED(hResult))
		    {
			    return hResult;
		    }

		    // 4. 作成したフォントコレクションを渡して TextFormat を作成
		    // ※フォントファミリ名は、システム名ではなく「フォントファイルが内部で持っている正式名」を指定します
		    hResult = _pDWriteFactory->CreateTextFormat(
		        _config.familyName.c_str(),  // NOTE: フォント内の正確なファミリ名でなければいけない
		        pFontCollection.Get(),
		        _config.weight,   // DWRITE_FONT_WEIGHT_NORMAL,
		        _config.style,    // DWRITE_FONT_STYLE_NORMAL,
		        _config.stretch,  // DWRITE_FONT_STRETCH_NORMAL,
		        _config.fontSize,
		        _config.localName.c_str(),
		        _out.pTextFormat.GetAddressOf());

		    // テキストの整列設定
		    _out.pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
		    _out.pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

		    return hResult;
	    });

	return hResult == S_OK;
}
