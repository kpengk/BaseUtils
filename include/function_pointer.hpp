// Convert C++ function pointer to c function pointer
#include <functional>
#include <type_traits>

template<typename T, unsigned int N>
struct FunctionPointer;

template<typename Ret, typename... Params, unsigned int N>
struct FunctionPointer<Ret(Params...), N>
{
	using ret_fn = Ret(*)(Params...);

	template<typename... Args>
	static Ret func_pointer(Args... args)
	{
		func(args...);
	}

	static ret_fn get_func_pointer(std::function<Ret(Params...)> fn)
	{
		func = fn;
		return static_cast<ret_fn>(FunctionPointer<Ret(Params...), N>::func_pointer);
	}

	static std::function<Ret(Params...)> func;
};

// Initialize the static member.
template<typename Ret, typename... Params, unsigned int N>
std::function<Ret(Params...)> FunctionPointer<Ret(Params...), N>::func;

#define FN_PTR(fnptrtype) FunctionPointer<std::remove_pointer_t<fnptrtype>, __COUNTER__>::get_func_pointer
