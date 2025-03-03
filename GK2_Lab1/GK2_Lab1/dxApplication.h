#pragma once
#include "windowApplication.h"
#include "dxDevice.h"
#include <DirectXMath.h>
#include <vector>

class DxApplication : public mini::WindowApplication
{
public:
	explicit DxApplication(HINSTANCE hInstance);
	std::vector<DirectX::XMFLOAT2> CreateTriangleVertices();

protected:
	int MainLoop() override;	
	bool ProcessMessage(mini::WindowMessage& msg) override;

private:
	void Render();
	void Update();

	struct VertexPositionColor {
		DirectX::XMFLOAT3 position, color;
	};

	static std::vector<VertexPositionColor> CreateCubeVertices();
	static std::vector<unsigned short> CreateCubeIndices();
	mini::dx_ptr<ID3D11Buffer> m_indexBuffer;

	DxDevice m_device;
	mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
	mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
	mini::dx_ptr<ID3D11Buffer> m_vertexBuffer;
	mini::dx_ptr<ID3D11VertexShader> m_vertexShader;
	mini::dx_ptr<ID3D11PixelShader> m_pixelShader;
	mini::dx_ptr<ID3D11InputLayout> m_layout;

	DirectX::XMFLOAT4X4 m_modelMtx, m_viewMtx, m_projMtx;
	mini::dx_ptr<ID3D11Buffer> m_cbMVP;

	float m_angle = 30.0f;

	bool m_dragging_left = false;
	bool m_dragging_right = false;

	int m_lastMouseX = 0, m_lastMouseY = 0;

	float m_degreesX = -30.0f;
	float m_degreesY = 0.0f;
	float m_degreesZ = 0.0f;

	float m_transZ = 25.0f;

	float m_cubeTransX = 0.0f;
};
