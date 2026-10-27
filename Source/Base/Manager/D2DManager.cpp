
#include "D2DManager.h"
#include <Base/Manager/Dx11Manager.h>
#include "../Core/Resource.h"

#include "../Core/Resource/Font.h"

cha::D2DManager::D2DManager(
    Dx11Manager& _dx11) :
    dx11_{ _dx11 }
{
}

bool cha::D2DManager::Initialize()
{
	HRESULT hResult{};

	// Direct2DとDirectWriteのファクトリ作成
	D2D1_FACTORY_OPTIONS options{};
#ifdef _DEBUG
	// デバッグはかなり詳細に
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
	hResult = D2D1CreateFactory(
	    D2D1_FACTORY_TYPE_SINGLE_THREADED,
	    __uuidof(ID2D1Factory1),
	    &options,
	    reinterpret_cast<void**>(pD2DFactory_.ReleaseAndGetAddressOf()));
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "D2D1の作成に失敗");
		return false;
	}

	// NOTE: IUnknown** な点注意
	ComPtr<IUnknown> pUnknownFactory{};
	hResult =
	    DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory3), pUnknownFactory.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "IDWriteFactoryの作成に失敗");
		return false;
	}

	// pDWriteFactory_へキャストして渡す
	hResult = pUnknownFactory.As(&pDWriteFactory_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "IDWriteFactoryのキャストに失敗");
		return false;
	}

	ComPtr<IDXGIDevice> pDxgiDevice{};
	dx11_.Ref(
	    [&hResult, &pDxgiDevice](const ComPtr<ID3D11Device>& _pDevice)
	    {
		    hResult = _pDevice->QueryInterface(
		        __uuidof(IDXGIDevice),
		        reinterpret_cast<void**>(pDxgiDevice.ReleaseAndGetAddressOf()));
	    });
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pDxgiDeviceの取得に失敗");
		return false;
	}

	hResult = pD2DFactory_->CreateDevice(pDxgiDevice.Get(), pD2DDevice_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pD2DFactory_->CreateDeviceに失敗");
		return false;
	}

	hResult = pD2DDevice_->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, pD2DContext_.GetAddressOf());
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "pD2DDevice_->CreateDeviceContextに失敗");
		return false;
	}

	// --- Step 3: スワップチェーンのバックバッファを D2D のレンダーターゲットとして紐付ける ---
	ComPtr<IDXGISurface> pBackBuffer{};
	dx11_.RefSwapChain(
	    [&hResult, &pBackBuffer](const ComPtr<IDXGISwapChain>& _pSwapChain)
	    {
		    hResult = _pSwapChain->GetBuffer(0, __uuidof(IDXGISurface), &pBackBuffer);
	    });
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "_pSwapChain->GetBufferに失敗");
		return false;
	}

	// DPIの取得とピクセルプロパティの設定
	D2D1_BITMAP_PROPERTIES1 bitmapProperties = D2D1::BitmapProperties1(
	    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
	    D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));

	hResult = pD2DContext_->CreateBitmapFromDxgiSurface(pBackBuffer.Get(), &bitmapProperties, &pD2DTargetBitmap_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "CreateBitmapFromDxgiSurfaceに失敗");
		return false;
	}

	// D2Dの描画先を設定
	pD2DContext_->SetTarget(pD2DTargetBitmap_.Get());

	InitializeBrushes();

	return true;
}

HRESULT cha::D2DManager::InitializeBrushes()
{
	HRESULT hResult;
	hResult = pD2DContext_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pTextBrush_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "CreateSolidColorBrushに失敗");
		return hResult;
	}

	hResult = pD2DContext_->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &pFigureBrush_);
	if (FAILED(hResult))
	{
		VCU_ASSERT(SUCCEEDED(hResult) && "図形描画用 CreateSolidColorBrushに失敗");
		return hResult;
	}

	return S_OK;
}

void cha::D2DManager::Begin()
{
	if (pD2DContext_)
	{
		pD2DContext_->BeginDraw();
	}
}

void cha::D2DManager::End()
{
	if (pD2DContext_)
	{
		// EndDrawの戻り値でデバイスロスト等のハンドリングが可能
		HRESULT hResult = pD2DContext_->EndDraw();
		if (hResult == D2DERR_RECREATE_TARGET)
		{
			// 必要に応じてリソースの再生成処理をここに記述
			VCU_ASSERT(false && "リソースの再生成処理");
		}
	}
}

void cha::D2DManager::DrawString(
    const Resource<ResourceType::Font>& _font,
    const std::wstring_view _text,
    const Vec2 _begin,
    const Vec2 _size,
    const Color _color)
{
	if (!pD2DContext_)
	{
		return;  // コンテキストがないなら無視
	}

	if (_text.empty())
	{
		return;  // 空っぽな文字列は描画するだけ無駄なため無視
	}

	D2D1_RECT_F renderRect = D2D1::RectF(_begin.x, _begin.y, _begin.x + _size.x, _begin.y + _size.y);

	pTextBrush_->SetColor(D2D1::ColorF(_color.r, _color.g, _color.b, _color.a));

	// いざ描画
	pD2DContext_->DrawTextW(
	    _text.data(),
	    static_cast<UINT32>(_text.size()),
	    _font.Get().pTextFormat.Get(),
	    renderRect,
	    pTextBrush_.Get());
}

void cha::D2DManager::DrawString(
    const Resource<ResourceType::Font>& _font,
    const std::string_view _text,
    const Vec2 _begin,
    const Vec2 _size,
    const Color _color)
{
	if (_text.empty())
	{
		return;  // 空っぽな文字列は描画するだけ無駄なため無視
	}

	int neededSize = MultiByteToWideChar(CP_UTF8, 0, _text.data(), static_cast<int>(_text.size()), nullptr, 0);

	// TODO: この変換もこのメモリも無駄なため文字列セットを作って管理するといいかも
	std::wstring wstr(neededSize, 0);

	MultiByteToWideChar(CP_UTF8, 0, _text.data(), static_cast<int>(_text.size()), &wstr[0], neededSize);

	DrawString(_font, wstr, _begin, _size, _color);
}
