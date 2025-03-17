#include "dxApplication.h"
#include <math.h>
#include <iostream>

using namespace mini;

DxApplication::DxApplication(HINSTANCE hInstance)
	: WindowApplication(hInstance), m_device(m_window)
{
	ID3D11Texture2D *temp;
	dx_ptr<ID3D11Texture2D> backTexture;
	m_device.swapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&temp));
	backTexture.reset(temp);

	m_backBuffer = m_device.CreateRenderTargetView(backTexture);

	SIZE wndSize = m_window.getClientSize();
	m_depthBuffer = m_device.CreateDepthStencilView(wndSize);
	auto backBuffer = m_backBuffer.get();
	m_device.context()->OMSetRenderTargets(1, &backBuffer, m_depthBuffer.get());
	Viewport viewport{ wndSize };
	m_device.context()->RSSetViewports(1, &viewport);

	const auto vsBytes = DxDevice::LoadByteCode(L"vs.cso");
	const auto psBytes = DxDevice::LoadByteCode(L"ps.cso");
	m_vertexShader = m_device.CreateVertexShader(vsBytes);
	m_pixelShader = m_device.CreatePixelShader(psBytes);

	const auto vertices = CreateCubeVertices();
	m_vertexBuffer = m_device.CreateVertexBuffer(vertices);

	const auto indices = CreateCubeIndices();
	m_indexBuffer = m_device.CreateIndexBuffer(indices);

	std::vector<D3D11_INPUT_ELEMENT_DESC> elements{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPositionColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	m_layout = m_device.CreateInputLayout(elements, vsBytes);

	XMStoreFloat4x4(&m_modelMtx, DirectX::XMMatrixIdentity());
	XMStoreFloat4x4(
		&m_viewMtx, 
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_degreesX)) * DirectX::XMMatrixTranslation(0.0f, 0.0f, m_transZ)
	);

	float aspectRatio = static_cast<float>(wndSize.cx) / wndSize.cy;

	XMStoreFloat4x4(&m_projMtx, DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(m_angle), aspectRatio, 0.1f, 100.0f));

	m_cbMVP = m_device.CreateConstantBuffer<DirectX::XMFLOAT4X4>();

	LARGE_INTEGER li;
	if (QueryPerformanceFrequency(&li)) {
		m_freq = static_cast<float>(li.QuadPart);
	}

	if (QueryPerformanceCounter(&li)) {
		m_lastTime = static_cast<float>(li.QuadPart) / m_freq;
	}
}

int DxApplication::MainLoop()
{
	MSG msg;
	//PeekMessage doesn't change MSG if there are no messages to be recieved.
	//However unlikely the case may be, that the first call to PeekMessage
	//doesn't find any messages, msg is zeroed out to make sure loop condition
	//isn't reading unitialized values.
	ZeroMemory(&msg, sizeof msg);
	do
	{
		if (PeekMessage(&msg, nullptr, 0,0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
			Render();
			m_device.swapChain()->Present(0, 0);
		}
	} while (msg.message != WM_QUIT);
	return static_cast<int>(msg.wParam);
}

void DxApplication::Update()
{
	LARGE_INTEGER li;
	if (QueryPerformanceCounter(&li)) {
		float ticks = static_cast<float>(li.QuadPart);
		float new_time = ticks / m_freq;
		float delta_time = new_time - m_lastTime;
		m_angle += fmodf(new_time * DirectX::XM_PI / 4, 2 * DirectX::XM_2PI);
		m_lastTime = new_time;
	}
	

	XMStoreFloat4x4(
		&m_viewMtx,
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_degreesX)) * DirectX::XMMatrixTranslation(0.0f, 0.0f, m_transZ)
	);

	XMStoreFloat4x4(
		&m_modelMtx,
		DirectX::XMMatrixRotationY(m_angle) * DirectX::XMMatrixTranslation(m_cubeTransX, 0.0f, 0.0f)
	);

	D3D11_MAPPED_SUBRESOURCE res;

	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	DirectX::XMMATRIX mvp = XMLoadFloat4x4(&m_modelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);

	memcpy(res.pData, &mvp, sizeof(DirectX::XMFLOAT4X4));

	m_device.context()->Unmap(m_cbMVP.get(), 0);
}

std::vector<DxApplication::VertexPositionColor> DxApplication::CreateCubeVertices()
{
	return {
		// Front face
		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
		{ { -0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },

		// Back face
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } },
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } }, 
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 0.0f } }, 


		{ { -0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } }, // 8
		{ { +0.5f, -0.5f, -0.5f }, { 0.0f, 0.0f, 1.0f } }, // 9
		{ { +0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } }, // 10
		{ { -0.5f, -0.5f, +0.5f }, { 0.0f, 0.0f, 1.0f } }, // 11


		{ { -0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } }, // 12
		{ { +0.5f, +0.5f, -0.5f }, { 0.0f, 1.0f, 1.0f } }, // 13
		{ { +0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 1.0f } }, // 14
		{ { -0.5f, +0.5f, +0.5f }, { 0.0f, 1.0f, 1.0f } }, // 15

		{ { -0.5f, -0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } }, 
		{ { -0.5f, +0.5f, -0.5f }, { 1.0f, 1.0f, 0.0f } }, 
		{ { -0.5f, +0.5f, +0.5f }, { 1.0f, 1.0f, 0.0f } }, 
		{ { -0.5f, -0.5f, +0.5f }, { 1.0f, 1.0f, 0.0f } }, 

		{ { +0.5f, -0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } }, 
		{ { +0.5f, +0.5f, -0.5f }, { 1.0f, 0.0f, 1.0f } }, 
		{ { +0.5f, +0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } }, 
		{ { +0.5f, -0.5f, +0.5f }, { 1.0f, 0.0f, 1.0f } }, 


	};
}

std::vector<unsigned short> DxApplication::CreateCubeIndices()
{
	return {
		0, 2, 1,
		0, 3, 2,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 14, 13,
		12, 15, 14,

		16, 18, 17,
		16, 19, 18,

		20, 21, 22,
		20, 22, 23
	};
}

void DxApplication::Render()
{
	float clearColor[] = { 0.5f, 0.5f, 1.0f, 1.0f };
	m_device.context()->ClearRenderTargetView(m_backBuffer.get(), clearColor);

	m_device.context()->ClearDepthStencilView(m_depthBuffer.get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_device.context()->VSSetShader(m_vertexShader.get(), nullptr, 0);
	m_device.context()->PSSetShader(m_pixelShader.get(), nullptr, 0);
	m_device.context()->IASetInputLayout(m_layout.get());
	m_device.context()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	ID3D11Buffer* cbs[] = { m_cbMVP.get() };
	m_device.context()->VSSetConstantBuffers(0, 1, cbs);

	ID3D11Buffer* vbs[] = { m_vertexBuffer.get() };
	UINT strides[] = { sizeof(VertexPositionColor) };
	UINT offsets[] = { 0 };

	m_device.context()->IASetVertexBuffers(0, 1, vbs, strides, offsets);
	m_device.context()->IASetIndexBuffer(m_indexBuffer.get(), DXGI_FORMAT_R16_UINT, 0);
	m_device.context()->DrawIndexed(36, 0, 0);

	/*XMStoreFloat4x4(
		&m_viewMtx,
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(m_degreesX)) * DirectX::XMMatrixTranslation(0.0f, 0.0f, m_transZ)
	);

	XMStoreFloat4x4(
		&m_modelMtx,
		DirectX::XMMatrixRotationY(m_angle) * DirectX::XMMatrixTranslation(m_cubeTransX, 0.0f, 0.0f)
	);

	D3D11_MAPPED_SUBRESOURCE res;

	m_device.context()->Map(m_cbMVP.get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &res);

	DirectX::XMMATRIX mvp = XMLoadFloat4x4(&m_modelMtx) * XMLoadFloat4x4(&m_viewMtx) * XMLoadFloat4x4(&m_projMtx);

	memcpy(res.pData, &mvp, sizeof(DirectX::XMFLOAT4X4));

	m_device.context()->Unmap(m_cbMVP.get(), 0);*/
}

std::vector<DirectX::XMFLOAT2> DxApplication::CreateTriangleVertices() {
	return {
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f },
		{ -0.5f, 0.5f }
	};
}

bool DxApplication::ProcessMessage(mini::WindowMessage& msg)
{
	int mouseX = m_lastMouseX, mouseY = m_lastMouseY;

	switch (msg.message) {
		case WM_LBUTTONDOWN: {
			if (!m_dragging_right) {
				m_dragging_left = true;
			}
		} break;

		case WM_RBUTTONDOWN: {
			if (!m_dragging_left) {
				m_dragging_right = true;
			}
		} break;

		case WM_MOUSEMOVE: {
			mouseX = LOWORD(msg.lParam);
			mouseY = HIWORD(msg.lParam);

			int dx = mouseX - m_lastMouseX;
			int dy = mouseY - m_lastMouseY;

			if (m_dragging_left) {
				m_degreesX -= dy;
				m_degreesX = fminf(m_degreesX, 180);
				m_degreesX = fmaxf(m_degreesX, -180);
			}

			if (m_dragging_right) {
				m_transZ -= dy;
				m_transZ = fminf(m_transZ, 50);
				m_transZ = fmaxf(m_transZ, 0);
			}
		} break;

		case WM_LBUTTONUP: {
			m_dragging_left = false;
		} break;

		case WM_RBUTTONUP: {
			m_dragging_right = false;
		} break;

	}

	m_lastMouseX = mouseX;
	m_lastMouseY = mouseY;

	return false;
}
