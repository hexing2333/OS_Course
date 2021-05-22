#include "FCB.h"

FCB::FCB(char newtype, string newname, int newsize,/* vector<int> newlocation,*/ FCB* newparent, FCB* newchild, FCB* newsibling)
{
	name = newname;
	type = newtype;
	parent = newparent;
	child = newchild;
	sibling = newsibling;
	size = newsize;
	/*location.swap(newlocation);*/
}
