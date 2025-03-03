#include "dxDevice.h"
#include "exceptions.h"
#include "window.h"
#include "dxStructures.h"
#include <fstream>
#include <vector>

using namespace mini;
using namespace std;

DxDevice::DxDevice(const Window& window)
{
	SwapChainDescription desc{ window.getHandle(), window.getClientSize() };
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;

	auto hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
		nullptr, 0, D3D11_SDK_VERSION, &desc, &swapChain, &device, nullptr, &context);

	m_device.reset(device);
	m_swapChain.reset(swapChain);
	m_context.reset(context);

	if (FAILED(hr))
		THROW_WINAPI;
}

dx_ptr<ID3D11RenderTargetView> DxDevice::CreateRenderTargetView(const dx_ptr<ID3D11Texture2D>& texture) const
{
	ID3D11RenderTargetView* temp;
	auto hr = m_device->CreateRenderTargetView(texture.get(), nullptr, &temp);
	dx_ptr<ID3D11RenderTargetView> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

vector<BYTE> DxDevice::LoadByteCode(const wstring& filename)
{
	ifstream sIn(filename, ios::in | ios::binary);
	if (!sIn)
		THROW(L"Unable to open " + filename);
	sIn.seekg(0, ios::end);
	auto byteCodeLength = sIn.tellg();
	sIn.seekg(0, ios::beg);
	vector<BYTE> byteCode(static_cast<unsigned int>(byteCodeLength));
	if (!sIn.read(reinterpret_cast<char*>(byteCode.data()), byteCodeLength))
		THROW(L"Error reading" + filename);
	sIn.close();
	return byteCode;
}

dx_ptr<ID3D11Texture2D> DxDevice::CreateTexture(const D3D11_TEXTURE2D_DESC& desc) const
{
	ID3D11Texture2D *temp;
	auto hr = m_device->CreateTexture2D(&desc, nullptr, &temp);
	dx_ptr<ID3D11Texture2D> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(const dx_ptr<ID3D11Texture2D>& texture) const
{
	ID3D11DepthStencilView* temp;
	auto hr = m_device->CreateDepthStencilView(texture.get(), nullptr, &temp);
	dx_ptr<ID3D11DepthStencilView> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11Buffer> DxDevice::CreateBuffer(const void* data, const D3D11_BUFFER_DESC& desc) const
{
	D3D11_SUBRESOURCE_DATA sdata;
	ZeroMemory(&sdata, sizeof sdata);
	sdata.pSysMem = data;

	ID3D11Buffer* temp;
	auto hr = m_device->CreateBuffer(&desc, data ? &sdata : nullptr, &temp);
	dx_ptr<ID3D11Buffer> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11VertexShader> DxDevice::CreateVertexShader(vector<BYTE> vsCode) const
{
	ID3D11VertexShader *temp;
	auto hr = m_device->CreateVertexShader(reinterpret_cast<const void*>(vsCode.data()), vsCode.size(), nullptr, &temp);
	dx_ptr<ID3D11VertexShader> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11PixelShader> DxDevice::CreatePixelShader(vector<BYTE> psCode) const
{
	ID3D11PixelShader *temp;
	auto hr = m_device->CreatePixelShader(reinterpret_cast<const void*>(psCode.data()), psCode.size(), nullptr, &temp);
	dx_ptr<ID3D11PixelShader> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11InputLayout> DxDevice::CreateInputLayout(const vector<D3D11_INPUT_ELEMENT_DESC> elements, vector<BYTE> vsCode) const
{
	ID3D11InputLayout* temp;
	auto hr = m_device->CreateInputLayout(elements.data(), static_cast<UINT>(elements.size()),
									reinterpret_cast<const void*>(vsCode.data()), vsCode.size(), &temp);
	dx_ptr<ID3D11InputLayout> result(temp);
	if (FAILED(hr))
		THROW_WINAPI;
	return result;
}

dx_ptr<ID3D11DepthStencilView> DxDevice::CreateDepthStencilView(SIZE size) const {
	auto desc = Texture2DDescription::DepthStencilDescription(size.cx, size.cy);
	dx_ptr<ID3D11Texture2D> texture = CreateTexture(desc);
	return CreateDepthStencilView(texture);
}
