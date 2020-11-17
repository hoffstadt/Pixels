#pragma once
#include <Windows.h>
#include <optional>

namespace Pixels {

	class Window
	{

	public:

		static std::optional<int> ProcessMessages();

	public:

		Window(const char* name, int width, int height);
		~Window();

		HWND getHandle() const { return m_hWnd; }

	private:

		static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	private:

		int  m_width;
		int  m_height;
		HWND m_hWnd;

	private:

		//-----------------------------------------------------------------------------
		// singleton manages registration/cleanup of window class
		//-----------------------------------------------------------------------------
		class WindowClass
		{

		public:

			static const char* GetName    () { return s_wndClassName; }
			static HINSTANCE   GetInstance() { return s_wndClass.m_hInst; }

		private:

			WindowClass();
			~WindowClass();

		private:

			static constexpr const char* s_wndClassName = "Pixels Window";
			static WindowClass           s_wndClass;
			HINSTANCE                    m_hInst;
		};
	};

}