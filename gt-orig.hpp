#ifndef GT_HPP
#define GT_HPP

#include <utility>
#include <type_traits>
#include <initializer_list>

namespace gametree {

	namespace help {
		// from http://stackoverflow.com/questions/4850473/pretty-print-c-stl-containers
		// (seems to be from Sven Groot)
		template<typename T>
			struct is_container_helper {
				private:
					template<typename C> static char
						test(decltype(begin(std::declval<C>())) *);
					template<typename C> static int  test(...);
				public:
					static const bool value
						= sizeof(test<T>(0)) == sizeof(char);
			};


		// Basic is_container template; specialize to derive from
		// std::true_type for all desired container types
		template<typename T> struct is_container
			: public ::std::integral_constant<bool,
						is_container_helper<T>::value> { };

		template<typename T1, typename T2>
			constexpr auto min(const T1 &t1, const T2 &t2) {
				return t1<t2 ? t1 : t2;
			}
		template<typename T1, typename T2>
			constexpr auto max(const T1 &t1, const T2 &t2) {
				return t1<t2 ? t2 : t1;
			}
	}

	template<typename R>
		constexpr auto expevalhelp(R &&sum, std::size_t n) {
			return sum/n;
		}

	template<typename E, typename... Es,typename R>
		constexpr auto expevalhelp(R &&sum, std::size_t n, E &&e, Es &&...es) {
			return expevalhelp(sum+e(),n+1,std::forward<Es>(es)...);
		}

	template<typename I1,typename I2,typename R>
		constexpr auto expevalhelp2(R &&sum, std::size_t n, I1 &&i, I2 &&e) -> decltype(sum/n) {
			return i==e ? sum/n : expevalhelp2(sum+(*i)(),n+1,i+1,std::forward<I2>(e));
		}

	template<typename L>
		constexpr auto expeval(L &&l) ->
			typename std::enable_if<help::is_container<L>::value,
				decltype(expevalhelp2((*begin(l))(),1,begin(l)+1,end(l)))>::type {
			return expevalhelp2((*begin(l))(),1,begin(l)+1,end(l));
		}



	template<typename E, typename... Es>
		constexpr auto expeval(E &&e, Es &&...es) ->
			typename std::enable_if<!help::is_container<E>::value,
				decltype(expevalhelp(e(),1,std::forward<Es>(es)...))>::type {
			return expevalhelp(e(),1,std::forward<Es>(es)...);
		}

	template<typename R>
		constexpr auto minevalhelp(R &&best) {
			return best;
		}

	template<typename E, typename... Es,typename R>
		constexpr auto minevalhelp(R &&best, E &&e, Es &&...es) {
			return minevalhelp(help::min(best,e()),std::forward<Es>(es)...);
		}

	template<typename I1,typename I2,typename R>
		constexpr R minevalhelp2(R &&best, I1 &&i, I2 &&e) {
			return i==e ? best : minevalhelp2(help::min(best,(*i)()),i+1,std::forward<I2>(e));
		}

	template<typename L>
		constexpr auto mineval(L &&l) ->
			typename std::enable_if<help::is_container<L>::value,
				decltype(minevalhelp2((*begin(l))(),begin(l)+1,end(l)))>::type {
			return minevalhelp2((*begin(l))(),begin(l)+1,end(l));
		}

	template<typename E, typename... Es>
		constexpr auto mineval(E &&e, Es &&...es) ->
			typename std::enable_if<!help::is_container<E>::value,
				decltype(minevalhelp(e(),std::forward<Es>(es)...))>::type {
			return minevalhelp(e(),std::forward<Es>(es)...);
		}

	template<typename R>
		constexpr auto maxevalhelp(R &&best) {
			return best;
		}

	template<typename E, typename... Es,typename R>
		constexpr auto maxevalhelp(R &&best, E &&e, Es &&...es) {
			return maxevalhelp(help::max(best,e()),std::forward<Es>(es)...);
		}

	template<typename I1,typename I2,typename R>
		constexpr R maxevalhelp2(R &&best, I1 &&i, I2 &&e) {
			return i==e ? best : maxevalhelp2(help::max(best,(*i)()),i+1,std::forward<I2>(e));
		}

	template<typename L>
		constexpr auto maxeval(L &&l) ->
			typename std::enable_if<help::is_container<L>::value,
				decltype(maxevalhelp2((*begin(l))(),begin(l)+1,end(l)))>::type {
			return maxevalhelp2((*begin(l))(),begin(l)+1,end(l));
		}

	template<typename E, typename... Es>
		constexpr auto maxeval(E &&e, Es &&...es) ->
			typename std::enable_if<!help::is_container<E>::value,
				decltype(maxevalhelp(e(),std::forward<Es>(es)...))>::type {
			return maxevalhelp(e(),std::forward<Es>(es)...);
		}
}


#endif
