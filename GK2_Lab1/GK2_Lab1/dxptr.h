#pragma once
#include <memory>

namespace mini
{
	/**********************************************************************//*!
	 * Deleter function object class for COM pointers.
	 * @tparam T COM object type that derives from IUnknown.
	 * @remark This deleter is designed to be used with DirectX resources.
	 * It can however work with any COM objects, since that is what all DirectX
	 * resources are.
	 *************************************************************************/
	template<class T>
	class DxDeleter
	{
	public:
		/******************************************************************//*!
		 * Type of a pointer to a COM object that can be handled by this
		 * deleter class.
		 *********************************************************************/
		typedef T* pointer;

		/******************************************************************//*!
		 * Releases the COM object.
		 * @param comObject pointer to COM object to be released.
		 * @remarks The only thing that needs to be done is to call
		 * IUnknown::Release because all COM object internally implement
		 * reference counting and delete themseleves if no longer used.
		 *********************************************************************/
		void operator () (pointer comObject) { if (comObject) comObject->Release(); }
	};

	/**********************************************************************//*!
	 * @brief Unique pointer to COM object.
	 * 
	 * Pointer indicating the ownership of COM resource. It makes sure the
	 * resource is released correctly based on RAII principle.
	 * 
	 * Copying is intentionally not supported. One can create multiple
	 * dx_ptr objects pointing to the same COM object and rely on its built-in
	 * reference counting. IUnknown::AddRef must manually be called on the COM
	 * object for each additional dx_ptr created that way, besides the first
	 * one.
	 * @tparam T COM object type that derives from IUnknown.
	 * @remark This smart pointer is designed to be used with DirectX
	 * resources. It can however work with any COM objects, since that is what
	 * all DirectX resources are.
	 *************************************************************************/
	template<class T>
	using dx_ptr = std::unique_ptr<T, DxDeleter<T>>;
}