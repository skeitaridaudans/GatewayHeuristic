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

void texnet_save
	( const char *path
	, guint8 *mapraw
	, int mapraw_length
	, gate *gates
	, int gates_length
	, GError **err
	)
{
	g_return_if_fail (err == NULL || *err == NULL);

	try
	{
		std::string str (reinterpret_cast <const char *> (mapraw), mapraw_length);
		std::istringstream fin (str);

		Pathfinding::TraversibleMap *tmap = new Pathfinding::TraversibleMap (fin);
		GatewayPathfinding::GatewayMap map (tmap);
		GatewayPathfinding::Gateway *gateways = new GatewayPathfinding::Gateway[gates_length];

		for (int i = 0; i < gates_length; ++i)
		{
			gateways[i] = GatewayPathfinding::Gateway
				( Pathfinding::Tile (gates[i].start.x, gates[i].start.y)
				, Pathfinding::Tile (gates[i].end.x, gates[i].end.y)
				);
		}

		map.setGates (gateways, gates_length);
		map.calculateZones ();
		map.writeMapData (path);
	}
	catch (std::exception &e)
	{
		g_set_error
			( err
			, g_quark_from_static_string ("save-error")
			, 0
			, "%s"
			, e.what ()
			);

		return;
	}
}

}
