#include <cstring>
#include <glib.h>
#include "parser.hpp"

using namespace wiz::mdlpath;

extern "C"
{

struct segment
{
	point opened;
	point *closed;
	int closed_length;
	int dummy;
};

void texpath_parse
	( guint8* contents
	, int contents_length
	, segment **segments
	, int *segments_length
	, point **path
	, int *path_length
	, GError **err
	)
{
	g_return_if_fail (err == NULL || *err == NULL);

	parser p;

	try
	{
		for (size_t i = 0; i < contents_length; ++i)
		{
			p.feed (contents[i]);
		}

		p.eof ();
	}
	catch (std::exception &e)
	{
		g_set_error
			( err
			, g_quark_from_static_string ("parse-error")
			, 0
			, "%s"
			, e.what ()
			);

		return;
	}

	size_t segments_size = p.p.segments.size () * sizeof (segment);
	*segments = static_cast<segment *> (g_malloc (segments_size));
	*segments_length = p.p.segments.size ();

	for (size_t i = 0; i < p.p.segments.size (); ++i)
	{
		std::pair<point, std::vector<point>> segment = p.p.segments[i];

		(*segments)[i].opened = segment.first;

		size_t closed_size = segment.second.size () * sizeof (point);
		(*segments)[i].closed = static_cast<point *> (g_malloc (closed_size));

		memcpy ((*segments)[i].closed, &segment.second[0], closed_size);

		(*segments)[i].closed_length = segment.second.size ();
		(*segments)[i].dummy = (*segments)[i].closed_length;
	}

	size_t path_size = p.p.path.size () * sizeof (point);
	*path = static_cast<point *> (g_malloc (path_size));
	*path_length = p.p.path.size ();

	memcpy (*path, &p.p.path[0], path_size);
}

}
