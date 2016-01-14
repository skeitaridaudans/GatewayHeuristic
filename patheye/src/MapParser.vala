namespace Patheye
{

class MapParser
{
	[CCode (cname = "texmap_parse")]
	private extern static uint8[] texmap_parse
		( uint8[] contents
		, out int width
		, out int height
		)
		throws Error;

	public static Map parse (uint8[] contents)
		throws Error
	{
		int width;
		int height;
		uint8[] map = texmap_parse (contents, out width, out height);

		return new Map.with_argb (contents, map, width, height);
	}
}

}
