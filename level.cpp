#include "shooter.h"
#include "level.h"

static level_t level;

level_t *level_get ()
{
	return &level;
}

bool level_load (const char *s)
{
	LOGI ("Loading %s\n", s);
	string l = "";
	ifstream f;

	f.open (s);
	
	if (!f.is_open()) {
		LOGI ("File %s not found!\n", s);
		return false;
	}

	vector<string> lines;

	unsigned x = 0, y = 0;

	while (!f.eof ()) {
		getline (f, l);

		y ++;

		if (x < l.length())
			x = l.length();
		
		//LOGI (l.c_str ());

		lines.push_back (l);
	}

	f.close ();

	LOGI ("Level dimension: %d*%d\n", x, y);

	level.dim_x = x;
	level.dim_y = y;

	level.data = (unsigned char *) calloc (x * y, sizeof (unsigned char));	

	for (y = 0; y < lines.size (); y ++) {
		for (unsigned i = 0; i < lines[y].length (); i ++) {
			level.data[(y*x) + i] = lines[y].c_str()[i];
		}		
	}

	return true;
}

bool level_init ()
{
	level.dim_x = 0;
	level.dim_y = 0;

	string lname = PATH_LEVEL;
	lname += "1.map";

	return level_load (lname.c_str());
}

void level_deinit ()
{
	free (level.data);
}
