#pragma once

#include "windowApplication.h"
#include "dxDevice.h"
#include "DirectXMath.h"
#include "clock.h"
#include "diInstance.h"
#include "keyboard.h"
#include "mouse.h"
#include "camera.h"

//***************** NEW *****************
//This is now a base class for future tasks.
//Added time measuring, input handling, camera support
//Added helper function to update constant buffer content

namespace mini
{
	class DxApplication : public mini::WindowApplication
	{
	public:
		explicit DxApplication(HINSTANCE hInstance,
			int wndWidth = Window::m_defaultWindowWidth,
			int wndHeight = Window::m_defaultWindowHeight,
			std::wstring wndTitle = L"DirectX Window");

	protected:
		int MainLoop() override;

		const Clock& getClock() const { return m_clock; }
		virtual void Render();
		virtual void Update(const Clock& c) {}

		void UpdateBuffer(const dx_ptr<ID3D11Buffer>& buffer, const void* data, size_t count);
		template<typename T>
		void UpdateBuffer(const dx_ptr<ID3D11Buffer>& buffer, const T& data)
		{
			UpdateBuffer(buffer, &data, sizeof(T));
		}

		bool HandleCameraInput(double dt);

		DxDevice m_device;

		DiInstance m_inputDevice;
		Mouse m_mouse;
		Keyboard m_keyboard;

		static constexpr float ROTATION_SPEED = 0.01f;
		static constexpr float ZOOM_SPEED = 0.02f;

		OrbitCamera m_camera;

	private:

		mini::dx_ptr<ID3D11RenderTargetView> m_backBuffer;
		mini::dx_ptr<ID3D11DepthStencilView> m_depthBuffer;
		Clock m_clock;

	};
}