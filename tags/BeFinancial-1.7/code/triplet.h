template<class a, class b, class c> class triplet
{
public:
	a first;
	b second;
	c third;
	triplet(a firstarg, b secondarg, c thirdarg) : 
		first(firstarg), second(secondarg),
		third(thirdarg) {}
};