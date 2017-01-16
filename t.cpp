#include "inlinegt.hpp"
#include <iostream>
#include <vector>

using namespace std;

template<typename T=std::size_t>
struct zeroto {
	struct iterator {
		template<typename TT>
		iterator(TT &&v) : x(std::forward<TT>(v)) {}
		const T &operator*() const { return x; }
		const T *operator->() const { return &x; }
		iterator& operator++() {
			++x;
               return *this;
          }
          const iterator operator++(int) {
               iterator ret(*this);
               ++(*this);
               return ret;
          }
		bool operator==(const iterator &i) const { return x==i.x; }
		bool operator!=(const iterator &i) const { return x!=i.x; }
		T x;
	};
	constexpr iterator begin() { return iterator(0); }
	constexpr iterator end() { return iterator(endx); }

	constexpr zeroto() : endx(std::numeric_limits<T>::max()) {}
	constexpr zeroto(const T &iend) : endx(iend) {}
	constexpr zeroto(T &&iend) : endx(std::move(iend)) {}
	T endx;
};

int main(int argc, char **argv) {
	sstate<> ss;
	vector<double> leaves = {0,1,-1,-2,4,5,0,3,-2,1,-5,-7,1,2,3,4};

	maxpick(ss,int i1 : zeroto<>(2)) {
		minpick(ss,int i2 : zeroto<>(2)) {
			maxpick(ss,int i3 : zeroto<>(2)) {
				minpick(ss,int i4 : zeroto<>(2)) {
					ss.set(leaves[i1*8+i2*4+i3*2+i4]);
					cout << "\t\t\t\t" << i4 << ": " << ss.ret << endl;
				}
				cout << "\t\t\t" << i3 << ": " << ss.ret << endl;
			}
			cout << "\t\t" << i2 << ": " << ss.ret << endl;
		}
		cout << "\t" << i1 << ": " << ss.ret << endl;
	}

	cout << ss.ret << endl;
}
				
