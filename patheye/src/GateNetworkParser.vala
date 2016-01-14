namespace Patheye
{

class GateNetworkParser
{
	[CCode (cname = "texnet_parse")]
	public extern static void texnet_parse
		( uint8[] contents
		, out Gate[] gates
		)
		throws Error;

	[CCode (cname = "texnet_save")]
	public extern static void texnet_save
		( string path
		, uint8[] mapraw
		, Gate[] network
		)
		throws Error;

	public static GateNetwork parse (uint8[] contents)
		throws Error
	{
		Gate[] gates;

		texnet_parse (contents, out gates);

		return new GateNetwork.with_gates (gates);
	}

	public static void save (string path, Map map, GateNetwork network)
		throws Error
	{
		texnet_save (path, map.raw, network.gates);
	}
}

}
