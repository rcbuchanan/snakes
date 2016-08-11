#ifndef PAIR_HH_
#define PAIR_HH_
template<typename T, typename U>
class pair<T, U> {
public:
	pair () {}
	pair (T tt , U uu) : t (tt), u (uu) {}
	T t;
	U u;
};

#endif
