#include <glib.h>
#include <cstring>
#include <sstream>
#include <string>
#include "GatewayPathfinding/Gateway.h"
#include "GatewayPathfinding/GatewayMap.h"

struct point
{
	int x;
	int y;
};

struct gate
{
	point start;
	point end;
};

extern "C"
{

void texnet_parse
	( guint8 *contents
	, int contents_length
	, gate **gates
	, int *gates_length
	, GError **err
	)
{
	g_return_if_fail (err == NULL || *err == NULL);

	try
	{
		std::string str (reinterpret_cast <const char *> (contents), contents_length);
		std::istringstream fin (str);
		GatewayPathfinding::GatewayMap map (fin);

		GatewayPathfinding::Gateway *tmp_gates = map.getGates();

		*gates_length = map.getGateCount();
		g_return_if_fail (tmp_gates != NULL);

		cout << "*gates_length: " << *gates_length << endl;
		cout << "sizeof (**gates): " << sizeof (**gates) << endl;
		size_t gates_size = *gates_length * sizeof (**gates);
		*gates = static_cast <gate *> (g_malloc (gates_size));

		for (size_t i = 0; i < *gates_length; ++i)
		{
			(*gates)[i].start.x = tmp_gates[i].getStart ().x;
			(*gates)[i].start.y = tmp_gates[i].getStart ().y;
			(*gates)[i].end.x = tmp_gates[i].getEnd ().x;
			(*gates)[i].end.y = tmp_gates[i].getEnd ().y;
			cout << i << " (" << (*gates)[i].start.x << ", " << (*gates)[i].start.y << ") - ("
					<< (*gates)[i].end.x << ", " << (*gates)[i].end.y << ")" << endl;
		}
		cout << "*gates_length: " << *gates_length << endl;
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
}

}
