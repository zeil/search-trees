#pragma once

#if __cplusplus < 201402L
namespace std
{
// https://herbsutter.com/gotw/_102/
template<typename T, typename ...Args>
std::unique_ptr<T> make_unique( Args&& ...args )
{
	return std::unique_ptr<T>( new T( std::forward<Args>(args)... ) );
}
} // namespace std
#endif // __cplusplus < 201402L
