#include <cstring>
#include <glib.h>
#include "parser.hpp"

using namespace wiz::texmap;

extern "C"
{

guint8 *texmap_parse
	( guint8* contents
	, int contents_length
	, gint *width
	, gint *height
	, int* result_length
	, GError **err
	)
{
	g_return_val_if_fail (err == NULL || *err == NULL, FALSE);

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

		return nullptr;
	}

	auto texdata = static_cast<guint8 *>
		(g_malloc (p.m.data.size ()));

	memcpy (texdata, &p.m.data[0], p.m.data.size ());

	*width = p.m.width;
	*height = p.m.height;
	*result_length = p.m.data.size ();

	return texdata;
}

}
