#include "Objects.h"
#include "container.h"
#include "kran.h"

Objects::Objects()
{
	for (int i = 0; i < 1; i++)
	{
		container contemp;
		contemp.x = i;
		listContainer.push_back(contemp);
	}
	kra;
}

Objects objects;