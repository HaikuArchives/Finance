template<class a, class b, class c, class d> class quadruplet
{
public:
	a first;
	b second;
	c third;
	d fourth;
	quadruplet(a firstarg, b secondarg, c thirdarg, d fourtharg) : 
		first(firstarg), second(secondarg),
		third(thirdarg), fourth(fourtharg) {}
};